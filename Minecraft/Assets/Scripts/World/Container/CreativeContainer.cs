using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CreativeContainer : Container
{
    [SerializeField] private int containerId = 0;

    [SerializeField] private Container hotBar;
    [SerializeField] private Container inventory;

    [SerializeField] private Container otherContainer;

    public override void Init()
    {
        for (int i = 0; i < slots.Count; i++)
        {
            slots[i].Init(i+1);
        }

        if (containerId == 0)
        {
            slots[9].SetContents((int)Constants.VAXEL_TYPE.DIRT, 1);
            slots[10].SetContents((int)Constants.VAXEL_TYPE.GRASS_TOP, 1);
            slots[11].SetContents((int)Constants.VAXEL_TYPE.OBSIDIAN, 1);
            slots[12].SetContents((int)Constants.VAXEL_TYPE.WATER, 1);
            slots[13].SetContents((int)Constants.VAXEL_TYPE.LAVA, 1);
            slots[14].SetContents((int)Constants.VAXEL_TYPE.STONE, 1);
            slots[15].SetContents((int)Constants.VAXEL_TYPE.COBBLESTONE, 1);
            slots[16].SetContents((int)Constants.VAXEL_TYPE.STONE_ANDESITE, 1);
            slots[17].SetContents((int)Constants.VAXEL_TYPE.STONE_DIORITE, 1);
            slots[18].SetContents((int)Constants.VAXEL_TYPE.STONE_GRANITE, 1);
            slots[19].SetContents((int)Constants.VAXEL_TYPE.COAL_ORE, 1);
            slots[20].SetContents((int)Constants.VAXEL_TYPE.IRON_ORE, 1);
            slots[21].SetContents((int)Constants.VAXEL_TYPE.GOLD_ORE, 1);
            slots[22].SetContents((int)Constants.VAXEL_TYPE.DIAMOND_ORE, 1);
            slots[23].SetContents((int)Constants.VAXEL_TYPE.EMERALD_ORE, 1);
            slots[24].SetContents((int)Constants.VAXEL_TYPE.LAPIS_ORE, 1);
            slots[25].SetContents((int)Constants.VAXEL_TYPE.LEAVES, 1);
            slots[26].SetContents((int)Constants.VAXEL_TYPE.LOG_OAK_TOP, 1);
            slots[27].SetContents((int)Constants.VAXEL_TYPE.PLANKS_OAK, 1);
            slots[28].SetContents((int)Constants.VAXEL_TYPE.PLANKS_BIRCH, 1);
            slots[29].SetContents((int)Constants.VAXEL_TYPE.LOG_BIRCH_TOP, 1);
            slots[30].SetContents((int)Constants.VAXEL_TYPE.GRAVEL, 1);
        }
        else if (containerId == 1)
        {
            slots[9].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY, 1);
            slots[10].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_BLACK, 1);
            slots[11].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_BLUE, 1);
            slots[12].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_BROWN, 1);
            slots[13].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_CYAN, 1);
            slots[14].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_GRAY, 1);
            slots[15].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_GREEN, 1);
            slots[16].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_LIGHT_BLUE, 1);
            slots[17].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_LIME, 1);
            slots[18].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_MAGENTA, 1);
            slots[19].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_ORANGE, 1);
            slots[20].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_PINK, 1);
            slots[21].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_PURPLE, 1);
            slots[22].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_RED, 1);
            slots[23].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_WHITE, 1);
            slots[24].SetContents((int)Constants.VAXEL_TYPE.HARDENED_CLAY_STAINED_YELLOW, 1);

            slots[25].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_BLUE, 1);
            slots[26].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_BROWN, 1);
            slots[27].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_CYAN, 1);
            slots[28].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_GRAY, 1);
            slots[29].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_GREEN, 1);
            slots[30].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_LIGHT_BLUE, 1);
            slots[31].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_LIME, 1);
            slots[32].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_MAGENTA, 1);
            slots[33].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_ORANGE, 1);
            slots[34].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_PINK, 1);
            slots[35].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_PURPLE, 1);
            slots[36].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_RED, 1);
            slots[37].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_SILVER, 1);
            slots[38].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_WHITE, 1);
            slots[39].SetContents((int)Constants.VAXEL_TYPE.WOOL_COLORED_YELLOW, 1);
        }

    }

    protected override void SetSlotContent(int slotId, int vaxelId, int amount)
    {
        if (slotId-1 < hotBar.slots.Count)
        {
            slots[slotId-1].SetContents(vaxelId, amount);
            hotBar.slots[slotId-1].SetContents(vaxelId, amount);
            inventory.slots[slotId-1].SetContents(vaxelId, amount);
            otherContainer.slots[slotId-1].SetContents(vaxelId, amount);
        }
    }

    public override void DropItem(int slotId)
    {
        
    }

    public override void SlotQuickMove()
    {
        int startSlotVaxelId = 0;
        int startSlotAmount = 0;
        bool startSlotIsStackable = slots[startSlotId-1].GetIsContain(ref startSlotVaxelId, ref startSlotAmount);

        if (startSlotId >= 1 && startSlotId <= 9)
        {
            SetSlotContent(startSlotId, 0, 0);
        }
        else
        {
            int thisSlotVaxelId = 0;
            int thisSlotAmount = 0;
            bool thisSlotIsStackable = false;
            for (int i = 0; i < 9; i++)
            {
                if (!slots[i].isQuickMoveable) continue;

                thisSlotIsStackable = slots[i].GetIsContain(ref thisSlotVaxelId, ref thisSlotAmount);
                if (thisSlotVaxelId == 0)
                {
                    SetSlotContent(i+1, startSlotVaxelId, stackMax);
                    return;
                }
                else if (thisSlotIsStackable && thisSlotVaxelId == startSlotVaxelId && thisSlotAmount != stackMax)
                {
                    SetSlotContent(i+1, startSlotVaxelId, stackMax);
                    return;
                }
            }
        }
    }

    public override bool AddContent(int vaxelId)
    {
        int nowVaxelId = 0;
        int nowAmount = 0;
        for (int i = 0; i < slots.Count; i++)
        {
            bool isStackable = slots[i].GetIsContain(ref nowVaxelId, ref nowAmount);
            if (nowVaxelId == 0) // 空いているスロット
            {
                slots[i].SetContents(vaxelId, 1);
                if (i < hotBar.slots.Count) hotBar.slots[i].SetContents(vaxelId, 1);
                return true;
            }
            else if (isStackable && nowVaxelId == vaxelId && nowAmount < stackMax) 
            {
                slots[i].SetContents(vaxelId, nowAmount + 1);
                if (i < hotBar.slots.Count) hotBar.slots[i].SetContents(vaxelId, nowAmount + 1);
                return true;
            }
        }

        // 空いているスロットがないため、何もしない
        return false;
    }

    public override Vector2 RemoveContent(int amount, int slot) // return x: vaxelId, y: amount
    {
        int nowVaxelId = 0;
        int nowAmount = 0;
        bool isStackable = slots[slot-1].GetIsContain(ref nowVaxelId, ref nowAmount);

        if (isStackable && nowVaxelId != 0 && nowAmount > amount)
        {
            slots[slot-1].SetContents(nowVaxelId, nowAmount - amount);
            if (slot <= hotBar.slots.Count) hotBar.slots[slot-1].SetContents(nowVaxelId, nowAmount - amount);
            return new Vector2(nowVaxelId, amount);
        }
        else if (isStackable && nowVaxelId != 0 && nowAmount <= amount)
        {
            slots[slot-1].SetContents(0, 0);
            if (slot <= hotBar.slots.Count) hotBar.slots[slot-1].SetContents(0, 0);
            return new Vector2(nowVaxelId, nowAmount);
        }
        else if (!isStackable && nowVaxelId != 0)
        {
            slots[slot-1].SetContents(0, 0);
            if (slot <= hotBar.slots.Count) hotBar.slots[slot-1].SetContents(0, 0);
            return new Vector2(nowVaxelId, 1);
        }
        else
        {
            return new Vector2(0, 0);
        }
    }
}
