using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Player : MonoBehaviour
{
    // プレイヤーの第何人称
    [SerializeField] private int viewMode = 1;

    // プレイヤーのパーツら
    [SerializeField] private GameObject parts;
    [SerializeField] private GameObject partsSub;

    // PartsのGameObject
    [SerializeField] private List<GameObject> partsList;

    // PartsのHead
    [SerializeField] private GameObject head;

    // Partsの右腕
    [SerializeField] private GameObject rightArm;

    // PartsのHeadにある右腕オブジェクト
    [SerializeField] private GameObject canvasRightArm;
    [SerializeField] private GameObject canvasRightArmIdle;


    // PartsのHeadにあるHoldingItemオブジェクト
    [SerializeField] private GameObject holdingObj;
    [SerializeField] private GameObject holdingObjIdle;

    // プレイヤーのカメラ
    [SerializeField] public Camera cam;

    // 当たり判定管理クラス
    [SerializeField] private McHitBoxAdmin hitBoxAdmin;
    private int hitBoxId;

    // Vaxel管理クラス
    [SerializeField] private BlockAdmin blockAdmin;
    
    // プレイヤーのリーチ
    [SerializeField] private float reach = 5f;
    public float Reach {get { return reach; }}

    // プレイヤーのマテリアル
    [SerializeField] private Material mat;

    // テクスチャ
    [SerializeField] private Texture2D texture;

    // プレイヤーの回転角度
    private Vector2 rot;
    public Vector2 Rot
    {
        get { return rot; }
    }

    // プレイヤーの現在座標
    private Vector3 pos;
    public Vector3 Pos
    {
        get { return pos; }
    }

    // プレイヤーのBoxCollider
    private BoxCollider bc;
    private float collisionOffset = 0.2f;

    // プレイヤーのRigidbody
    private Rigidbody rb;

    // プレイヤーの現在のスピード
    private float speed = 0.0f;

    // プレイヤーの各種スピード
    [SerializeField] private float walkingSpeed = 7.0f;
    [SerializeField] private float runningSpeed = 10.0f;
    [SerializeField] private float walkFlySpeedAspect = 0.5f;
    [SerializeField] private float runFlySpeedAspect = 1f;


    // プレイヤーのジャンプ力
    [SerializeField] private float jumpForce = 5.0f;

    // プレイヤーの地面に接地しているかどうか
    private bool isGrounded = true;

    // プレイヤーが着地したかどうか
    private bool isLanded = false;

    // プレイヤーが走っているかどうか
    private bool isRunning = false;

    // プレイヤーが空中にいるかどうか
    private bool isFlying = false;

    // ジャンプベクトル
    private Vector3 jumpVec = Vector3.zero;

    // 慣性の強さ
    [SerializeField] private float inertia = 0.9f;

    // プレイヤーの移動ベクトル
    private Vector3 movement = Vector3.zero;

    // 走っている際の視野角
    [SerializeField] private float diffRunningFov = 30.0f;

    // プレイヤーの移動方向を回転
    Quaternion p180Rot = Quaternion.Euler(0, 180f, 0);
    Quaternion p90Rot = Quaternion.Euler(0, 90f, 0);
    Quaternion p45Rot = Quaternion.Euler(0, 45f, 0);
    Quaternion m90Rot = Quaternion.Euler(0, -90f, 0);
    Quaternion m45Rot = Quaternion.Euler(0, -45f, 0);

    // アニメーター
    private Animator animParts;
    private Animator animSub;
    private Animator animRightArm;
    private Animator animHolding;

    // ブロックのセレクター
    [SerializeField] private GameObject selector;
    [SerializeField] private List<GameObject> selectorParts;
    [SerializeField] private Texture2D selectorTexture;
    [SerializeField] private List<Texture2D> destroyStageTextures;

    // フレーム内で設置したブロックのデータ
    [HideInInspector] public bool isFrameSetBlock = false;
    [HideInInspector] public Vector4 frameSetBlocks;

    // フレーム内で破壊したブロックのデータ
    [HideInInspector] public bool isFrameDestroyBlock = false;
    [HideInInspector] public Vector4 frameDestroyBlocks;

    // 最後に設置したブロックについて
    int lastSetFrame = 0;

    // 現在破壊しているブロックについて
    private bool isDestroying = false;
    private float destroyProgress = 0f;
    private float blockDurability = 10f;
    private Vector4 destroyingBlock;

    // コンテナら
    [SerializeField] private Inventory inventory;
    [SerializeField] private HotBar hotBar;
    [SerializeField] private CreativeContainer creativeItemContainer;
    [SerializeField] private CreativeContainer creativeItemContainer2;

    // 何らかのインベントリを開いているかどうか
    public bool isInventoryOpen = false;

    [SerializeField] private GameObject triggerHitBoxObj;

    [SerializeField] private McSounds mcSounds = null;

    private bool isFastFalling = false;
    [SerializeField] private float fastFallSpeed = 10.0f;

    public void Init()
    {
        // プレイヤーの初期位置と回転角度
        pos = transform.position;
        rot = parts.transform.rotation.eulerAngles;

        triggerHitBoxObj.SetActive(true);
        triggerHitBoxObj.transform.position = pos;

        // プレイヤーのBoxColliderを取得
        bc = GetComponent<BoxCollider>();

        // プレイヤーのRigidbodyを取得
        rb = GetComponent<Rigidbody>();

        // カーソルをロック
        McControls.CursorLock(true);

        // コンテナの初期化
        inventory.Init();
        hotBar.Init();
        creativeItemContainer.Init();
        creativeItemContainer2.Init();

        // アニメーターの初期化
        animParts = parts.GetComponent<Animator>();
        animSub = partsSub.GetComponent<Animator>();
        animRightArm = canvasRightArm.GetComponent<Animator>();
        animHolding = holdingObj.GetComponent<Animator>();

        // プレイヤーの1人称視点での初期化
        if (viewMode == 1)
        {
            for (int i = 0; i < partsList.Count; i++)
            {
                partsList[i].SetActive(false);
            }
            partsSub.SetActive(false);
        }
        else
        {
            parts.SetActive(true);
            partsSub.SetActive(true);

            canvasRightArmIdle.SetActive(false);
            canvasRightArm.SetActive(false);

            holdingObj.SetActive(false);
            holdingObjIdle.SetActive(false);
        }

        // テクスチャを設定
        mat.mainTexture = texture;

        // セレクターのテクスチャを設定
        selectorParts = SupportFunc.GetChildren(selector);
        for (int i = 0; i < selectorParts.Count; i++)
        {
            selectorParts[i].GetComponent<MeshRenderer>().material.mainTexture = selectorTexture;
        }

        // プレイヤーの当たり判定を設定
        hitBoxId = hitBoxAdmin.RegisterHitBox(pos, bc.size, Vector3.zero);
    }

    public void Create(Vector3 pos)
    {
        transform.position = pos;
        this.pos = pos;
    }

    public void Load()
    {

    }

    private void ViewUpdate(ref List<Vector4> targetBlocks)
    {
        if (isInventoryOpen) return;

        // プレイヤーの視点移動
        Vector2 mouseAxis = McControls.GetMouseAxis();

        rot.x = Mathf.Clamp(rot.x + mouseAxis.y, -90f, 90f);
        rot.y += mouseAxis.x;

        parts.transform.rotation = Quaternion.Euler(0.0f, rot.y, 0.0f);

        partsSub.transform.rotation = Quaternion.Euler(partsSub.transform.rotation.x, rot.y, partsSub.transform.rotation.z);
        head.transform.localRotation = Quaternion.Euler(rot.x, 0.0f, 0.0f);

        // セレクター位置更新
        if (targetBlocks[Constants.TARGET_BLOCK_SELECT].w != 0)
        {
            selector.SetActive(true);
            selector.transform.position = new Vector3
            (
                targetBlocks[Constants.TARGET_BLOCK_SELECT].x - Constants.WORLD_HALF_SIZE, 
                targetBlocks[Constants.TARGET_BLOCK_SELECT].y, 
                targetBlocks[Constants.TARGET_BLOCK_SELECT].z - Constants.WORLD_HALF_SIZE
            );
        }
        else
        {
            selector.SetActive(false);
        }
    }

    private void MoveUpdate()
    {
        if (isInventoryOpen) return;

        if (McControls.IsKey(Constants.CONTROL_SPRINT)) isRunning = true;

        int isFor = (McControls.IsKey(Constants.CONTROL_FOR)) ? 1 : 0;
        int isBack = (McControls.IsKey(Constants.CONTROL_BACK)) ? 1 : 0;
        int isLeft = (McControls.IsKey(Constants.CONTROL_LEFT)) ? 1 : 0;
        int isRight = (McControls.IsKey(Constants.CONTROL_RIGHT)) ? 1 : 0;

        // 移動方向を取得
        int vertical = isFor + isBack;
        int horizontal = isLeft + isRight;
        int diagonal = vertical + horizontal;

        if (isRunning && isFor == 1 && vertical == 1)
        {
            cam.fieldOfView = McVideos.Fov + diffRunningFov;
        }
        else
        {
            isRunning = false;
            cam.fieldOfView = McVideos.Fov;
        }

        // 移動ベクトル
        movement = Vector3.zero;

        // 移動ベクトルのZ成分を設定
        if (diagonal != 0) // 移動
        {
            if (isRunning && isFor == 1 && vertical == 1)
            {
                speed = runningSpeed;
                animParts.SetInteger(Constants.ANIM_TYPE, Constants.ANIM_PLAYER_RUN);
            }
            else
            {
                speed = walkingSpeed;
                animParts.SetInteger(Constants.ANIM_TYPE, Constants.ANIM_PLAYER_WALK);
            }

            movement.z = -speed;
        }
        else // 停止
        {
            isRunning = false;
            cam.fieldOfView = McVideos.Fov;

            speed = 0.0f;
            movement = Vector3.zero;
            animParts.SetInteger(Constants.ANIM_TYPE, Constants.ANIM_PLAYER_BREATH);
        }

        // 移動ベクトルを設定
        if (vertical == 2 || horizontal == 2) // 停止
        {
            movement = Vector3.zero;
        }
        else if (diagonal == 2) // 斜め移動
        {
            if (isFor == 1)
            {
                if (isLeft == 1) movement = m45Rot * movement;
                else if (isRight == 1) movement = p45Rot * movement;
            }
            else if (isBack == 1)
            {
                movement = p180Rot * movement;
                if (isLeft == 1) movement = p45Rot * movement;
                else if (isRight == 1) movement = m45Rot * movement;
            }
        }
        else if (vertical == 1)
        {
            if (isBack == 1) movement = p180Rot * movement;
        }
        else if (horizontal == 1)
        {
            if (isLeft == 1) movement = m90Rot * movement;
            else if (isRight == 1) movement = p90Rot * movement;
        }

        Quaternion rotate = Quaternion.Euler(0, parts.transform.eulerAngles.y, 0);
        movement = rotate * movement;
    }

    private void FlyUpdate()
    {
        // 飛び始めた際の移動ベクトルを設定
        if (!isFlying && !isGrounded)
        {
            isFlying = true;
            jumpVec = movement;
        }
        else if (isFlying && !isLanded)
        {
            int isFor = (McControls.IsKey(Constants.CONTROL_FOR)) ? 1 : 0;
            int isBack = (McControls.IsKey(Constants.CONTROL_BACK)) ? 1 : 0;
            int isLeft = (McControls.IsKey(Constants.CONTROL_LEFT)) ? 1 : 0;
            int isRight = (McControls.IsKey(Constants.CONTROL_RIGHT)) ? 1 : 0;

            // 移動方向を取得
            int vertical = isFor + isBack;
            int horizontal = isLeft + isRight;
            int diagonal = vertical + horizontal;

            // 移動ベクトル
            Vector3 newMovement = Vector3.zero;

            // 移動ベクトルのZ成分を設定
            if (diagonal != 0) // 移動
            {
                if (isRunning && isFor == 1 && vertical == 1)
                {
                    speed = runningSpeed * runFlySpeedAspect;
                    animParts.SetInteger(Constants.ANIM_TYPE, Constants.ANIM_PLAYER_RUN);
                }
                else
                {
                    speed = walkingSpeed * walkFlySpeedAspect;
                    animParts.SetInteger(Constants.ANIM_TYPE, Constants.ANIM_PLAYER_WALK);
                }

                newMovement.z = -speed;
            }

            // 移動ベクトルを設定
            if (diagonal == 2) // 斜め移動
            {
                if (isFor == 1)
                {
                    if (isLeft == 1) newMovement = m45Rot * newMovement;
                    else if (isRight == 1) newMovement = p45Rot * newMovement;
                }
                else if (isBack == 1)
                {
                    newMovement = p180Rot * newMovement;
                    if (isLeft == 1) newMovement = p45Rot * newMovement;
                    else if (isRight == 1) newMovement = m45Rot * newMovement;
                }
            }
            else if (vertical == 1)
            {
                if (isBack == 1) newMovement = p180Rot * newMovement;
            }
            else if (horizontal == 1)
            {
                if (isLeft == 1) newMovement = m90Rot * newMovement;
                else if (isRight == 1) newMovement = p90Rot * newMovement;
            }

            Quaternion rotate = Quaternion.Euler(0, parts.transform.eulerAngles.y, 0);
            newMovement = rotate * newMovement;

            movement = Vector3.Lerp(jumpVec, newMovement, inertia);
        }
        else if (isFlying && isLanded)
        {
            isFlying = false;
            isLanded = false;
        }
    }

    private void Jump()
    {
        if (isInventoryOpen) return;

        if (McControls.IsKey(Constants.CONTROL_JUMP) && isGrounded)
        {
            rb.AddForce(Vector3.up * jumpForce, ForceMode.Impulse);
            isGrounded = false;
        }
    }

    private void UpdateHitBox()
    {
        // プレイヤーの当たり判定を更新
        hitBoxAdmin.UpdatePos(hitBoxId, pos);
    }

    private void HandAnim()
    {
        if (viewMode != 1)
        {
            rightArm.SetActive(false);
            partsSub.SetActive(true);
            animSub.SetInteger(Constants.ANIM_TYPE, Constants.ANIM_PLAYER_USE);
        }
        else
        {
            if (hotBar.GetIsContain(hotBar.SelectingSlot) == 0)
            {
                canvasRightArmIdle.SetActive(false);
                canvasRightArm.SetActive(true);
                animRightArm.SetInteger(Constants.ANIM_TYPE, Constants.ANIM_PLAYER_USE);
            }
            else
            {
                holdingObjIdle.SetActive(false);
                holdingObj.SetActive(true);
                animHolding.SetInteger(Constants.ANIM_TYPE, Constants.ANIM_PLAYER_USE);
            }
        }
    }

    private void Attack(ref List<Vector4> targetBlocks)
    {
        if (isInventoryOpen) return;

        if (McControls.IsKey(Constants.CONTROL_ATTACK)) HandAnim();

        if (McControls.IsKey(Constants.CONTROL_ATTACK) && !isDestroying)
        {
            // ブロックの破壊
            if 
            (
                targetBlocks[Constants.TARGET_BLOCK_SELECT].w != 0 && 
                targetBlocks[Constants.TARGET_BLOCK_SELECT].w != Constants.BLOCK_TYPE_CANT_SET
            ){
                isDestroying = true;
                destroyProgress = 0f; // 破壊段階を初期化
                blockDurability = 100f; // ブロックの耐久値を取得
                destroyingBlock = targetBlocks[Constants.TARGET_BLOCK_SELECT];
            }
        }
        else if (McControls.IsKey(Constants.CONTROL_ATTACK))
        {
            if (isDestroying && targetBlocks[Constants.TARGET_BLOCK_SELECT].Equals(destroyingBlock))
            {
                destroyProgress += 20f;

                // 破壊段階に応じてテクスチャを変更
                int destroyStage = (int)((destroyProgress / blockDurability) * destroyStageTextures.Count);

                if (destroyStage < destroyStageTextures.Count)
                {
                    for (int i = 0; i < selectorParts.Count; i++)
                    {
                        selectorParts[i].GetComponent<MeshRenderer>().material.mainTexture = destroyStageTextures[destroyStage];
                    }
                }

                // 耐久が0以下になった場合はブロックを破壊
                if (destroyProgress >= blockDurability)
                {
                    isDestroying = false;
                    destroyProgress = 0f;

                    isFrameDestroyBlock = true;

                    // ブロックの破壊
                    blockAdmin.Break(targetBlocks[Constants.TARGET_BLOCK_SELECT], ref frameDestroyBlocks);

                    for (int i = 0; i < selectorParts.Count; i++)
                    {
                        selectorParts[i].GetComponent<MeshRenderer>().material.mainTexture = selectorTexture;
                    }
                }
            }
            else if (isDestroying && !targetBlocks[Constants.TARGET_BLOCK_SELECT].Equals(destroyingBlock))
            {
                isDestroying = false;
                destroyProgress = 0f;
                for (int i = 0; i < selectorParts.Count; i++)
                {
                    selectorParts[i].GetComponent<MeshRenderer>().material.mainTexture = selectorTexture;
                }
            }
        }
        else if (McControls.IsKeyUp(Constants.CONTROL_ATTACK))
        {
            isDestroying = false;
            destroyProgress = 0f;
            for (int i = 0; i < selectorParts.Count; i++)
            {
                selectorParts[i].GetComponent<MeshRenderer>().material.mainTexture = selectorTexture;
            }
        }
    }

    private bool IsBlockInPlayer(Vector4 target)
    {
        Vector3 boxOrigin = new Vector3
        (
            pos.x - bc.size.x / 2, pos.y, pos.z - bc.size.z / 2
        );

        Vector3 boxOriginFloatPart = new Vector3
        (
            boxOrigin.x - (float)((int)boxOrigin.x),
            boxOrigin.y - (float)((int)boxOrigin.y),
            boxOrigin.z - (float)((int)boxOrigin.z)
        );

        Vector3Int blockOrigin = new Vector3Int
        (
            (int)boxOrigin.x, (int)boxOrigin.y, (int)boxOrigin.z
        );

        if (boxOriginFloatPart.x >= 0.5) blockOrigin.x++;
        if (boxOriginFloatPart.y >= 0.5) blockOrigin.y++;
        if (boxOriginFloatPart.z >= 0.5) blockOrigin.z++;
        if (boxOriginFloatPart.x <= -0.5) blockOrigin.x--;
        if (boxOriginFloatPart.y <= -0.5) blockOrigin.y--;
        if (boxOriginFloatPart.z <= -0.5) blockOrigin.z--;

        Vector3 boxOpposite = new Vector3
        (
            pos.x + bc.size.x / 2,
            pos.y + bc.size.y,
            pos.z + bc.size.z / 2
        );

        Vector3 boxOppositeFloatPart = new Vector3
        (
            boxOpposite.x - (float)((int)boxOpposite.x),
            boxOpposite.y - (float)((int)boxOpposite.y),
            boxOpposite.z - (float)((int)boxOpposite.z)
        );

        Vector3Int blockOpposite = new Vector3Int
        (
            (int)boxOpposite.x, (int)boxOpposite.y, (int)boxOpposite.z
        );

        if (boxOppositeFloatPart.x >= 0.5) blockOpposite.x++;
        if (boxOppositeFloatPart.y >= 0.5) blockOpposite.y++;
        if (boxOppositeFloatPart.z >= 0.5) blockOpposite.z++;
        if (boxOppositeFloatPart.x <= -0.5) blockOpposite.x--;
        if (boxOppositeFloatPart.y <= -0.5) blockOpposite.y--;
        if (boxOppositeFloatPart.z <= -0.5) blockOpposite.z--;

        blockOrigin.x += Constants.WORLD_HALF_SIZE;
        blockOrigin.z += Constants.WORLD_HALF_SIZE;

        blockOpposite.x += Constants.WORLD_HALF_SIZE;
        blockOpposite.z += Constants.WORLD_HALF_SIZE;

        for (int x = blockOrigin.x; x <= blockOpposite.x; x++)
        {
            for (int y = blockOrigin.y; y <= blockOpposite.y; y++)
            {
                for (int z = blockOrigin.z; z <= blockOpposite.z; z++)
                {
                    if (target.x == x && target.y == y && target.z == z)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    private void Use(ref List<Vector4> targetBlocks)
    {
        if (isInventoryOpen) return;

        if (McControls.IsKeyDown(Constants.CONTROL_USE))
        {
            if 
            (
                targetBlocks[Constants.TARGET_BLOCK_SET].w == 0 && 
                !IsBlockInPlayer(targetBlocks[Constants.TARGET_BLOCK_SET]) &&
                hotBar.GetIsContain(hotBar.SelectingSlot) != 0
            ){
                HandAnim();

                lastSetFrame = Time.frameCount;
                isFrameSetBlock = true;
                
                // ブロックの設置
                blockAdmin.Set
                (
                    targetBlocks[Constants.TARGET_BLOCK_SET],
                    ref frameSetBlocks, hotBar, hotBar.SelectingSlot
                );
            }
        }
        else if (McControls.IsKey(Constants.CONTROL_USE))
        {
            if 
            (
                targetBlocks[Constants.TARGET_BLOCK_SET].w == 0 && Time.frameCount - lastSetFrame > 10 &&
                !IsBlockInPlayer(targetBlocks[Constants.TARGET_BLOCK_SET]) &&
                hotBar.GetIsContain(hotBar.SelectingSlot) != 0
            ){
                HandAnim();
                
                lastSetFrame = Time.frameCount;
                isFrameSetBlock = true;

                // ブロックの設置
                blockAdmin.Set
                (
                    targetBlocks[Constants.TARGET_BLOCK_SET],
                    ref frameSetBlocks, hotBar, hotBar.SelectingSlot
                );
            }
        }

    }

    public void PickUpItem(GameObject entityObj)
    {
        EntityItem entityItem = entityObj.GetComponent<EntityItem>();
        if (entityItem != null)
        {
            // インベントリにアイテムを追加
            inventory.AddContent(entityItem.ID);
            UpdateHotBarSlot();
            Destroy(entityObj);
        }
    }

    public void ResetFrameBlocks()
    {
        isFrameSetBlock = false;
        isFrameDestroyBlock = false;
    }

    private void UpdateHotBarSlot()
    {
        if (hotBar.SelectSlot(hotBar.SelectingSlot)) // 何らかのアイテムを持っている
        {
            canvasRightArmIdle.SetActive(false);
            holdingObjIdle.SetActive(true);
        }
        else
        {
            canvasRightArmIdle.SetActive(true);
            holdingObjIdle.SetActive(false);
        }
    }

    public void OnArmAnimEnd()
    {
        if (viewMode != 1)
        {
            rightArm.SetActive(true);
            partsSub.SetActive(false);
        }
        else
        {
            canvasRightArm.SetActive(false);
            canvasRightArmIdle.SetActive(true);
        }

        UpdateHotBarSlot();
    }

    public void OnHoldingAnimEnd()
    {
        holdingObj.SetActive(false);
        holdingObjIdle.SetActive(true);

        UpdateHotBarSlot();
    }

    public void Execute(List<Vector4> targetBlocks)
    {   
        // フレーム開始
        FrameStart();

        // プレイヤーの視点更新
        ViewUpdate(ref targetBlocks);

        // プレイヤーの着地中移動更新
        if (isGrounded) MoveUpdate();

        // プレイヤーのジャンプ
        Jump();

        // プレイヤーの飛行中移動更新
        FlyUpdate();

        Attack(ref targetBlocks);

        Use(ref targetBlocks);

        // プレイヤーの移動
        UpdateHitBox();

        // フレーム終了
        FrameFinish();
    }

    public void UpdateInfos()
    {
        // 位置情報の更新
        if (!isInventoryOpen)
        {
            movement.y = rb.velocity.y;
            rb.velocity = movement;
        }
        else
        {
            if (isFlying && !isLanded)
            {
                movement.y = rb.velocity.y;
                rb.velocity = movement;
            }
            else rb.velocity = Vector3.zero;
        }

        // EntityItemとの当たり判定トリガーの位置を更新
        triggerHitBoxObj.transform.position = transform.position;

        // インベントリの更新
        blockAdmin.FinishedSet(frameSetBlocks);
        blockAdmin.FinishedBreak(frameDestroyBlocks);
    }

    private void HotBarSelectSlot(int slot)
    {
        hotBar.SelectSlot(slot);
        UpdateHotBarSlot();
    }

    public void FrameStart()
    {
        // プレイヤーの現在座標を取得
        pos = transform.position;

        // プレイヤーの現在のスピードから高速で落下しているかどうかを判定
        if (rb.velocity.y < -fastFallSpeed) isFastFalling = true;

        if (!isInventoryOpen) // プレイヤーのホットバーの選択スロットを更新
        {
            if (McControls.IsKeyDown(Constants.CONTROL_HS1)) HotBarSelectSlot(1);
            else if (McControls.IsKeyDown(Constants.CONTROL_HS2)) HotBarSelectSlot(2);
            else if (McControls.IsKeyDown(Constants.CONTROL_HS3)) HotBarSelectSlot(3);
            else if (McControls.IsKeyDown(Constants.CONTROL_HS4)) HotBarSelectSlot(4);
            else if (McControls.IsKeyDown(Constants.CONTROL_HS5)) HotBarSelectSlot(5);
            else if (McControls.IsKeyDown(Constants.CONTROL_HS6)) HotBarSelectSlot(6);
            else if (McControls.IsKeyDown(Constants.CONTROL_HS7)) HotBarSelectSlot(7);
            else if (McControls.IsKeyDown(Constants.CONTROL_HS8)) HotBarSelectSlot(8);
            else if (McControls.IsKeyDown(Constants.CONTROL_HS9)) HotBarSelectSlot(9);

            float scroll = Input.GetAxis("Mouse ScrollWheel");
            if (scroll > 0)
            {
                if (hotBar.SelectingSlot == 1) HotBarSelectSlot(9);
                else HotBarSelectSlot(hotBar.SelectingSlot - 1);
            }
            else if (scroll < 0)
            {
                if (hotBar.SelectingSlot == 9) HotBarSelectSlot(1);
                else HotBarSelectSlot(hotBar.SelectingSlot + 1);
            }

            if (McControls.IsKeyDown(Constants.CONTROL_DROP_ITEM))
            {
                // ホットバーのアイテムをドロップ
                hotBar.DropItem(hotBar.SelectingSlot);
                UpdateHotBarSlot();
            }
        }
        else
        {
            UpdateHotBarSlot();
        }
    }

    private void FrameFinish()
    {

    }

    public void LoadFromJson()
    {

    }

    public void SaveToJson()
    {

    }

    public void DestroyJson()
    {

    }

    void OnCollisionEnter(Collision collision)
    {
        foreach (ContactPoint contact in collision.contacts)
        {
            if (rb.velocity.y <= 0.001f && rb.velocity.y >= -0.001f)
            {
                if (contact.point.y >= pos.y - collisionOffset && contact.point.y <= pos.y + collisionOffset && !isFlying)
                {
                    isGrounded = true;
                    if (isFastFalling)
                    {
                        mcSounds.PlayPlayer(Constants.SOUND_FALL_SMALL);
                        isFastFalling = false;
                    }
                }
                else if 
                (
                    contact.point.y >= pos.y - collisionOffset && contact.point.y <= pos.y + collisionOffset && isFlying)
                {
                    isLanded = true;
                    if (isFastFalling)
                    {
                        mcSounds.PlayPlayer(Constants.SOUND_FALL_SMALL);
                        isFastFalling = false;
                    }
                }
            }
        }
    }
}
