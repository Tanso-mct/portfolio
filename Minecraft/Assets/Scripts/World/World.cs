using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEngine.AI;

public class World : MonoBehaviour
{
    // ワールド情報のリスト
    // ブロックのバッファー。その座標に存在しているブロックの種類を示す。
    private ComputeBuffer blocksIDBuff;

    // 当たり判定を持たないブロックのバッファー。その座標に存在しているブロックの種類を示す。
    private ComputeBuffer throughBlocksIDBuff;

    // ワールドメッシュオブジェクト
    [SerializeField] private GameObject objWorldMesh;
    private Mesh worldMesh;
    private MeshCollider worldMeshCollider;

    // ワールドすり抜けメッシュオブジェクト
    [SerializeField] private GameObject objWorldThroughMesh;
    private Mesh worldThroughMesh;

    // プレイヤー
    [SerializeField] private Player player;

    // Vaxelの管理クラス
    [SerializeField] private BlockAdmin blockAdmin;

    // HitBoxの管理クラス
    [SerializeField] private McHitBoxAdmin hitboxAdmin;

    // Shader
    [SerializeField] private ComputeShader worldShader;

    // 各Vaxelのソースメッシュオブジェクト
    [SerializeField] private WorldMesh meshBlock;
    [SerializeField] private WorldMesh meshGrass;
    [SerializeField] private WorldMesh meshStairs;

    // [0] 描画するブロックの数 [1][2][3] ブロックのインデックス、頂点の開始位置、頂点インデックスの開始位置
    private ComputeBuffer countsBuff;
    private int drawBlockCount = 0;
    private int meshVsCount = 0;
    private int meshTrisCount = 0;

    // ワールドメッシュの頂点データバッファー。描画範囲によりサイズが変わる。
    private ComputeBuffer meshVsBuff;

    // ワールドメッシュのUVデータバッファー。描画範囲によりサイズが変わる。
    private ComputeBuffer meshUVsBuff;

    // ワールドメッシュの頂点インデックスデータバッファー。描画範囲によりサイズが変わる。
    private ComputeBuffer meshTrisBuff;

    // ソースブロックらのメッシュデータ
    List<Vector3> sourceMeshVs;
    List<Vector2> sourceMeshUVs;
    List<int> sourceMeshTris;
    private ComputeBuffer sourceMeshVsBuff;
    private ComputeBuffer sourceMeshUVsBuff;
    private ComputeBuffer sourceMeshTrisBuff;

    // Raycast用のバッファー
    private ComputeBuffer raycastBlocksBuff;
    Vector4[] raycastBlocks;

    // Raycastの精度
    [SerializeField] private int rayAccuracy = 100;

    private void SaveWorldInfo()
    {
        // JSONファイルに保存
    }

