using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

abstract public class GameWindow : MonoBehaviour
{
    protected bool isCreated = false;
    public bool IsCreated
        { get { return isCreated; } }

    protected bool isOpening = false;
    public bool IsOpening
        { get { return isOpening; } }

    [SerializeField] protected bool isScroll = false;
    public bool IsScroll
        { get { return isScroll; } }

    [SerializeField] protected bool isPopUp = false;
    public bool IsPopUp
        { get { return isPopUp; } }

    [SerializeField] private Canvas canvas;
    [SerializeField] private Image background;

    [SerializeField] protected List<GameObject> images = null;
    [SerializeField] protected List<GameObject> texts = null;

    protected Dictionary<string, Element> diImageEl;
    protected Dictionary<string, Element> diTextEl;

    // 孫オブジェクトをすべて取得
    protected List<GameObject> GetAllChildren(GameObject parent)
    {
        List<GameObject> children = new List<GameObject>();
        foreach (Transform child in parent.transform)
        {
            children.Add(child.gameObject);
        }
        return children;
    }

    protected void GetElements(ref List<GameObject> parents, ref Dictionary<string, Element> diEl)
    {
        for (int i = 0; i < parents.Count; i++)
        {
            List<GameObject> children = GetAllChildren(parents[i]);
            for (int j = 0; j < children.Count; j++)
            {
                if (!diEl.TryAdd(children[j].name, children[j].GetComponent<Element>()))
                {
                    Debug.LogError("Failed to add " + children[j].name + " to diEl.");
                }
            }
        }
    }

    protected void ElementsInit(ref Dictionary<string, Element> diEl)
    {
        foreach (KeyValuePair<string, Element> pair in diEl)
        {
            pair.Value.Init();
        }
    }

    protected void ShowCanvases(bool val)
    {
        canvas.enabled = val;
    }

    protected void ShowBackgrounds(bool val)
    {
        background.enabled = val;
    }

    abstract public void Init();

    protected void ElementsShow(ref Dictionary<string, Element> diEl)
    {
        foreach (KeyValuePair<string, Element> pair in diEl)
        {
            pair.Value.Show();
        }
    }

    abstract public void Show();

    protected void ElementsClose(ref Dictionary<string, Element> diEl)
    {
        foreach (KeyValuePair<string, Element> pair in diEl)
        {
            pair.Value.Close();
        }
    }

    abstract public void Close();

    protected void ElementsExecute(ref Dictionary<string, Element> diEl)
    {
        foreach (KeyValuePair<string, Element> pair in diEl)
        {
            pair.Value.Execute();
        }
    }

    abstract public void Execute();

    protected void ElementsMove(ref Dictionary<string, Element> diEl, ref Vector2 vec)
    {
        foreach (KeyValuePair<string, Element> pair in diEl)
        {
            pair.Value.Move(ref vec);
        }
    }

    abstract public void Move(ref Vector2 moveVec);
}
