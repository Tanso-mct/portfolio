#pragma once

#include "bdc_cage/include/dll_config.h"
#include "riaecs/riaecs.h"
#include "mono_input_monitor/include/keyboard_monitor.h"

#include <DirectXMath.h>

namespace bdc_cage
{
	constexpr size_t ComponentCageMaxCount = 5;

    class BDC_CAGE_API ComponentCage
    {
    private:
        // --- 移動目標 ---
        DirectX::XMFLOAT3 targetPos_ = { 0, 0, 0 };   // 移動先の座標
        bool isMoving_ = false;                         // 移動中フラグ
        float moveSpeed_ = 6.0f;                        // 1秒あたりの移動速度

        // --- 停止管理 ---
        float perCellDelay = 2.0f;                      // 1回の停止時間（秒）
        float cellTimer = 0.0f;                         // 停止中の経過時間

        // --- ケージやランダム移動用 ---
        float cageSize_ = 15.0f;                        // ケージサイズ
        int roamCount_ = 0;                             // ランダム移動の回数制御
        int numDivisions_ = 6;                          // ステージの分割数（グリッド数）

        bool wasOnGrid_ = false;

        DirectX::XMFLOAT3 prevPos_ = { 0, 0, 0 };

    public:
        ComponentCage() = default;
        ~ComponentCage() = default;

        struct SetupParam
        {
            DirectX::XMFLOAT3 targetPos_ = { 0,0,0 };
            bool isMoving_ = false;
            float moveSpeed_ = 6.0f;
            float cageSize_ = 15.0f;
            int roamCount_ = 0;
            int numDivisions_ = 6;
            float perCellDelay = 2.0f;
        };
        void Setup(const SetupParam& param)
        {
            targetPos_ = param.targetPos_;
            isMoving_ = param.isMoving_;
            moveSpeed_ = param.moveSpeed_;
            cageSize_ = param.cageSize_;
            roamCount_ = param.roamCount_;
            numDivisions_ = param.numDivisions_;
            perCellDelay = param.perCellDelay;
        }

        // --- Target ---
        void SetTargetPos(const DirectX::XMFLOAT3& pos) { targetPos_ = pos; }
        DirectX::XMFLOAT3 GetTargetPos() const { return targetPos_; }

        void SetPrevPos(const DirectX::XMFLOAT3& pos) { prevPos_ = pos; }
        DirectX::XMFLOAT3 GetPrevPos() const { return prevPos_; }

        // --- Moving ---
        void SetIsMoving(bool m) { isMoving_ = m; }
        bool IsMoving() const { return isMoving_; }

        // --- Move Speed ---
        void SetMoveSpeed(float s) { moveSpeed_ = s; }
        float GetMoveSpeed() const { return moveSpeed_; }

        // --- Timers ---
        void ResetCellTimer() { cellTimer = 0.0f; }
        void AddCellTimer(float dt) { cellTimer += dt; }
        float GetCellTimer() const { return cellTimer; }
        float GetPerCellDelay() const { return perCellDelay; }

        // --- Divisions ---
        void SetNumDivisions(int n) { numDivisions_ = n; }
        int GetNumDivisions() const { return numDivisions_; }

        // --- ケージ / ランダム移動用（今は未使用） ---
        void SetCageSize(float s) { cageSize_ = s; }
        float GetCageSize() const { return cageSize_; }
        void SetRoamCount(int c) { roamCount_ = c; }
        int GetRoamCount() const { return roamCount_; }

        void SetWasOnGrid(bool b) { wasOnGrid_ = b; };
        bool GetWasOnGrid() { return wasOnGrid_; };
    };


	extern BDC_CAGE_API riaecs::ComponentRegistrar
		<ComponentCage, ComponentCageMaxCount> ComponentCageID;
}