    public void Init()
    {
        // ワールドの初期化
        blocksIDBuff = new ComputeBuffer(Constants.WORLD_SIZE * Constants.WORLD_HEIGHT * Constants.WORLD_SIZE, sizeof(int));

        // ワールドすり抜けブロックらの初期化
        throughBlocksIDBuff = new ComputeBuffer(Constants.WORLD_SIZE * Constants.WORLD_HEIGHT * Constants.WORLD_SIZE, sizeof(int));

        // Vaxelの管理クラスの初期化
        blockAdmin.Init();

        // ソースメッシュオブジェクトの初期化
        sourceMeshVs = new List<Vector3>();
        sourceMeshUVs = new List<Vector2>();
        sourceMeshTris = new List<int>();
        
        meshBlock.Init();
        meshGrass.Init();
        meshStairs.Init();

        // ソースメッシュオブジェクトのデータを作成
        meshBlock.SetData(ref worldShader, ref sourceMeshVs, ref sourceMeshUVs, ref sourceMeshTris);
        meshGrass.SetGrassData(ref worldShader, ref sourceMeshVs, ref sourceMeshUVs, ref sourceMeshTris);
        meshStairs.SetStairsData(ref worldShader, ref sourceMeshVs, ref sourceMeshUVs, ref sourceMeshTris);

        // ソースメッシュオブジェクトのバッファー作成
        sourceMeshVsBuff = new ComputeBuffer(sourceMeshVs.Count, sizeof(float) * 3);
        sourceMeshUVsBuff = new ComputeBuffer(sourceMeshUVs.Count, sizeof(float) * 2);
        sourceMeshTrisBuff = new ComputeBuffer(sourceMeshTris.Count, sizeof(int));

        sourceMeshVsBuff.SetData(sourceMeshVs);
        sourceMeshUVsBuff.SetData(sourceMeshUVs);
        sourceMeshTrisBuff.SetData(sourceMeshTris);

        // ワールドメッシュの初期化
        worldMesh = new Mesh();
        objWorldMesh.GetComponent<MeshFilter>().mesh = worldMesh;
        worldMesh.MarkDynamic();

        Texture meshAtlasTexture = null;
        SupportFunc.LoadTexture(ref meshAtlasTexture, Constants.TEXTURE_ATLAS_BLOCK);
        objWorldMesh.GetComponent<MeshRenderer>().material.mainTexture = meshAtlasTexture;

        worldMeshCollider = objWorldMesh.GetComponent<MeshCollider>();

        // ワールドすり抜けメッシュの初期化
        worldThroughMesh = new Mesh();
        objWorldThroughMesh.GetComponent<MeshFilter>().mesh = worldThroughMesh;
        worldThroughMesh.MarkDynamic();

        Texture throughMeshAtlasTexture = null;
        SupportFunc.LoadTexture(ref throughMeshAtlasTexture, Constants.TEXTURE_ATLAS_BLOCK);
        objWorldThroughMesh.GetComponent<MeshRenderer>().material.mainTexture = throughMeshAtlasTexture;

        // [0] 描画するブロックの数 [1][2] 頂点の開始位置、頂点インデックスの開始位置を示すバッファー
        countsBuff = new ComputeBuffer(3, sizeof(int));

        int[] countsAry = new int[3];
        for (int i = 0; i < 3; i++) countsAry[i] = 0;
        countsBuff.SetData(countsAry);
        
        // ワールドメッシュの頂点、UV、頂点インデックスのバッファー作成
        meshVsBuff = new ComputeBuffer
        (
            McVideos.RenderDistance * Constants.CHUCK_SIZE * 
            McVideos.RenderDistance * Constants.CHUCK_SIZE *
            McVideos.RenderDistance * Constants.CHUCK_SIZE * 
            Constants.SOURCE_MESH_VS_MAX, 
            sizeof(float) * 3
        );

        meshUVsBuff = new ComputeBuffer
        (
            McVideos.RenderDistance * Constants.CHUCK_SIZE * 
            McVideos.RenderDistance * Constants.CHUCK_SIZE  *
            McVideos.RenderDistance * Constants.CHUCK_SIZE * 
            Constants.SOURCE_MESH_VS_MAX, 
            sizeof(float) * 2
        );

        meshTrisBuff = new ComputeBuffer
        (
            McVideos.RenderDistance * Constants.CHUCK_SIZE * 
            McVideos.RenderDistance * Constants.CHUCK_SIZE  *
            McVideos.RenderDistance * Constants.CHUCK_SIZE * 
            Constants.SOURCE_MESH_TRIS_MAX, 
            sizeof(int)
        );

        // Raycastの初期化
        RaycastInit();

        // ワールドの生成もしくは読み込み
        Create();
    }

    private void SetMeshGenerateBuff(int kernelIndex)
    {
        // シェーダーの定数をセット
        Constants.SetShaderConstants(ref worldShader);

        // 各バッファーをシェーダーにセット
        worldShader.SetBuffer(kernelIndex, "blocksID", blocksIDBuff);

        worldShader.SetInt("RENDER_DISTANCE", McVideos.RenderDistance);

        worldShader.SetBuffer(kernelIndex, "counts", countsBuff);

        worldShader.SetBuffer(kernelIndex, "meshVs", meshVsBuff);
        worldShader.SetBuffer(kernelIndex, "meshUVs", meshUVsBuff);
        worldShader.SetBuffer(kernelIndex, "meshTris", meshTrisBuff);

        worldShader.SetBuffer(kernelIndex, "sourceMeshVs", sourceMeshVsBuff);
        worldShader.SetBuffer(kernelIndex, "sourceMeshUVs", sourceMeshUVsBuff);
        worldShader.SetBuffer(kernelIndex, "sourceMeshTris", sourceMeshTrisBuff);
    }

