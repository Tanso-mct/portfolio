using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class WorldShowParts : MonoBehaviour
{
    public string worldName = "World Name";
    [SerializeField] private TextEl textEl = null;
    [SerializeField] private GameObject selectingImageGroup = null;

    public void Create()
    {
        textEl.initText.text = worldName;
    }

    public void ClickEvent()
    {
        textEl.ShowImages(false, textEl.initGroup.name);
        textEl.ShowImages(true, selectingImageGroup.name);
    }

    public void UnClickEvent()
    {
        textEl.ShowImages(false, selectingImageGroup.name);
        textEl.ShowImages(true, textEl.initGroup.name);
    }

}
