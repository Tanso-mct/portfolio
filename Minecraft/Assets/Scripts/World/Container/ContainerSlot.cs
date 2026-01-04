using System.Collections;
using System.Collections.Generic;
using UnityEngine;

using UnityEngine.UI;

public class ContainerSlot : MonoBehaviour
{
    [SerializeField] private Container container;
    [SerializeField] private TextEl textEl;

    [SerializeField] private Image selector;
    [SerializeField] private Image itemImage;

    [SerializeField] private float blockScale = 0.5f;

    private int id;
    private int amount = 0;

    private int slotId;

    [SerializeField] private bool isSelectable = true;
    [SerializeField] public bool isQuickMoveable = true;

    private bool isClicked = false;

    public void Init(int slotId)
    {
        Sprite nullItemSprite = null;
        SupportFunc.LoadSprite(ref nullItemSprite, Constants.SPRITE_NULL);

        itemImage.sprite = nullItemSprite;

        textEl.initText.text = "";

        id = 0;
        amount = 0;

        this.slotId = slotId;
    }

    public void OnHover()
    {
        selector.gameObject.SetActive(true);

        container.DropItem(slotId);

        if (isSelectable) container.SetNowHoverSlot(slotId);
        else container.SetNowHoverSlot(0);
    }

    public void UnHover()
    {
        selector.gameObject.SetActive(false);

        container.SetNowHoverSlot(0);
    }

    public void OnClick()
    {
        container.StartHoverSlotMove(slotId);
    }

    public bool GetIsContain(ref int vaxelId, ref int amount)
    {
        vaxelId = id;
        amount = this.amount;

        return SupportFunc.IsStackable(vaxelId);
    }

    public int GetIsContain()
    {
        return id;
    }

    public void SetContents(int vaxelId, int amount)
    {
        id = vaxelId;
        Sprite blockSprite = SupportFunc.LoadSpriteFromId(id);;

        itemImage.sprite = blockSprite;
        itemImage.transform.localScale = new Vector3(blockScale, blockScale, blockScale);

        if (amount >= 2)
        {
            textEl.initText.text = amount.ToString();
        }
        else
        {
            textEl.initText.text = "";
        }

        this.amount = amount;
    }
}