    private void SetThroughMeshGenerateBuff(int kernelIndex)
    {
        // シェーダーの定数をセット
        Constants.SetShaderConstants(ref worldShader);

        // 各バッファーをシェーダーにセット
        worldShader.SetBuffer(kernelIndex, "blocksID", throughBlocksIDBuff);

        worldShader.SetInt("RENDER_DISTANCE", McVideos.RenderDistance);

        worldShader.SetBuffer(kernelIndex, "counts", countsBuff);

        worldShader.SetBuffer(kernelIndex, "meshVs", meshVsBuff);
        worldShader.SetBuffer(kernelIndex, "meshUVs", meshUVsBuff);
        worldShader.SetBuffer(kernelIndex, "meshTris", meshTrisBuff);

        worldShader.SetBuffer(kernelIndex, "sourceMeshVs", sourceMeshVsBuff);
        worldShader.SetBuffer(kernelIndex, "sourceMeshUVs", sourceMeshUVsBuff);
        worldShader.SetBuffer(kernelIndex, "sourceMeshTris", sourceMeshTrisBuff);
    }

    // DEBUG用
    private void CreateBlock(ref Vector3Int pos, int blockId, ref int[] blocksId)
    {
        int index = pos.x + pos.y * Constants.WORLD_SIZE + pos.z * Constants.WORLD_SIZE * Constants.WORLD_HEIGHT;
        blocksId[index] = blockId;
        pos.x++;
    }

