using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

public class SelectBarParts : MonoBehaviour
{
    [SerializeField] private float val;
    public float Val{ get { return val; } }

    [SerializeField] private float maxVal;
    [SerializeField] private float minVal;

    private bool isClicked = false;
    private bool isMoving = false;
    private Vector2 initSelectorPos;
    private Vector2 initMousePos;

    [HideInInspector] public Vector2 selectorPos;
    public Vector2 SelectorPos { get { return selectorPos; } }

    public UnityEvent mouseEvent = null;
    [SerializeField] private SelectBarEl selectBarEl = null;

    [SerializeField] private GameObject hoverText = null;

    public void Init(float val)
    {
        // SelectorÇÃèâä˙à íuÇê›íË
        this.val = val;
        float aspect = (val - minVal) / (maxVal - minVal);
        float posX = selectBarEl.GetBarMin() + selectBarEl.GetFrameWidth() * aspect;
        Vector2 pos = new Vector2(posX, selectBarEl.selector.transform.position.y);
        selectBarEl.selector.transform.position = pos;
    }

    public void EditTxt(string txt)
    {
        selectBarEl.initText.text = txt;
        hoverText.GetComponent<Text>().text = txt;
    }

    public void HoverEvent()
    {
        selectBarEl.ShowText(false, selectBarEl.initText.name);
        selectBarEl.ShowText(true, hoverText.name);
    }

    void Update()
    {
        if (isClicked)
        {
            if (!isMoving)
            {
                isMoving = true;
                initMousePos = Input.mousePosition;
                initSelectorPos = selectBarEl.selector.transform.position;
            }
            else
            {
                Vector2 pos = Input.mousePosition;

                Vector2 diff = pos - initMousePos;
                Vector2 newPos = initSelectorPos + new Vector2(diff.x, 0);
                if (newPos.x >= selectBarEl.GetBarMin() && newPos.x <= selectBarEl.GetBarMax())
                {
                    float aspect = (newPos.x - selectBarEl.GetBarMin()) / selectBarEl.GetFrameWidth();
                    val = minVal + (maxVal - minVal) * aspect;

                    selectBarEl.Transfer(ref newPos, ref selectBarEl.selector);

                    mouseEvent.Invoke();
                }
                else if (newPos.x < selectBarEl.GetBarMin())
                {
                    val = minVal;
                    Vector2 minPos = new Vector2(selectBarEl.GetBarMin(), newPos.y);
                    selectBarEl.Transfer(ref minPos, ref selectBarEl.selector);

                    mouseEvent.Invoke();
                }
                else if (newPos.x > selectBarEl.GetBarMax())
                {
                    val = maxVal;
                    Vector2 maxPos = new Vector2(selectBarEl.GetBarMax(), newPos.y);
                    selectBarEl.Transfer(ref maxPos, ref selectBarEl.selector);

                    mouseEvent.Invoke();
                }
            }

        }

        if (Input.GetMouseButtonUp(0) && isClicked)
        {
            selectorPos = selectBarEl.selector.transform.position;
            isClicked = false;
            isMoving = false;
        }
    }

    public void UnHoverEvent()
    {
        selectBarEl.ShowText(false, hoverText.name);
        selectBarEl.ShowText(true, selectBarEl.initText.name);
    }

    public void ClickEvent()
    {
        isClicked = true;
    }
}
