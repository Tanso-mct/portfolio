using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

public class SelectBarEl : Element
{
    [SerializeField] public Text initText;
    [SerializeField] public GameObject texts;

    [SerializeField] public Image selector;

    [SerializeField] private float margin;

    private bool isHover = false;
    public UnityEvent hoverEvent = null;
    public UnityEvent unHoverEvent = null;

    public UnityEvent clickEvent = null;

    public override void Init()
    {
        // 継承元クラスの初期化処理を実行
        BaseInit();

        // テキストの初期化処理
        List<GameObject> textList = GetAllChildren(texts);

        diTexts = new Dictionary<string, Text>();
        for (int i = 0; i < textList.Count; i++)
        {
            textList[i].GetComponent<Text>().enabled = false;
            if (!diTexts.TryAdd(textList[i].name, textList[i].GetComponent<Text>()))
            {
                Debug.LogError("Failed to add " + textList[i].name + " to diTexts.");
            }
        }
    }

    public override void Show()
    {
        // 画像の表示処理
        ShowImages(true, initGroup.name);

        // テキストの表示処理
        ShowText(true, initText.name);
    }

    public override void Close()
    {
        // 画像の非表示処理
        ShowAllImages(false);

        // テキストの非表示処理
        ShowAllTexts(false);
    }

    public float GetBarMin()
    {
        return frame.GetComponent<RectTransform>().position.x - frame.GetComponent<RectTransform>().rect.width / 2 + margin;
    }

    public float GetBarMax()
    {
        return frame.GetComponent<RectTransform>().position.x + frame.GetComponent<RectTransform>().rect.width / 2 - margin;
    }

    public float GetFrameWidth()
    {
        return frame.GetComponent<RectTransform>().rect.width;
    }

    public override void Execute()
    {
        if (IsHover() && hoverEvent != null)
        {
            isHover = true;
            hoverEvent.Invoke();
        }
        else if (!IsHover() && isHover)
        {
            isHover = false;
            unHoverEvent.Invoke();
        }
        
        if (IsClick() && clickEvent != null)
        {
            clickEvent.Invoke();
        }
    }
}