    // Paramに保存されているワールド情報を使用してワールドの生成
    public void Create()
    {
        // プレイヤーの初期化
        player.Init();

        // ワールド全体を処理するためのスレッドグループ数
        int worldThreadGroupsX = Mathf.CeilToInt(Constants.WORLD_SIZE / 8.0f);
        int worldThreadGroupsY = Mathf.CeilToInt(Constants.WORLD_HEIGHT / 8.0f);
        int worldThreadGroupsZ = Mathf.CeilToInt(Constants.WORLD_SIZE / 8.0f);

        // プレイヤーの座標を変換
        Vector3Int convertedPos = SupportFunc.CoordsIntConvert(player.Pos);

        // 描画範囲を処理するためのスレッドグループ数
        Vector3Int worldOrigin = new Vector3Int
        (
            convertedPos.x - McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.y - McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.z - McVideos.RenderDistance * Constants.CHUCK_SIZE
        );

        Vector3Int worldOpposite = new Vector3Int
        (
            convertedPos.x + McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.y + McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.z + McVideos.RenderDistance * Constants.CHUCK_SIZE
        );

        worldOrigin.x = Mathf.Clamp(worldOrigin.x, 0, Constants.WORLD_SIZE - 1);
        worldOrigin.y = Mathf.Clamp(worldOrigin.y, 0, Constants.WORLD_HEIGHT - 1);
        worldOrigin.z = Mathf.Clamp(worldOrigin.z, 0, Constants.WORLD_SIZE - 1);

        worldOpposite.x = Mathf.Clamp(worldOpposite.x, 0, Constants.WORLD_SIZE - 1);
        worldOpposite.y = Mathf.Clamp(worldOpposite.y, 0, Constants.WORLD_HEIGHT - 1);
        worldOpposite.z = Mathf.Clamp(worldOpposite.z, 0, Constants.WORLD_SIZE - 1);

        // 処理するインデックスの開始地点、終了地点をセット
        worldShader.SetInt("VIEW_ORIGIN_X", worldOrigin.x);
        worldShader.SetInt("VIEW_ORIGIN_Y", worldOrigin.y);
        worldShader.SetInt("VIEW_ORIGIN_Z", worldOrigin.z);

        int[] blocksId = new int[Constants.WORLD_SIZE * Constants.WORLD_HEIGHT * Constants.WORLD_SIZE];
        int[] throughBlocksId = new int[Constants.WORLD_SIZE * Constants.WORLD_HEIGHT * Constants.WORLD_SIZE];

        // フラットワールドの生成
        for (int x = 0; x < Constants.WORLD_SIZE; x++)
        {
            for (int y = 0; y < Constants.WORLD_HEIGHT; y++)
            {
                for (int z = 0; z < Constants.WORLD_SIZE; z++)
                {
                    int index = x + y * Constants.WORLD_SIZE + z * Constants.WORLD_SIZE * Constants.WORLD_HEIGHT;
                    if (y == 0) blocksId[index] = (int)Constants.VAXEL_TYPE.BEDROCK; 
                    else if (y >= 1 && y <= 2) blocksId[index] = (int)Constants.VAXEL_TYPE.DIRT;
                    else if (y == 3) blocksId[index] = (int)Constants.VAXEL_TYPE.GRASS_TOP;
                    else blocksId[index] = (int)Constants.VAXEL_TYPE.AIR;
                }
            }
        }

        
        blocksIDBuff.SetData(blocksId);
        throughBlocksIDBuff.SetData(throughBlocksId);

        int viewThreadGroupsX = Mathf.CeilToInt((worldOpposite.x - worldOrigin.x) / 8.0f);
        int viewThreadGroupsY = Mathf.CeilToInt((worldOpposite.y - worldOrigin.y) / 8.0f);
        int viewThreadGroupsZ = Mathf.CeilToInt((worldOpposite.z - worldOrigin.z) / 8.0f);

        // 空気と隣接するブロックを描画ブロックとし、それらの情報を取得
        int meshGenerate = worldShader.FindKernel("MeshGenerate");
        SetMeshGenerateBuff(meshGenerate);
        worldShader.Dispatch(meshGenerate, viewThreadGroupsX, viewThreadGroupsY, viewThreadGroupsZ);

        // 各数を取得
        int[] countsAry = new int[3];
        countsBuff.GetData(countsAry);

        drawBlockCount = countsAry[0];
        meshVsCount = countsAry[1];
        meshTrisCount = countsAry[2];

        // カウント配列を初期化
        for (int i = 0; i < 3; i++) countsAry[i] = 0;
        countsBuff.SetData(countsAry);

        // ワールドメッシュの頂点、UV、頂点インデックスを取得
        Vector3[] meshVsAry = new Vector3[meshVsCount];
        Vector2[] meshUVsAry = new Vector2[meshVsCount];
        int[] meshTrisAry = new int[meshTrisCount];

        meshVsBuff.GetData(meshVsAry);
        meshUVsBuff.GetData(meshUVsAry);
        meshTrisBuff.GetData(meshTrisAry);

        // ワールドメッシュの頂点、UV、頂点インデックスを設定
        worldMesh.Clear();
        worldMesh.SetVertices(meshVsAry);
        worldMesh.SetUVs(0, meshUVsAry);
        worldMesh.SetTriangles(meshTrisAry, 0);

        // ワールドメッシュの更新
        if (worldMesh.vertices.Length > 0)
        {
            worldMesh.RecalculateTangents();
            worldMesh.RecalculateNormals();
            worldMesh.RecalculateBounds();
            worldMesh.Optimize();

            // ワールドメッシュのコライダーを更新
            worldMeshCollider.sharedMesh = null;
            worldMeshCollider.sharedMesh = worldMesh;
        }


        // 空気もしくは流体と隣接するブロックを描画ブロックとし、それらの情報を取得
        int throughMeshGenerate = worldShader.FindKernel("ThroughMeshGenerate");
        SetThroughMeshGenerateBuff(throughMeshGenerate);
        worldShader.Dispatch(throughMeshGenerate, viewThreadGroupsX, viewThreadGroupsY, viewThreadGroupsZ);

        // 各数を取得
        countsBuff.GetData(countsAry);

        drawBlockCount = countsAry[0];
        meshVsCount = countsAry[1];
        meshTrisCount = countsAry[2];

        // カウント配列を初期化
        for (int i = 0; i < 3; i++) countsAry[i] = 0;
        countsBuff.SetData(countsAry);

        // ワールドメッシュの頂点、UV、頂点インデックスを取得
        Vector3[] throughMeshVsAry = new Vector3[meshVsCount];
        Vector2[] throughMeshUVsAry = new Vector2[meshVsCount];
        int[] throughMeshTrisAry = new int[meshTrisCount];

        meshVsBuff.GetData(throughMeshVsAry);
        meshUVsBuff.GetData(throughMeshUVsAry);
        meshTrisBuff.GetData(throughMeshTrisAry);

        // ワールドメッシュの頂点、UV、頂点インデックスを設定
        worldThroughMesh.Clear();
        worldThroughMesh.SetVertices(meshVsAry);
        worldThroughMesh.SetUVs(0, meshUVsAry);
        worldThroughMesh.SetTriangles(meshTrisAry, 0);

        // ワールドメッシュの更新
        if (worldThroughMesh.vertices.Length > 0)
        {
            worldThroughMesh.RecalculateTangents();
            worldThroughMesh.RecalculateNormals();
            worldThroughMesh.RecalculateBounds();
            worldThroughMesh.Optimize();
        }

        // プレイヤーの生成及び配置
        player.Create(player.Pos);
    }

    // Paramに保存されているワールド情報から指定のJsonファイルを使用して読み込む
    public void LoadFromJson()
    {
        // メッシュの読み込み

        // プレイヤーの読み込み及び配置
        player.Init();
        player.LoadFromJson();
    }

