#include "bdc_cage/src/pch.h"
#include "bdc_cage/include/system_cage.h"
#include "mono_identity/mono_identity.h"
#include <iostream>
#include <random>

#pragma comment(lib, "mono_transform.lib")
#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_delta_time.lib")
#pragma comment(lib, "mono_identity.lib")

bool CrossedGridLine(
    float prevPos,
    float nowPos,
    float stageSize,
    float cageSize,
    int numDivisions)
{
    float range = stageSize - cageSize;
    float halfRange = range * 0.5f;
    float step = range / static_cast<float>(numDivisions);

    float prevIdx = (prevPos + halfRange) / step;
    float nowIdx = (nowPos + halfRange) / step;

    int prevGrid = static_cast<int>(std::floor(prevIdx + 0.5f));
    int nowGrid = static_cast<int>(std::floor(nowIdx + 0.5f));

    return prevGrid != nowGrid;
}

bdc_cage::SystemCage::SystemCage()
{
}

bdc_cage::SystemCage::~SystemCage()
{
}

bool bdc_cage::SystemCage::Update
(
    riaecs::IECSWorld& ecsWorld,
    riaecs::IAssetContainer& assetCont,
    riaecs::ISystemLoopCommandQueue& systemLoopCmdQueue
) {
    for (const riaecs::Entity& entity : ecsWorld.View(bdc_cage::ComponentCageID())())
    {
        mono_identity::ComponentIdentity* identity
            = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
                ecsWorld, entity, mono_identity::ComponentIdentityID(), "Identity", RIAECS_LOG_LOC);

        // Skip if identity component is not active
        if (!identity->IsActiveSelf())
            continue;

        bdc_cage::ComponentCage* cageComp
            = riaecs::GetComponentWithCheck<bdc_cage::ComponentCage>(
                ecsWorld, entity, bdc_cage::ComponentCageID(), "Cage", RIAECS_LOG_LOC);

        mono_transform::ComponentTransform* transform
            = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
                ecsWorld, entity, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        deltaTimeProvider_.UpdateTime();
        DirectX::XMFLOAT3 pos = bdc_cage::updateCagePos(cageComp, transform, cageMoveDir_, STAGE_SIZE, deltaTimeProvider_.GetDeltaTime());

        transform->SetPos(pos,ecsWorld);
    }
    return true;
}

BDC_CAGE_API DirectX::XMFLOAT3 bdc_cage::updateCagePos(
    bdc_cage::ComponentCage* cageComp,
    mono_transform::ComponentTransform* transform,
    GridDirection& cageMoveDir,
    float stageSize,
    float dt)
{
    // ---- 位置取得 ----
    DirectX::XMFLOAT3 currentPos = transform->GetPos();
    DirectX::XMFLOAT3 prevPos = cageComp->GetPrevPos();

    // ---- 次の位置（未来）を計算 ----
    DirectX::XMFLOAT3 nextPos = currentPos;
    if (cageComp->IsMoving())
    {
        nextPos = MoveInDirection(
            transform,
            cageMoveDir,
            cageComp->GetMoveSpeed(),
            dt);
    }

    // ---- 到達判定（prev → next）----
    bool onGrid = ReachedTarget(
        prevPos,
        nextPos,
        stageSize,
        cageComp->GetCageSize(),
        cageComp->GetNumDivisions());

    bool justArrived = (onGrid && !cageComp->GetWasOnGrid());

    // ---- 到達した瞬間 ----
    if (justArrived)
    {
        DirectX::XMFLOAT3 snapped =
            GetGridByPosition(
                nextPos,
                stageSize,
                cageComp->GetCageSize(),
                cageComp->GetNumDivisions());

        cageComp->ResetCellTimer();
        cageComp->SetIsMoving(false);
        cageComp->SetWasOnGrid(true);
        cageComp->SetPrevPos(snapped);
        return snapped;
    }

    // ---- タイマー更新 ----
    cageComp->AddCellTimer(dt);

    // ---- グリッド上で停止中 ----
    if (onGrid && !cageComp->IsMoving())
    {
        DirectX::XMFLOAT3 snapped =
            GetGridByPosition(
                currentPos,
                stageSize,
                cageComp->GetCageSize(),
                cageComp->GetNumDivisions());

        if (cageComp->GetCellTimer() < cageComp->GetPerCellDelay())
        {
            cageComp->SetWasOnGrid(true);
            cageComp->SetPrevPos(snapped);
            return snapped;
        }

        cageComp->SetIsMoving(true);
        cageMoveDir = DecideNextDirection(cageComp, snapped, stageSize);
    }

    // ---- 移動 or 停止の確定 ----
    cageComp->SetWasOnGrid(onGrid);
    cageComp->SetPrevPos(nextPos);
    return nextPos;
}


