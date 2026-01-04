using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class HotBar : Container
{
    private int selectingSlot = 1;
    public int SelectingSlot{ get { return selectingSlot; } }

    [SerializeField] private GameObject selectFrame;
    [SerializeField] private float selectFrameMoveVal = 98.0f;

    [SerializeField] private Container inventory;
    [SerializeField] private CreativeContainer creativeContainer;
    [SerializeField] private CreativeContainer creativeContainer2;

    [SerializeField] private GameObject holdingBlock;

    [SerializeField] private GameObject holdingBlockIdle;

    [SerializeField] private Texture holdingBlockTexture;

    public override void Init()
    {
        for (int i = 0; i < slots.Count; i++)
        {
            slots[i].Init(i+1);
        }

        holdingBlock.SetActive(false);
        holdingBlockIdle.SetActive(false);
    }

    private bool CreateHolding(int vaxelId)
    {
        holdingBlock.SetActive(false);
        holdingBlockIdle.SetActive(false);

        if (vaxelId == 0) return false;

        holdingBlock.SetActive(true);
        holdingBlockIdle.SetActive(true);

        List<Texture> texture = SupportFunc.LoadMultiTextureFromId(vaxelId);

        List<GameObject> holdingChildren = SupportFunc.GetChildren(holdingBlock);
        for (int i = 0; i < holdingChildren.Count; i++)
        {
            holdingChildren[i].GetComponent<MeshRenderer>().material.mainTexture = texture[i];
        }

        List<GameObject> holdingIdleChildren = SupportFunc.GetChildren(holdingBlockIdle);
        for (int i = 0; i < holdingIdleChildren.Count; i++)
        {
            holdingIdleChildren[i].GetComponent<MeshRenderer>().material.mainTexture = texture[i];
        }

        return true;

    }

    public bool SelectSlot(int slot)
    {
        if (!(slot >= 1 && slot <= 9)) return false;

        selectFrame.transform.position = new Vector3
        (
            selectFrame.transform.position.x + (slot - selectingSlot) * selectFrameMoveVal,
            selectFrame.transform.position.y,
            selectFrame.transform.position.z
        );

        selectingSlot = slot;

        CreateHolding(GetIsContain(slot));

        return GetIsContain(slot) != 0;
    }

    public override void DropItem(int slotId)
    {
        if (slotId >= 1 && slotId <= 9)
        {
            if (McControls.IsKeyDown(Constants.CONTROL_DROP_ITEM) && !Input.GetKey(KeyCode.LeftShift))
            {
                RemoveContent(1, slotId);
            }
            else if (McControls.IsKeyDown(Constants.CONTROL_DROP_ITEM) && Input.GetKey(KeyCode.LeftShift))
            {
                RemoveContent(64, slotId);
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
                inventory.slots[i].SetContents(vaxelId, 1);
                creativeContainer.slots[i].SetContents(vaxelId, 1);
                creativeContainer2.slots[i].SetContents(vaxelId, 1);
                return true;
            }
            else if (isStackable && nowVaxelId == vaxelId && nowAmount < stackMax) 
            {
                slots[i].SetContents(vaxelId, nowAmount + 1);
                inventory.slots[i].SetContents(vaxelId, nowAmount + 1);
                creativeContainer.slots[i].SetContents(vaxelId, nowAmount + 1);
                creativeContainer2.slots[i].SetContents(vaxelId, nowAmount + 1);
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
            inventory.slots[slot-1].SetContents(nowVaxelId, nowAmount - amount);
            creativeContainer.slots[slot-1].SetContents(nowVaxelId, nowAmount - amount);
            creativeContainer2.slots[slot-1].SetContents(nowVaxelId, nowAmount - amount);
            return new Vector2(nowVaxelId, amount);
        }
        else if (isStackable && nowVaxelId != 0 && nowAmount <= amount)
        {
            slots[slot-1].SetContents(0, 0);
            inventory.slots[slot-1].SetContents(0, 0);
            creativeContainer.slots[slot-1].SetContents(0, 0);
            creativeContainer2.slots[slot-1].SetContents(0, 0);
            return new Vector2(nowVaxelId, nowAmount);
        }
        else if (!isStackable && nowVaxelId != 0)
        {
            slots[slot-1].SetContents(0, 0);
            inventory.slots[slot-1].SetContents(0, 0);
            creativeContainer.slots[slot-1].SetContents(0, 0);
            creativeContainer2.slots[slot-1].SetContents(0, 0);
            return new Vector2(nowVaxelId, 1);
        }
        else
        {
            return new Vector2(0, 0);
        }
    }
}