    public void SaveToJson()
    {
        // 現在のワールド情報、データをJsonファイルに保存
    }

    public void DestroyJson()
    {
        // Paramに保存されているワールド情報を使用してワールドを削除
    }

    private void RaycastInit()
    {
        int raySize = (int)(player.Reach * rayAccuracy);

        // レイキャスト用のバッファーを作成
        raycastBlocks = new Vector4[raySize];
        for (int i = 0; i < raySize; i++)
        {
            raycastBlocks[i].x = 0;
            raycastBlocks[i].y = 0;
            raycastBlocks[i].z = 0;
            raycastBlocks[i].w = 0;
        }

        raycastBlocksBuff = new ComputeBuffer(raySize, sizeof(float) * 4);
        raycastBlocksBuff.SetData(raycastBlocks);
    }

    // X = Block.x, Y = Block.y, Z = Block.z W = BlockType
    private List<Vector4> RaycastAtBlock()
    {
        Vector3 origin = player.cam.transform.position;
        origin.x += Constants.WORLD_HALF_SIZE;
        origin.z += Constants.WORLD_HALF_SIZE;

        Vector3 direction = player.cam.transform.forward;

        int raySize = (int)(player.Reach * rayAccuracy);

        worldShader.SetFloat("RAY_SIZE", raySize);

        worldShader.SetFloat("RAY_ORIGIN_X", origin.x);
        worldShader.SetFloat("RAY_ORIGIN_Y", origin.y);
        worldShader.SetFloat("RAY_ORIGIN_Z", origin.z);

        worldShader.SetFloat("RAY_DIRECTION_X", direction.x);
        worldShader.SetFloat("RAY_DIRECTION_Y", direction.y);
        worldShader.SetFloat("RAY_DIRECTION_Z", direction.z);

        worldShader.SetFloat("RAY_LENGTH", player.Reach);

        int raycastAtBlock = worldShader.FindKernel("RaycastAtBlock");

        // シェーダーの定数をセット
        Constants.SetShaderConstants(ref worldShader);

        worldShader.SetBuffer(raycastAtBlock, "blocksID", blocksIDBuff);
        worldShader.SetBuffer(raycastAtBlock, "raycastBlocks", raycastBlocksBuff);

        int threadGroupsX = Mathf.CeilToInt(raySize / 8.0f);
        int threadGroupsY = Mathf.CeilToInt(1);
        int threadGroupsZ = Mathf.CeilToInt(1);

        worldShader.Dispatch(raycastAtBlock, threadGroupsX, threadGroupsY, threadGroupsZ);

        raycastBlocksBuff.GetData(raycastBlocks);

        Vector4 selectBlock = new Vector4(0, 0, 0, Constants.BLOCK_TYPE_CANT_SET);
        Vector4 setBlock = new Vector4(0, 0, 0, Constants.BLOCK_TYPE_CANT_SET);
        for (int i = 0; i < raycastBlocks.Length; i++)
        {
            if (raycastBlocks[i].w != (int)Constants.VAXEL_TYPE.AIR)
            {
                selectBlock.x = raycastBlocks[i].x;
                selectBlock.y = raycastBlocks[i].y;
                selectBlock.z = raycastBlocks[i].z;
                selectBlock.w = raycastBlocks[i].w;

                if (i - 1 >= 0)
                {
                    setBlock.x = raycastBlocks[i - 1].x;
                    setBlock.y = raycastBlocks[i - 1].y;
                    setBlock.z = raycastBlocks[i - 1].z;
                    setBlock.w = raycastBlocks[i - 1].w;
                }
                else
                {
                    setBlock.w = Constants.BLOCK_TYPE_CANT_SET;
                }
                break;
            }
        }

        return new List<Vector4> { selectBlock, setBlock };
    }