BDC_CAGE_API bdc_cage::GridDirection bdc_cage::DecideNextDirection(bdc_cage::ComponentCage* cageComp, DirectX::XMFLOAT3 NowPos, float StageSize)
{
    DirectX::XMFLOAT3 targetGrid =
        GetGridByPosition(cageComp->GetTargetPos(),
            StageSize,
            cageComp->GetCageSize(),
            cageComp->GetNumDivisions());

    float dx = targetGrid.x - NowPos.x; // +ならEast方向へ近づく
    float dz = targetGrid.z - NowPos.z; // +ならNorth方向へ近づく

    float stageEnd = (StageSize - cageComp->GetCageSize()) / 2.0f;
    float movableRange = StageSize - cageComp->GetCageSize();
    float cellSize = movableRange / cageComp->GetNumDivisions();

    // まずは全方向を候補に入れる
    std::vector<GridDirection> dirs = {
        GridDirection::East,
        GridDirection::West,
        GridDirection::North,
        GridDirection::South
    };

    //壁の方向を削る
    {
        auto round01 = [](float v) {
            return std::roundf(v * 10.0f) / 10.0f;
            };

        auto canMove = [&](GridDirection dir) {
            float nx = NowPos.x;
            float nz = NowPos.z;

            if (dir == GridDirection::East)  nx += cellSize;
            if (dir == GridDirection::West)  nx -= cellSize;
            if (dir == GridDirection::North) nz += cellSize;
            if (dir == GridDirection::South) nz -= cellSize;

            float rx = round01(nx);
            float rz = round01(nz);

            float rStageEnd = round01(stageEnd);

            bool can = (rx >= -rStageEnd && rx <= rStageEnd &&
                rz >= -rStageEnd && rz <= rStageEnd);

            return can;
            };


        // 壁に当たる方向を削る
        dirs.erase(
            std::remove_if(dirs.begin(), dirs.end(),
                [&](GridDirection d) { return !canMove(d); }),
            dirs.end()
        );
    }

    if (cageComp->GetRoamCount() <= 0) {

        // X方向が既に一致していたら East/West を削る
        if (std::abs(dx) < 0.0001f) {
            dirs.erase(
                std::remove_if(dirs.begin(), dirs.end(),
                    [&](GridDirection d) {
                        return d == GridDirection::East ||
                            d == GridDirection::West;
                    }),
                dirs.end()
            );
        }

        // Z方向が一致していたら North/South を削る
        if (std::abs(dz) < 0.0001f) {
            dirs.erase(
                std::remove_if(dirs.begin(), dirs.end(),
                    [&](GridDirection d) {
                        return d == GridDirection::North ||
                            d == GridDirection::South;
                    }),
                dirs.end()
            );
        }

    // RoamCount が 0 → 遠ざかる方向は禁止
        dirs.erase(
            std::remove_if(dirs.begin(), dirs.end(),
                [&](GridDirection d) {
                    if (d == GridDirection::East && dx < 0) return true; // 東へ行くと遠ざかる
                    if (d == GridDirection::West && dx > 0) return true; // 西へ行くと遠ざかる
                    if (d == GridDirection::North && dz < 0) return true; // 北へ行くと遠ざかる
                    if (d == GridDirection::South && dz > 0) return true; // 南へ行くと遠ざかる
                    return false;
                }),
            dirs.end()
        );
    }

    // 候補ゼロ = 動けない
    if (dirs.empty()) {
        return GridDirection::none; // or None を入れるなら None を作る
    }

    // ---- ランダム選択 ----
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, (int)dirs.size() - 1);

    GridDirection chosen = dirs[dist(rng)];

    // chosen がターゲットから遠ざかるなら RoamCount を減らす
    auto isWeirdMove = [&](GridDirection d) {
        if (d == GridDirection::East && dx < 0) return true;
        if (d == GridDirection::West && dx > 0) return true;
        if (d == GridDirection::North && dz < 0) return true;
        if (d == GridDirection::South && dz > 0) return true;
        return false;
        };

    if (isWeirdMove(chosen)) {
        cageComp->SetRoamCount(cageComp->GetRoamCount() - 1);
    }

    return chosen;
}

