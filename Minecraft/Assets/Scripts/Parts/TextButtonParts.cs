using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

public class TextButtonParts : MonoBehaviour
{
    public UnityEvent clickEvent = null;
    [SerializeField] private TextEl textEl = null;

    [SerializeField] private GameObject hoverImageGroup = null;
    [SerializeField] private GameObject hoverText = null;

    public void HoverEvent()
    {
        textEl.ShowImages(false, textEl.initGroup.name);
        textEl.ShowText(false, textEl.initText.name);

        textEl.ShowImages(true, hoverImageGroup.name);
        textEl.ShowText(true, hoverText.name);
    }

    public void UnHoverEvent()
    {
        textEl.ShowImages(false, hoverImageGroup.name);
        textEl.ShowText(false, hoverText.name);

        textEl.ShowImages(true, textEl.initGroup.name);
        textEl.ShowText(true, textEl.initText.name);
    }

    public void ClickEvent()
    {
        clickEvent.Invoke();
    }

    public void ShowInitText(bool isShow)
    {
        textEl.ShowText(isShow, textEl.initText.name);
    }

    public void ShowHoverText(bool isShow)
    {
        textEl.ShowText(isShow, hoverText.name);
    }

    public void EditInitText(string text)
    {
        textEl.initText.text = text;
    }

    public void EditHoverText(string text)
    {
        hoverText.GetComponent<Text>().text = text;
    }

    public string GetInitText()
    {
        return textEl.initText.text;
    }

    public string GetHoverText()
    {
        return hoverText.GetComponent<Text>().text;
    }
}