    private void MeshUpdate()
    {
        // プレイヤーの座標を変換
        Vector3Int convertedPos = SupportFunc.CoordsIntConvert(player.Pos);

        // 描画範囲を処理するためのスレッドグループ数
        Vector3Int worldOrigin = new Vector3Int
        (
            convertedPos.x - McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.y - McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.z - McVideos.RenderDistance * Constants.CHUCK_SIZE
        );

        Vector3Int worldOpposite = new Vector3Int
        (
            convertedPos.x + McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.y + McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.z + McVideos.RenderDistance * Constants.CHUCK_SIZE
        );

        worldOrigin.x = Mathf.Clamp(worldOrigin.x, 0, Constants.WORLD_SIZE - 1);
        worldOrigin.y = Mathf.Clamp(worldOrigin.y, 0, Constants.WORLD_HEIGHT - 1);
        worldOrigin.z = Mathf.Clamp(worldOrigin.z, 0, Constants.WORLD_SIZE - 1);

        worldOpposite.x = Mathf.Clamp(worldOpposite.x, 0, Constants.WORLD_SIZE - 1);
        worldOpposite.y = Mathf.Clamp(worldOpposite.y, 0, Constants.WORLD_HEIGHT - 1);
        worldOpposite.z = Mathf.Clamp(worldOpposite.z, 0, Constants.WORLD_SIZE - 1);

        int viewThreadGroupsX = Mathf.CeilToInt((worldOpposite.x - worldOrigin.x) / 8.0f);
        int viewThreadGroupsY = Mathf.CeilToInt((worldOpposite.y - worldOrigin.y) / 8.0f);
        int viewThreadGroupsZ = Mathf.CeilToInt((worldOpposite.z - worldOrigin.z) / 8.0f);

        if (viewThreadGroupsX == 0) return;

        // 処理するインデックスの開始地点、終了地点をセット
        worldShader.SetInt("VIEW_ORIGIN_X", worldOrigin.x);
        worldShader.SetInt("VIEW_ORIGIN_Y", worldOrigin.y);
        worldShader.SetInt("VIEW_ORIGIN_Z", worldOrigin.z);

        // 空気と隣接するブロックを描画ブロックとし、それらの情報を取得
        int meshGenerate = worldShader.FindKernel("MeshGenerate");
        SetMeshGenerateBuff(meshGenerate);
        worldShader.Dispatch(meshGenerate, viewThreadGroupsX, viewThreadGroupsY, viewThreadGroupsZ);

        // 各数を取得
        int[] countsAry = new int[3];
        countsBuff.GetData(countsAry);

        drawBlockCount = countsAry[0];
        meshVsCount = countsAry[1];
        meshTrisCount = countsAry[2];

        // カウント配列を初期化
        for (int i = 0; i < 3; i++) countsAry[i] = 0;
        countsBuff.SetData(countsAry);

        // ワールドメッシュの頂点、UV、頂点インデックスを取得
        Vector3[] meshVsAry = new Vector3[meshVsCount];
        Vector2[] meshUVsAry = new Vector2[meshVsCount];
        int[] meshTrisAry = new int[meshTrisCount];

        meshVsBuff.GetData(meshVsAry);
        meshUVsBuff.GetData(meshUVsAry);
        meshTrisBuff.GetData(meshTrisAry);

        // ワールドメッシュの頂点、UV、頂点インデックスを設定
        worldMesh.Clear();
        worldMesh.SetVertices(meshVsAry);
        worldMesh.SetUVs(0, meshUVsAry);
        worldMesh.SetTriangles(meshTrisAry, 0);

        if (worldMesh.vertices.Length > 0)
        {
            // ワールドメッシュの更新
            worldMesh.RecalculateTangents();
            worldMesh.RecalculateNormals();
            worldMesh.RecalculateBounds();
            worldMesh.Optimize();

            // ワールドメッシュのコライダーを更新
            worldMeshCollider.sharedMesh = null;
            worldMeshCollider.sharedMesh = worldMesh;
        }

    }