BDC_CAGE_API DirectX::XMFLOAT3 bdc_cage::MoveInDirection(
    mono_transform::ComponentTransform* transform,
    bdc_cage::GridDirection dir,
    float speed,
    float dt)
{
    DirectX::XMFLOAT3 pos = transform->GetPos();
    float dx = 0.0f;
    float dz = 0.0f;

    // GridDirection → 動かす方向
    switch (dir)
    {
    case GridDirection::North: // +Z
        dz = 1.0f;
        break;
    case GridDirection::South: // -Z
        dz = -1.0f;
        break;
    case GridDirection::East: // +X
        dx = 1.0f;
        break;
    case GridDirection::West: // -X
        dx = -1.0f;
        break;
    }

    // 実際の移動
    pos.x += dx * speed * dt;
    pos.z += dz * speed * dt;

    return pos;
}

BDC_CAGE_API DirectX::XMFLOAT3 bdc_cage::GetGridByPosition(DirectX::XMFLOAT3 targetPos, float StageSize, float cageSize, int numDivisions)
{
    float canMoveSquareSize = StageSize - cageSize;

    float halfSize = canMoveSquareSize / 2.0f;
    float step = canMoveSquareSize / static_cast<float>(numDivisions); // 分割幅

    auto roundToGridLine = [&](float coord) -> float {
        // coord を 0～numDivisions のインデックスに変換
        float index = (coord + halfSize) / step;

        int nearestIndex = static_cast<int>(index + 0.5f); // 四捨五入

        // インデックスをクランプ
        if (nearestIndex < 0) nearestIndex = 0;
        if (nearestIndex > numDivisions) nearestIndex = numDivisions;

        // 交点の位置へ変換
        return -halfSize + step * nearestIndex;
        };

    DirectX::XMFLOAT3 result;
    result.x = roundToGridLine(targetPos.x);
    result.y = 0;
    result.z = roundToGridLine(targetPos.z);

    return result;

}

BDC_CAGE_API bool bdc_cage::ReachedTarget(
    DirectX::XMFLOAT3 prevPos,
    DirectX::XMFLOAT3 nowPos,
    float StageSize,
    float cageSize,
    int numDivisions)
{
    float range = StageSize - cageSize;
    float halfRange = range * 0.5f;
    float step = range / static_cast<float>(numDivisions);

    auto reachedAxis = [&](float prev, float now)
    {
        float prevIdx = (prev + halfRange) / step;
        float nowIdx  = (now  + halfRange) / step;

        float rounded = std::round(nowIdx);
        float snapped = -halfRange + rounded * step;

        // ① 線の近くにいる
        bool nearLine = std::fabs(now - snapped) < 0.01f;

        // ② 線を跨いだ
        bool crossed = (prevIdx - rounded) * (nowIdx - rounded) < 0.0f;

        return nearLine || crossed;
    };

    return reachedAxis(prevPos.x, nowPos.x)
        && reachedAxis(prevPos.z, nowPos.z);
}

BDC_CAGE_API riaecs::SystemFactoryRegistrar
<bdc_cage::SystemCage> bdc_cage::SystemCageID;
