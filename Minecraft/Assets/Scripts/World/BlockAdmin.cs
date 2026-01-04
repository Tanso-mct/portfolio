using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockAdmin : MonoBehaviour
{
    // ワールド上の何らかのデータを持つブロックを管理
    private Dictionary<Vector3Int, Vaxel> dataContainBlocks;

    private Vaxel setVaxel;
    private Vaxel useVaxel;
    private Vaxel breakVaxel;

    [SerializeField] private GameObject entityItemParent = null;

    [SerializeField] private McSounds mcSounds = null;


    public void Init()
    {
        setVaxel = null;
        useVaxel = null;
        breakVaxel = null;
    }

    private Vaxel GetVaxelInstance(int blockId)
    {
        Vaxel rtVaxel = null;

        switch (blockId)
        {
            case (int)Constants.VAXEL_TYPE.DIRT:
            case (int)Constants.VAXEL_TYPE.GRASS_TOP:
            case (int)Constants.VAXEL_TYPE.OBSIDIAN:
            case (int)Constants.VAXEL_TYPE.WATER:
            case (int)Constants.VAXEL_TYPE.LAVA:
            case (int)Constants.VAXEL_TYPE.STONE:
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
            case (int)Constants.VAXEL_TYPE.LEAVES:
            case (int)Constants.VAXEL_TYPE.LOG_OAK_TOP:
            case (int)Constants.VAXEL_TYPE.LOG_OAK:
            case (int)Constants.VAXEL_TYPE.LOG_OAK_BOTTOM:
            case (int)Constants.VAXEL_TYPE.PLANKS_OAK:
            case (int)Constants.VAXEL_TYPE.PLANKS_BIRCH:
            case (int)Constants.VAXEL_TYPE.LOG_BIRCH_TOP:
            case (int)Constants.VAXEL_TYPE.LOG_BIRCH:
            case (int)Constants.VAXEL_TYPE.LOG_BIRCH_BOTTOM:
            case (int)Constants.VAXEL_TYPE.GRAVEL:
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
                rtVaxel = new Vaxel();
                rtVaxel.Init(entityItemParent, mcSounds);
                break;
        }

        return rtVaxel;
    }

    private Vaxel GetVaxel(Vector4 block)
    {
        int blockId = (int)block.w;
        return GetVaxelInstance(blockId);
    }

    private Vaxel GetVaxel(Container container, int slot)
    {
        int blockId = container.GetIsContain(slot);
        return GetVaxelInstance(blockId);
    }

    public void Set(Vector4 block, ref Vector4 frameSetBlock, Container sourceContainer, int setSlot)
    {
        setVaxel = GetVaxel(sourceContainer, setSlot);
        setVaxel.TrySet(block, ref frameSetBlock, sourceContainer, setSlot);
    }

    public void Break(Vector4 block, ref Vector4 frameDestroyBlock)
    {
        breakVaxel = GetVaxel(block);
        if (breakVaxel == null) return;
        breakVaxel.TryBreak(block, ref frameDestroyBlock);
    }

    public bool FinishedSet(Vector4 frameSetBlock)
    {
        if (setVaxel == null) return false;

        setVaxel.FinishedSet(frameSetBlock);

        setVaxel = null;

        return true;
    }

    public bool FinishedBreak(Vector4 frameDestroyBlock)
    {
        if (breakVaxel == null) return false;

        breakVaxel.FinishedBreak(frameDestroyBlock);

        breakVaxel = null;
        return true;
    }



}