    private void ThroughMeshUpdate()
    {
        // プレイヤーの座標を変換
        Vector3Int convertedPos = SupportFunc.CoordsIntConvert(player.Pos);

        // 描画範囲を処理するためのスレッドグループ数
        Vector3Int worldOrigin = new Vector3Int
        (
            convertedPos.x - McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.y - McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.z - McVideos.RenderDistance * Constants.CHUCK_SIZE
        );

        Vector3Int worldOpposite = new Vector3Int
        (
            convertedPos.x + McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.y + McVideos.RenderDistance * Constants.CHUCK_SIZE,
            convertedPos.z + McVideos.RenderDistance * Constants.CHUCK_SIZE
        );

        worldOrigin.x = Mathf.Clamp(worldOrigin.x, 0, Constants.WORLD_SIZE - 1);
        worldOrigin.y = Mathf.Clamp(worldOrigin.y, 0, Constants.WORLD_HEIGHT - 1);
        worldOrigin.z = Mathf.Clamp(worldOrigin.z, 0, Constants.WORLD_SIZE - 1);

        worldOpposite.x = Mathf.Clamp(worldOpposite.x, 0, Constants.WORLD_SIZE - 1);
        worldOpposite.y = Mathf.Clamp(worldOpposite.y, 0, Constants.WORLD_HEIGHT - 1);
        worldOpposite.z = Mathf.Clamp(worldOpposite.z, 0, Constants.WORLD_SIZE - 1);

        int viewThreadGroupsX = Mathf.CeilToInt((worldOpposite.x - worldOrigin.x) / 8.0f);
        int viewThreadGroupsY = Mathf.CeilToInt((worldOpposite.y - worldOrigin.y) / 8.0f);
        int viewThreadGroupsZ = Mathf.CeilToInt((worldOpposite.z - worldOrigin.z) / 8.0f);

        if (viewThreadGroupsX == 0) return;

        // 処理するインデックスの開始地点、終了地点をセット
        worldShader.SetInt("VIEW_ORIGIN_X", worldOrigin.x);
        worldShader.SetInt("VIEW_ORIGIN_Y", worldOrigin.y);
        worldShader.SetInt("VIEW_ORIGIN_Z", worldOrigin.z);

        // 空気と隣接するブロックを描画ブロックとし、それらの情報を取得
        int throughMeshGenerate = worldShader.FindKernel("ThroughMeshGenerate");
        SetThroughMeshGenerateBuff(throughMeshGenerate);
        worldShader.Dispatch(throughMeshGenerate, viewThreadGroupsX, viewThreadGroupsY, viewThreadGroupsZ);

        // 各数を取得
        int[] countsAry = new int[3];
        countsBuff.GetData(countsAry);

        drawBlockCount = countsAry[0];
        meshVsCount = countsAry[1];
        meshTrisCount = countsAry[2];

        // カウント配列を初期化
        for (int i = 0; i < 3; i++) countsAry[i] = 0;
        countsBuff.SetData(countsAry);

        // ワールドメッシュの頂点、UV、頂点インデックスを取得
        Vector3[] meshVsAry = new Vector3[meshVsCount];
        Vector2[] meshUVsAry = new Vector2[meshVsCount];
        int[] meshTrisAry = new int[meshTrisCount];

        meshVsBuff.GetData(meshVsAry);
        meshUVsBuff.GetData(meshUVsAry);
        meshTrisBuff.GetData(meshTrisAry);

        // ワールドメッシュの頂点、UV、頂点インデックスを設定
        worldThroughMesh.Clear();
        worldThroughMesh.SetVertices(meshVsAry);
        worldThroughMesh.SetUVs(0, meshUVsAry);
        worldThroughMesh.SetTriangles(meshTrisAry, 0);

        if (worldMesh.vertices.Length > 0)
        {
            // ワールドメッシュの更新
            worldThroughMesh.RecalculateTangents();
            worldThroughMesh.RecalculateNormals();
            worldThroughMesh.RecalculateBounds();
            worldThroughMesh.Optimize();
        }

    }

    private void BlockUpdate()
    {
        // ブロックの生成
        int blockUpdate = worldShader.FindKernel("BlockUpdate");
        worldShader.SetBuffer(blockUpdate, "blocksID", blocksIDBuff);
        worldShader.SetBuffer(blockUpdate, "throughBlocksID", throughBlocksIDBuff);

        int[] targetBlockID = new int[1];
        ComputeBuffer targetBlockIDBuff = new ComputeBuffer(1, sizeof(int));
        worldShader.SetBuffer(blockUpdate, "targetBlockID", targetBlockIDBuff);

        // シェーダーの定数をセット
        Constants.SetShaderConstants(ref worldShader);

        if (player.isFrameSetBlock)
        {
            // ブロックの生成
            worldShader.SetInt("TARGET_BLOCK_X", (int)player.frameSetBlocks.x);
            worldShader.SetInt("TARGET_BLOCK_Y", (int)player.frameSetBlocks.y);
            worldShader.SetInt("TARGET_BLOCK_Z", (int)player.frameSetBlocks.z);
            worldShader.SetInt("GENERATE_BLOCK_ID", (int)player.frameSetBlocks.w);
            worldShader.Dispatch(blockUpdate, 1, 1, 1);

            targetBlockIDBuff.GetData(targetBlockID);
            player.frameSetBlocks.w = targetBlockID[0];
        }
        else
        {
            player.frameSetBlocks.w = Constants.FRAME_BLOCK_IS_NULL;
        }

        if (player.isFrameDestroyBlock)
        {
            // ブロックの削除
            worldShader.SetInt("TARGET_BLOCK_X", (int)player.frameDestroyBlocks.x);
            worldShader.SetInt("TARGET_BLOCK_Y", (int)player.frameDestroyBlocks.y);
            worldShader.SetInt("TARGET_BLOCK_Z", (int)player.frameDestroyBlocks.z);
            worldShader.SetInt("GENERATE_BLOCK_ID", (int)Constants.VAXEL_TYPE.AIR);
            worldShader.Dispatch(blockUpdate, 1, 1, 1);

            targetBlockIDBuff.GetData(targetBlockID);
            player.frameDestroyBlocks.w = targetBlockID[0];
        }
        else
        {
            player.frameDestroyBlocks.w = Constants.FRAME_BLOCK_IS_NULL;
        }

        player.ResetFrameBlocks();


        // バッファの解放
        targetBlockIDBuff.Release();
    }

