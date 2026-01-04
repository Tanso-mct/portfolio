using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using UnityEngine.UI;

public class Container : MonoBehaviour
{
    [SerializeField] public List<ContainerSlot> slots;

    public int width = 9;
    [SerializeField] public int height = 4;

    protected int stackMax = 64;

    [SerializeField] protected ContainerSlot hoverSlot;

    protected bool isStartHoverSlotMoving = false;
    protected bool isHoverSlotMoving = false;

    protected Vector2 startHoverSlotPos;
    protected Vector2 startMousePos;

    protected int startSlotId;
    protected int nowHoverSlotId;

    [SerializeField] private GameObject entityItemParent;
    private float entityItemOffY = 0.7f;

    [SerializeField] private GameObject playerObj;

    private int passedFrame = 0;
    private int moveFinishedFrame = 0;

    [SerializeField] private bool isInfiniteBlock = false;

    public void SetNowHoverSlot(int slotId)
    {
        nowHoverSlotId = slotId;
    }

    public virtual void SlotQuickMove()
    {

    }

    public virtual void DropItem(int slotId)
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

    public void StartHoverSlotMove(int slotId)
    {
        if (moveFinishedFrame + 2 < passedFrame)
        {
            if (!isStartHoverSlotMoving && !Input.GetKey(KeyCode.LeftShift))
            {
                isStartHoverSlotMoving = true;
                startSlotId = slotId;
            }
            else if (!isStartHoverSlotMoving && Input.GetKey(KeyCode.LeftShift))
            {
                startSlotId = slotId;
                SlotQuickMove();
            }
        }
    }

    protected virtual void SetSlotContent(int slotId, int vaxelId, int amount)
    {
        slots[slotId-1].SetContents(vaxelId, amount);
    }

    public void Update()
    {
        passedFrame++;

        if (Input.GetMouseButtonDown(0) && isHoverSlotMoving)
        {
            int vaxelId = 0;
            int amount = 0;
            hoverSlot.GetIsContain(ref vaxelId, ref amount);

            if (nowHoverSlotId == -1) // Delete
            {
                SetSlotContent(startSlotId, 0, 0);
            }
            else if (nowHoverSlotId != 0)
            {
                int hoverVaxelId = 0;
                int hoverAmount = 0;
                bool isStackable = slots[nowHoverSlotId-1].GetIsContain(ref hoverVaxelId, ref hoverAmount);

                if (hoverVaxelId == 0)
                {
                    SetSlotContent(nowHoverSlotId, vaxelId, amount);
                }
                else if (hoverVaxelId == vaxelId)
                {
                    if (isStackable && hoverAmount + amount <= stackMax)
                    {
                        SetSlotContent(nowHoverSlotId, vaxelId, hoverAmount + amount);
                    }
                    else if (isStackable && hoverAmount + amount > stackMax)
                    {
                        SetSlotContent(nowHoverSlotId, vaxelId, stackMax);
                        if ((!isInfiniteBlock) || (isInfiniteBlock && startSlotId >= 1 && startSlotId <= 9)) 
                        {
                            SetSlotContent(startSlotId, vaxelId, hoverAmount + amount - stackMax);
                        }
                    }
                }
                else if (hoverVaxelId != vaxelId)
                {
                    SetSlotContent(nowHoverSlotId, vaxelId, amount);
                    if ((!isInfiniteBlock) || (isInfiniteBlock && startSlotId >= 1 && startSlotId <= 9)) 
                    {
                        SetSlotContent(startSlotId, hoverVaxelId, hoverAmount);
                    }
                }
                else
                {
                    if ((!isInfiniteBlock) || (isInfiniteBlock && startSlotId >= 1 && startSlotId <= 9)) 
                    {
                        SetSlotContent(startSlotId, vaxelId, amount);
                    }
                }
            }
            else
            {
                if ((!isInfiniteBlock) || (isInfiniteBlock && startSlotId >= 1 && startSlotId <= 9)) 
                {
                    SetSlotContent(startSlotId, vaxelId, amount);
                }
            }

            hoverSlot.gameObject.SetActive(false);

            moveFinishedFrame = passedFrame;

            isStartHoverSlotMoving = false;
            isHoverSlotMoving = false;
        }
        else if (isHoverSlotMoving)
        {
            Vector2 inputPos = Input.mousePosition;
            Vector2 diff = inputPos - startMousePos;
            Vector2 newPos = startHoverSlotPos + diff;

            hoverSlot.transform.position = newPos;
        }

        if (isStartHoverSlotMoving && !isHoverSlotMoving)
        {
            isHoverSlotMoving = true;

            hoverSlot.gameObject.SetActive(true);
            hoverSlot.transform.position = slots[startSlotId-1].transform.position;

            int vaxelId = 0;
            int amount = 0;
            slots[startSlotId-1].GetIsContain(ref vaxelId, ref amount);

            hoverSlot.SetContents(vaxelId, amount);

            startHoverSlotPos = slots[startSlotId-1].transform.position;
            startMousePos = Input.mousePosition;

            if ((!isInfiniteBlock) || (isInfiniteBlock && startSlotId >= 1 && startSlotId <= 9)) 
            {
                SetSlotContent(startSlotId, 0, 0);
            }
        }
    }

    public virtual void Init()
    {
        for (int i = 0; i < slots.Count; i++)
        {
            slots[i].Init(i+1);
        }
    }

    public int GetIsContain(int slotId)
    {
        return slots[slotId-1].GetIsContain();
    }

    public virtual bool AddContent(int vaxelId)
    {
        int nowVaxelId = 0;
        int nowAmount = 0;
        for (int i = 0; i < slots.Count; i++)
        {
            bool isStackable = slots[i].GetIsContain(ref nowVaxelId, ref nowAmount);
            if (nowVaxelId == 0)
            {
                slots[i].SetContents(vaxelId, 1);
                return true;
            }
            else if (isStackable && nowVaxelId == vaxelId && nowAmount < stackMax) 
            {
                slots[i].SetContents(vaxelId, nowAmount + 1);
                return true;
            }
        }

        return false;
    }

    public virtual Vector2 RemoveContent(int amount, int slot) // return x: vaxelId, y: amount
    {
        int nowVaxelId = 0;
        int nowAmount = 0;
        bool isStackable = slots[slot-1].GetIsContain(ref nowVaxelId, ref nowAmount);

        if (isStackable && nowVaxelId != 0 && nowAmount > amount)
        {
            slots[slot-1].SetContents(nowVaxelId, nowAmount - amount);
            return new Vector2(nowVaxelId, amount);
        }
        else if (isStackable && nowVaxelId != 0 && nowAmount <= amount)
        {
            slots[slot-1].SetContents(0, 0);
            return new Vector2(nowVaxelId, nowAmount);
        }
        else if (!isStackable && nowVaxelId != 0)
        {
            slots[slot-1].SetContents(0, 0);
            return new Vector2(nowVaxelId, 1);
        }
        else
        {
            return new Vector2(0, 0);
        }
    }
}
