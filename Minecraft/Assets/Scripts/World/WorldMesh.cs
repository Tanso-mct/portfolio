using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WorldMesh : MonoBehaviour
{
    // Combineしたメッシュの頂点、UV、頂点インデックス
    private List<Vector3> vertices;
    private List<Vector2> uv;
    private List<int> triangles;

    public Texture2D textureAtlas;

    List<GameObject> squares;

    public void Init()
    {
        gameObject.AddComponent<MeshFilter>();
        gameObject.AddComponent<MeshRenderer>();

        CombineSquares();

        gameObject.SetActive(false);
    }

    public void CombineSquares()
    {
        // 新しいメッシュを作成
        Mesh combinedMesh = new Mesh();

        // Squareを設定
        squares = SupportFunc.GetChildren(gameObject);

        // Squareがない場合は処理を終了
        if (squares.Count == 0)　return;

        // Squareのテクスチャタイルを設定
        List<Vector2Int> textureTiles = new List<Vector2Int>();
        for (int i = 0; i < squares.Count; i++)
        {
            textureTiles.Add(squares[i].GetComponent<WorldSquare>().textureTile);
        }

        // 各SquareのUVを設定
        // テクスチャ一つなら左上を、面事にかわるなら左上から右へ、下へと続く
        for (int i = 0; i < squares.Count; i++)
        {
            Vector2Int tile = new Vector2Int
            (
                textureTiles[i].x % 5, 
                (textureAtlas.height / 16 - 1) - (int)(textureTiles[i].y / 5)
            );

            Vector2 originUV = new Vector2(tile.x * 16, tile.y * 16);
            originUV.x /= textureAtlas.width;
            originUV.y /= textureAtlas.height;

            Vector2 oppositeUV = new Vector2(tile.x * 16 + 16, tile.y * 16 + 16);
            oppositeUV.x /= textureAtlas.width;
            oppositeUV.y /= textureAtlas.height;

            Mesh mesh = squares[i].GetComponent<MeshFilter>().mesh;
            squares[i].GetComponent<MeshRenderer>().material.mainTexture = textureAtlas;

            Vector2[] uvs = new Vector2[4];

            uvs[0] = new Vector2(originUV.x, originUV.y);
            uvs[1] = new Vector2(oppositeUV.x, originUV.y); 
            uvs[2] = new Vector2(originUV.x, oppositeUV.y);
            uvs[3] = new Vector2(oppositeUV.x, oppositeUV.y);
            
            mesh.uv = uvs;
        }

        CombineInstance[] combine = new CombineInstance[squares.Count];
        for (int i = 0; i < squares.Count; i++)
        {
            combine[i].mesh = squares[i].GetComponent<MeshFilter>().sharedMesh;
            combine[i].transform = squares[i].transform.localToWorldMatrix;

        }

        combinedMesh.CombineMeshes(combine);

        // 結合したメッシュを適用
        GetComponent<MeshFilter>().mesh = combinedMesh;
        GetComponent<MeshRenderer>().material.mainTexture = textureAtlas;

        // 各Squareの情報を格納
        vertices = new List<Vector3>();
        uv = new List<Vector2>();
        triangles = new List<int>();
        vertices.AddRange(combinedMesh.vertices);
        uv.AddRange(combinedMesh.uv);
        for (int i = 0; i < squares.Count; i++)
        {
            triangles.AddRange(squares[i].GetComponent<MeshFilter>().mesh.triangles);
        }

        // 最大頂点数、頂点インデックスを設定
        Constants.SOURCE_MESH_VS_MAX = (vertices.Count > Constants.SOURCE_MESH_VS_MAX) ? vertices.Count : Constants.SOURCE_MESH_VS_MAX;
        Constants.SOURCE_MESH_TRIS_MAX = (triangles.Count > Constants.SOURCE_MESH_TRIS_MAX) ? triangles.Count : Constants.SOURCE_MESH_TRIS_MAX;
    }

    public void SetData(ref ComputeShader shader, ref List<Vector3> vertices, ref List<Vector2> uv, ref List<int> triangles)
    {
        float textureTileX = 16f / textureAtlas.width;
        float textureTileY = 16f / textureAtlas.height;

        shader.SetFloat("TEXTURE_BLOCK_TILE_U", textureTileX);
        shader.SetFloat("TEXTURE_BLOCK_TILE_V", textureTileY);

        shader.SetInt("SOURCE_MESH_BLOCK_FACE_COUNT", squares.Count);
        shader.SetInt("SOURCE_MESH_BLOCK_VS_INDEX", vertices.Count);
        shader.SetInt("SOURCE_MESH_BLOCK_TRIS_INDEX", triangles.Count);

        shader.SetInt("SOURCE_MESH_BLOCK_FRONT_FACES", 1);
        shader.SetInt("SOURCE_MESH_BLOCK_BACK_FACES", 1);
        shader.SetInt("SOURCE_MESH_BLOCK_LEFT_FACES", 1);
        shader.SetInt("SOURCE_MESH_BLOCK_RIGHT_FACES", 1);
        shader.SetInt("SOURCE_MESH_BLOCK_TOP_FACES", 1);
        shader.SetInt("SOURCE_MESH_BLOCK_BOTTOM_FACES", 1);
        
        vertices.AddRange(this.vertices);
        uv.AddRange(this.uv);
        triangles.AddRange(this.triangles);
    }

    public void SetGrassData(ref ComputeShader shader, ref List<Vector3> vertices, ref List<Vector2> uv, ref List<int> triangles)
    {
        shader.SetInt("SOURCE_MESH_GRASS_FACE_COUNT", squares.Count);
        shader.SetInt("SOURCE_MESH_GRASS_VS_INDEX", vertices.Count);
        shader.SetInt("SOURCE_MESH_GRASS_TRIS_INDEX", triangles.Count);

        shader.SetInt("SOURCE_MESH_GRASS_FRONT_FACES", 1);
        shader.SetInt("SOURCE_MESH_GRASS_BACK_FACES", 1);
        shader.SetInt("SOURCE_MESH_GRASS_LEFT_FACES", 1);
        shader.SetInt("SOURCE_MESH_GRASS_RIGHT_FACES", 1);
        shader.SetInt("SOURCE_MESH_GRASS_TOP_FACES", 1);
        shader.SetInt("SOURCE_MESH_GRASS_BOTTOM_FACES", 1);
        
        vertices.AddRange(this.vertices);
        uv.AddRange(this.uv);
        triangles.AddRange(this.triangles);
    }

    public void SetStairsData(ref ComputeShader shader, ref List<Vector3> vertices, ref List<Vector2> uv, ref List<int> triangles)
    {
        shader.SetInt("SOURCE_MESH_STAIRS_FACE_COUNT", squares.Count);
        shader.SetInt("SOURCE_MESH_STAIRS_VS_INDEX", vertices.Count);
        shader.SetInt("SOURCE_MESH_STAIRS_TRIS_INDEX", triangles.Count);

        shader.SetInt("SOURCE_MESH_STAIRS_FRONT_FACES", 2);
        shader.SetInt("SOURCE_MESH_STAIRS_BACK_FACES", 1);
        shader.SetInt("SOURCE_MESH_STAIRS_LEFT_FACES", 2);
        shader.SetInt("SOURCE_MESH_STAIRS_RIGHT_FACES", 2);
        shader.SetInt("SOURCE_MESH_STAIRS_TOP_FACES", 2);
        shader.SetInt("SOURCE_MESH_STAIRS_BOTTOM_FACES", 1);
        
        vertices.AddRange(this.vertices);
        uv.AddRange(this.uv);
        triangles.AddRange(this.triangles);
    }
}
