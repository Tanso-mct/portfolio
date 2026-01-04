using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class SupportFunc
{
    static public Vector3Int CoordsIntConvert(Vector3 coords)
    {
        Vector3Int rtCoords = new Vector3Int();

        rtCoords.x = (int)coords.x + Constants.WORLD_HALF_SIZE;
        rtCoords.y = (int)coords.y;
        rtCoords.z = (int)coords.z + Constants.WORLD_HALF_SIZE;

        return rtCoords;
    }

    static public Vector3 CoordsFloatConvert(Vector3 coords)
    {
        Vector3 rtCoords = new Vector3();       

        rtCoords.x = coords.x + Constants.WORLD_HALF_SIZE;
        rtCoords.y = coords.y;
        rtCoords.z = coords.z + Constants.WORLD_HALF_SIZE;

        return rtCoords;
    }

    static public Vector3 PosFloatConvert(Vector3 coords)
    {
        Vector3 rtCoords = new Vector3();

        rtCoords.x = coords.x - Constants.WORLD_HALF_SIZE;
        rtCoords.y = coords.y;
        rtCoords.z = coords.z - Constants.WORLD_HALF_SIZE;

        return rtCoords;
    }

    static public List<GameObject> GetChildren(GameObject parent)
    {
        List<GameObject> children = new List<GameObject>();

        foreach (Transform child in parent.transform)
        {
            children.Add(child.gameObject);
        }

        return children;
    }

    static public void InstantiatePrefab
    (
        ref GameObject target, ref GameObject prefab, string prefabPath, ref GameObject parent, Vector3 coords
    ){
        prefab = Resources.Load<GameObject>(prefabPath);

        if (prefab != null)
        {
            target = GameObject.Instantiate(prefab, coords, Quaternion.identity);

            if (target != null)
            {
                target.transform.SetParent(parent.transform);
            }
            else
            {
                Debug.LogError("Failed to instantiate prefab: " + prefabPath);
            }
        }
        else
        {
            Debug.LogError("Failed to load prefab: " + prefabPath);
        }
    }

    static public void InstantiateCopy(ref GameObject target, ref GameObject source, ref GameObject parent, Vector3 coords)
    {
        target = GameObject.Instantiate(source, coords, Quaternion.identity);

        if (target != null)
        {
            target.transform.SetParent(parent.transform);
        }
        else
        {
            Debug.LogError("Failed to instantiate copy: " + source.name);
        }
    }

    static public void LoadTexture(ref Texture texture, string texturePath)
    {
        texture = Resources.Load<Texture>(texturePath);
        if (texture == null) Debug.LogError("Failed to load texture: " + texturePath);
    }

    static public void SetMeshRendererTexture(ref MeshRenderer meshRend, ref Texture texture)
    {
        if (meshRend != null) meshRend.material.mainTexture = texture;
        else Debug.LogError("Failed to set texture: " + texture.name);
    }

    static public void LoadSprite(ref Sprite sprite, string spritePath)
    {
        sprite = Resources.Load<Sprite>(spritePath);
        if (sprite == null)
        {
            Debug.LogError("Failed to load sprite: " + spritePath);
        }
    }

    static public string GetSpritePathFromId(int vaxelId)
    {
        string rtStr = "";
        if (vaxelId < Constants.VAXEL_SPRITE_PATH.Count)
        {
            rtStr = Constants.VAXEL_SPRITE_PATH[vaxelId];
        }

        return rtStr;
    }

    public static string GetSoundTypeFromId(int vaxelId)
    {
        string rtStr = "";
        switch (vaxelId)
        {
            case (int)Constants.VAXEL_TYPE.AIR:
            case (int)Constants.VAXEL_TYPE.WATER:
            case (int)Constants.VAXEL_TYPE.LAVA:
                rtStr = "";
                break;

            case (int)Constants.VAXEL_TYPE.DIRT:
            case (int)Constants.VAXEL_TYPE.LEAVES:
            case (int)Constants.VAXEL_TYPE.GRAVEL:
                rtStr = Constants.SOUND_DIG_GRAVEL;
                break;

            case (int)Constants.VAXEL_TYPE.GRASS_TOP:
            case (int)Constants.VAXEL_TYPE.GRASS_SIDE:
            case (int)Constants.VAXEL_TYPE.GRASS_BOTTOM:
                rtStr = Constants.SOUND_DIG_GRASS;
                break;

            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_BLUE:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_BROWN:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_CYAN:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_GRAY:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_GREEN:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_LIGHT_BLUE:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_LIME:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_MAGENTA:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_ORANGE:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_PINK:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_PURPLE:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_RED:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_SILVER:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_WHITE:
            case (int)Constants.VAXEL_TYPE.WOOL_COLORED_YELLOW:
                rtStr = Constants.SOUND_DIG_CLOTH;
                break;

            case (int)Constants.VAXEL_TYPE.STONE:
            case (int)Constants.VAXEL_TYPE.OBSIDIAN:
            case (int)Constants.VAXEL_TYPE.COBBLESTONE:
            case (int)Constants.VAXEL_TYPE.STONE_ANDESITE:
            case (int)Constants.VAXEL_TYPE.STONE_DIORITE:
            case (int)Constants.VAXEL_TYPE.STONE_GRANITE:
            case (int)Constants.VAXEL_TYPE.COAL_ORE:
            case (int)Constants.VAXEL_TYPE.IRON_ORE:
            case (int)Constants.VAXEL_TYPE.GOLD_ORE:
            case (int)Constants.VAXEL_TYPE.DIAMOND_ORE:
            case (int)Constants.VAXEL_TYPE.EMERALD_ORE:
            case (int)Constants.VAXEL_TYPE.LAPIS_ORE:

            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_BLACK:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_BLUE:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_BROWN:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_CYAN:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_GRAY:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_GREEN:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_LIGHT_BLUE:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_LIME:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_MAGENTA:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_ORANGE:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_PINK:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_PURPLE:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_RED:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_SILVER:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_WHITE:
            case (int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_YELLOW:
                rtStr = Constants.SOUND_DIG_STONE;
                break;
            
            case (int)Constants.VAXEL_TYPE.LOG_OAK_TOP:
            case (int)Constants.VAXEL_TYPE.LOG_OAK_BOTTOM:
            case (int)Constants.VAXEL_TYPE.LOG_OAK:
            case (int)Constants.VAXEL_TYPE.PLANKS_OAK:
            case (int)Constants.VAXEL_TYPE.PLANKS_BIRCH:
            case (int)Constants.VAXEL_TYPE.LOG_BIRCH_TOP:
            case (int)Constants.VAXEL_TYPE.LOG_BIRCH_BOTTOM:
            case (int)Constants.VAXEL_TYPE.LOG_BIRCH:
                rtStr = Constants.SOUND_DIG_WOOD;
                break;
        }

        return rtStr;
    }

    static public bool IsStackable(int vaxelId)
    {
        return true;
    }

    static public bool IsItem(int vaxelId)
    {
        // switch (vaxelId)
        // {
        //     case (int)Constants.VAXEL_TYPE.WOOD_PICKAXE:
        //     case (int)Constants.VAXEL_TYPE.STONE_PICKAXE:
        //     case (int)Constants.VAXEL_TYPE.IRON_PICKAXE:
        //     case (int)Constants.VAXEL_TYPE.GOLD_PICKAXE:
        //     case (int)Constants.VAXEL_TYPE.DIAMOND_PICKAXE:
        //     case (int)Constants.VAXEL_TYPE.WOOD_SHOVEL:
        //     case (int)Constants.VAXEL_TYPE.STONE_SHOVEL:
        //     case (int)Constants.VAXEL_TYPE.IRON_SHOVEL:
        //     case (int)Constants.VAXEL_TYPE.GOLD_SHOVEL:
        //     case (int)Constants.VAXEL_TYPE.DIAMOND_SHOVEL:
        //     case (int)Constants.VAXEL_TYPE.WOOD_AXE:
        //     case (int)Constants.VAXEL_TYPE.STONE_AXE:
        //     case (int)Constants.VAXEL_TYPE.IRON_AXE:
        //     case (int)Constants.VAXEL_TYPE.GOLD_AXE:
        //     case (int)Constants.VAXEL_TYPE.DIAMOND_AXE:
        //     case (int)Constants.VAXEL_TYPE.WOOD_SWORD:
        //     case (int)Constants.VAXEL_TYPE.STONE_SWORD:
        //     case (int)Constants.VAXEL_TYPE.IRON_SWORD:
        //     case (int)Constants.VAXEL_TYPE.GOLD_SWORD:
        //     case (int)Constants.VAXEL_TYPE.DIAMOND_SWORD:
        //         return true;
        // }

        return false;
    }

    static public Sprite LoadSpriteFromId(int vaxelId)
    {
        Sprite rtSprite = null;

        string spritePath = GetSpritePathFromId(vaxelId);
        LoadSprite(ref rtSprite, spritePath);

        return rtSprite;
    }

    static public Texture LoadTextureFromId(int vaxelId)
    {
        Texture rtTexture = null;

        string texturePath = GetSpritePathFromId(vaxelId);
        LoadTexture(ref rtTexture, texturePath);

        return rtTexture;
    }

    static public List<Texture> LoadMultiTextureFromId(int vaxelId)
    {
        List<Texture> rtTextures = new List<Texture>();

        switch (vaxelId)
        {
            case (int)Constants.VAXEL_TYPE.GRASS_TOP:
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.GRASS_SIDE));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.GRASS_SIDE));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.GRASS_SIDE));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.GRASS_SIDE));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.GRASS_TOP));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.GRASS_BOTTOM));
                return rtTextures;

            case (int)Constants.VAXEL_TYPE.LOG_OAK_TOP:
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_OAK));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_OAK));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_OAK));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_OAK));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_OAK_TOP));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_OAK_BOTTOM));
                return rtTextures;

            case (int)Constants.VAXEL_TYPE.LOG_BIRCH_TOP:
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_BIRCH));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_BIRCH));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_BIRCH));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_BIRCH));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_BIRCH_TOP));
                rtTextures.Add(LoadTextureFromId((int)Constants.VAXEL_TYPE.LOG_BIRCH_BOTTOM));
                return rtTextures;
        }

        for (int i = 0; i < 6; i++)
        {
            rtTextures.Add(LoadTextureFromId(vaxelId));
        }

        return rtTextures;
    }

}