    private void EntityUpdate()
    {

    }

    private void ItemUpdate()
    {

    }

    private void HitBoxUpdate()
    {
        // プレイヤーのヒットボックスの更新
        int hitBox = worldShader.FindKernel("HitBox");
        worldShader.SetBuffer(hitBox, "blocksID", throughBlocksIDBuff);

        // シェーダーの定数をセット
        Constants.SetShaderConstants(ref worldShader);

        // 配列の生成
        hitboxAdmin.CreateAry();

        // 各バッファを作成
        ComputeBuffer boxPosBuff = new ComputeBuffer(hitboxAdmin.GetHitBoxAmount(), sizeof(float) * 3);
        ComputeBuffer boxSizeBuff = new ComputeBuffer(hitboxAdmin.GetHitBoxAmount(), sizeof(float) * 3);
        ComputeBuffer hitBlockTypeBuff = new ComputeBuffer(hitboxAdmin.GetHitBoxAmount(), sizeof(int));

        // データをセット
        boxPosBuff.SetData(hitboxAdmin.boxPosAry);
        boxSizeBuff.SetData(hitboxAdmin.boxSizeAry);

        // バッファーのセット
        worldShader.SetBuffer(hitBox, "boxPos", boxPosBuff);
        worldShader.SetBuffer(hitBox, "boxSize", boxSizeBuff);
        worldShader.SetBuffer(hitBox, "hitBlockType", hitBlockTypeBuff);

        int threadGroupsX = Mathf.CeilToInt(hitboxAdmin.GetHitBoxAmount() / 8.0f);
        int threadGroupsY = Mathf.CeilToInt(1 / 8.0f);
        int threadGroupsZ = Mathf.CeilToInt(1 / 8.0f);

        // ヒットボックスの更新
        worldShader.Dispatch(hitBox, 1, 1, 1);

        // データの取得
        hitBlockTypeBuff.GetData(hitboxAdmin.hitBlockTypeAry);

        if (Input.GetKeyDown(KeyCode.H))
        {
            Debug.Log("====================================");

            for (int i = 0; i < hitboxAdmin.GetHitBoxAmount(); i++)
            {
                Debug.Log("HitBlockType : " + hitboxAdmin.hitBlockTypeAry[i]);
            }
        }

        // バッファーの解放
        boxPosBuff.Release();
        boxSizeBuff.Release();
        hitBlockTypeBuff.Release();
    }

    public void Execute()
    {
        // プレイヤーの実行。Raycastの結果を送信
        player.Execute(RaycastAtBlock());

        // ブロックの更新
        BlockUpdate();

        // メッシュの更新
        MeshUpdate();
        ThroughMeshUpdate();

        // エンティティの更新
        EntityUpdate();

        // アイテムの更新
        ItemUpdate();

        // 当たり判定の更新
        HitBoxUpdate();

        // プレイヤーの位置更新
        player.UpdateInfos();
        
    }

    void OnDestroy()
    {
        // バッファを解放
        if (blocksIDBuff != null) blocksIDBuff.Release();
        if (throughBlocksIDBuff != null) throughBlocksIDBuff.Release();

        if (countsBuff != null) countsBuff.Release();

        if (meshVsBuff != null) meshVsBuff.Release();
        if (meshUVsBuff != null) meshUVsBuff.Release();
        if (meshTrisBuff != null) meshTrisBuff.Release();
        
        if (sourceMeshVsBuff != null) sourceMeshVsBuff.Release();
        if (sourceMeshUVsBuff != null) sourceMeshUVsBuff.Release();
        if (sourceMeshTrisBuff != null) sourceMeshTrisBuff.Release();

        if (raycastBlocksBuff != null) raycastBlocksBuff.Release();
    }
}
