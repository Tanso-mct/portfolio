using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MenuWindow : GameWindow
{
    [SerializeField] protected List<GameObject> inputBoxes = null;
    [SerializeField] protected List<GameObject> buttons = null;
    [SerializeField] protected List<GameObject> selectBars = null;

    protected Dictionary<string, Element> diInputBoxEl;
    protected Dictionary<string, Element> diButtonEl;
    protected Dictionary<string, Element> diSelectBarEl;

    public override void Init()
    {
        // 各キャンバスとパネルを非表示にする
        ShowCanvases(false);
        ShowBackgrounds(false);

        // 各辞書変数を初期化
        diImageEl = new Dictionary<string, Element>();
        diTextEl = new Dictionary<string, Element>();

        diInputBoxEl = new Dictionary<string, Element>();
        diButtonEl = new Dictionary<string, Element>();
        diSelectBarEl = new Dictionary<string, Element>();

        // 各要素を取得
        GetElements(ref images, ref diImageEl);
        GetElements(ref texts, ref diTextEl);

        GetElements(ref inputBoxes, ref diInputBoxEl);
        GetElements(ref buttons, ref diButtonEl);
        GetElements(ref selectBars, ref diSelectBarEl);

        // 各要素を初期化
        ElementsInit(ref diImageEl);
        ElementsInit(ref diTextEl);

        ElementsInit(ref diInputBoxEl);
        ElementsInit(ref diButtonEl);
        ElementsInit(ref diSelectBarEl);

    }

    public override void Show()
    {
        // 開いている場合は処理しない
        if (isOpening) return;
        isOpening = true;

        // キャンバスとパネルを表示
        ShowCanvases(true);
        ShowBackgrounds(true);

        // 各要素を表示
        ElementsShow(ref diImageEl);
        ElementsShow(ref diTextEl);

        ElementsShow(ref diInputBoxEl);
        ElementsShow(ref diButtonEl);
        ElementsShow(ref diSelectBarEl);
    }

    public override void Close()
    {
        // 開いていない場合は処理しない
        if (!isOpening) return;
        isOpening = false;

        // キャンバスとパネルを非表示
        ShowCanvases(false);
        ShowBackgrounds(false);

        // 各要素を非表示
        ElementsClose(ref diImageEl);
        ElementsClose(ref diTextEl);

        ElementsClose(ref diInputBoxEl);
        ElementsClose(ref diButtonEl);
        ElementsClose(ref diSelectBarEl);
    }

    public override void Execute()
    {
        // 開いていない場合は処理しない
        if (!isOpening) return;

        // 各要素を実行
        ElementsExecute(ref diImageEl);
        ElementsExecute(ref diTextEl);

        ElementsExecute(ref diInputBoxEl);
        ElementsExecute(ref diButtonEl);
        ElementsExecute(ref diSelectBarEl);
    }

    public override void Move(ref Vector2 moveVec)
    {
        if (!isOpening) return;

        // 各要素を移動
        ElementsMove(ref diImageEl, ref moveVec);
        ElementsMove(ref diTextEl, ref moveVec);

        ElementsMove(ref diInputBoxEl, ref moveVec);
        ElementsMove(ref diButtonEl, ref moveVec);
        ElementsMove(ref diSelectBarEl, ref moveVec);
    }
}
