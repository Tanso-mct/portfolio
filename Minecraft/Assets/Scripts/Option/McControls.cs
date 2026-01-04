using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class McControls : MonoBehaviour
{
    private static bool hasSaveData = false;

    [SerializeField] private SelectBarParts senSb;
    private float senMax = 8;
    private float senMin = 0.001f;
    private static float sen = 4;
    private static Vector2 senSbPos;

    static private KeyCode attackKey = KeyCode.Mouse0;
    static private KeyCode dropKey = KeyCode.Q;
    static private KeyCode useKey = KeyCode.Mouse1;

    static private KeyCode hotBar1Key = KeyCode.Alpha1;
    static private KeyCode hotBar2Key = KeyCode.Alpha2;
    static private KeyCode hotBar3Key = KeyCode.Alpha3;
    static private KeyCode hotBar4Key = KeyCode.Alpha4;
    static private KeyCode hotBar5Key = KeyCode.Alpha5;
    static private KeyCode hotBar6Key = KeyCode.Alpha6;
    static private KeyCode hotBar7Key = KeyCode.Alpha7;
    static private KeyCode hotBar8Key = KeyCode.Alpha8;
    static private KeyCode hotBar9Key = KeyCode.Alpha9;

    static private KeyCode inventoryKey = KeyCode.E;

    static private KeyCode jumpKey = KeyCode.Space;
    static private KeyCode sprintKey = KeyCode.LeftShift;
    static private KeyCode leftKey = KeyCode.A;
    static private KeyCode rightKey = KeyCode.D;
    static private KeyCode backKey = KeyCode.S;
    static private KeyCode forwardKey = KeyCode.W;

    private TextButtonParts editingBtnParts = null;
    private int editingFrame = 0;
    private bool isEditing = false;
    private string editingBtnName;

    [SerializeField] private TextButtonParts attackBtnParts = null;
    [SerializeField] private TextButtonParts dropBtnParts = null;
    [SerializeField] private TextButtonParts useBtnParts = null;

    [SerializeField] private TextButtonParts hotBar1BtnParts = null;
    [SerializeField] private TextButtonParts hotBar2BtnParts = null;
    [SerializeField] private TextButtonParts hotBar3BtnParts = null;
    [SerializeField] private TextButtonParts hotBar4BtnParts = null;
    [SerializeField] private TextButtonParts hotBar5BtnParts = null;
    [SerializeField] private TextButtonParts hotBar6BtnParts = null;
    [SerializeField] private TextButtonParts hotBar7BtnParts = null;
    [SerializeField] private TextButtonParts hotBar8BtnParts = null;
    [SerializeField] private TextButtonParts hotBar9BtnParts = null;

    [SerializeField] private TextButtonParts inventoryBtnParts = null;
    
    [SerializeField] private TextButtonParts jumpBtnParts = null;
    [SerializeField] private TextButtonParts sprintBtnParts = null;
    [SerializeField] private TextButtonParts leftBtnParts = null;
    [SerializeField] private TextButtonParts rightBtnParts = null;
    [SerializeField] private TextButtonParts backBtnParts = null;
    [SerializeField] private TextButtonParts forwardBtnParts = null;

    [SerializeField] private McSounds mcSounds = null;

    public void Init()
    {
        if (hasSaveData)
        {
            // Load data
        }

        senSb.Init((int)(((sen / senMax) * 100f)));
        senSb.EditTxt("Sensitivity: " + (int)(((sen / senMax) * 100f)) + "%");

        attackBtnParts.EditInitText(attackKey.ToString());
        attackBtnParts.EditHoverText(attackKey.ToString());
        dropBtnParts.EditInitText(dropKey.ToString());
        dropBtnParts.EditHoverText(dropKey.ToString());
        useBtnParts.EditInitText(useKey.ToString());
        useBtnParts.EditHoverText(useKey.ToString());


        hotBar1BtnParts.EditInitText(hotBar1Key.ToString());
        hotBar1BtnParts.EditHoverText(hotBar1Key.ToString());
        hotBar2BtnParts.EditInitText(hotBar2Key.ToString());
        hotBar2BtnParts.EditHoverText(hotBar2Key.ToString());
        hotBar3BtnParts.EditInitText(hotBar3Key.ToString());
        hotBar3BtnParts.EditHoverText(hotBar3Key.ToString());
        hotBar4BtnParts.EditInitText(hotBar4Key.ToString());
        hotBar4BtnParts.EditHoverText(hotBar4Key.ToString());
        hotBar5BtnParts.EditInitText(hotBar5Key.ToString());
        hotBar5BtnParts.EditHoverText(hotBar5Key.ToString());
        hotBar6BtnParts.EditInitText(hotBar6Key.ToString());
        hotBar6BtnParts.EditHoverText(hotBar6Key.ToString());
        hotBar7BtnParts.EditInitText(hotBar7Key.ToString());
        hotBar7BtnParts.EditHoverText(hotBar7Key.ToString());
        hotBar8BtnParts.EditInitText(hotBar8Key.ToString());
        hotBar8BtnParts.EditHoverText(hotBar8Key.ToString());
        hotBar9BtnParts.EditInitText(hotBar9Key.ToString());
        hotBar9BtnParts.EditHoverText(hotBar9Key.ToString());

        inventoryBtnParts.EditInitText(inventoryKey.ToString());
        inventoryBtnParts.EditHoverText(inventoryKey.ToString());

        jumpBtnParts.EditInitText(jumpKey.ToString());
        jumpBtnParts.EditHoverText(jumpKey.ToString());
        sprintBtnParts.EditInitText(sprintKey.ToString());
        sprintBtnParts.EditHoverText(sprintKey.ToString());
        leftBtnParts.EditInitText(leftKey.ToString());
        leftBtnParts.EditHoverText(leftKey.ToString());
        rightBtnParts.EditInitText(rightKey.ToString());
        rightBtnParts.EditHoverText(rightKey.ToString());
        backBtnParts.EditInitText(backKey.ToString());
        backBtnParts.EditHoverText(backKey.ToString());
        forwardBtnParts.EditInitText(forwardKey.ToString());
        forwardBtnParts.EditHoverText(forwardKey.ToString());
    }

    public void Save()
    {
        // Save data
    }

    public void ControlBackground()
    {
        Param.popUpWindowDone = true;
    }

    public void SenEdit()
    {
        sen = senMin + (senMax - senMin) * senSb.Val / 100f;

        senSbPos = senSb.SelectorPos;

        senSb.EditTxt("Sensitivity: " + (int)(((sen / senMax) * 100f)) + "%");

        Param.popUpWindowDone = true;
    }

    private bool GetInputKey(ref KeyCode target)
    {
        if (Input.anyKeyDown)
        {
            foreach (KeyCode vKey in System.Enum.GetValues(typeof(KeyCode)))
            {
                if (Input.GetKey(vKey))
                {
                    target = vKey;
                    return true;
                }
            }
        }

        return false;
    }

    public void BindEdit(GameObject btnObj)
    {
        if (isEditing) return;

        editingBtnName = btnObj.name.Substring(0, btnObj.name.IndexOf("_"));

        editingBtnParts =  btnObj.GetComponent<TextButtonParts>();

        editingBtnParts.ShowInitText(false);
        editingBtnParts.ShowHoverText(true);

        string strEditing = "<  " + editingBtnParts.GetHoverText() + "  >";
        editingBtnParts.EditHoverText(strEditing);

        isEditing = true;
        editingFrame = 0;

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    private void Update()
    {
        if (editingFrame == 0 && isEditing)
        {
            editingFrame++;
        }
        else if (editingFrame == 1 && isEditing)
        {
            BindSet();
        }
    }

    private void BindInput(ref KeyCode target)
    {
        if (GetInputKey(ref target))
        {
            editingBtnParts.EditInitText(target.ToString());
            editingBtnParts.EditHoverText(target.ToString());
            editingBtnParts.ShowInitText(true);
            editingBtnParts.ShowHoverText(false);
            isEditing = false;
            editingFrame = 0;
        }
    }

    private void BindSet()
    {
        switch (editingBtnName)
        {
            case Constants.CONTROL_ATTACK:
                BindInput(ref attackKey);
                return;

            case Constants.CONTROL_DROP_ITEM:
                BindInput(ref dropKey);
                return;

            case Constants.CONTROL_USE:
                BindInput(ref useKey);
                return;

            case Constants.CONTROL_HS1:
                BindInput(ref hotBar1Key);
                return;

            case Constants.CONTROL_HS2:
                BindInput(ref hotBar2Key);
                return;

            case Constants.CONTROL_HS3:
                BindInput(ref hotBar3Key);
                return;

            case Constants.CONTROL_HS4:
                BindInput(ref hotBar4Key);
                return;

            case Constants.CONTROL_HS5:
                BindInput(ref hotBar5Key);
                return;

            case Constants.CONTROL_HS6:
                BindInput(ref hotBar6Key);
                return;

            case Constants.CONTROL_HS7:
                BindInput(ref hotBar7Key);
                return;

            case Constants.CONTROL_HS8:
                BindInput(ref hotBar8Key);
                return;

            case Constants.CONTROL_HS9:
                BindInput(ref hotBar9Key);
                return;

            case Constants.CONTROL_INVENTORY:
                BindInput(ref inventoryKey);
                return;

            case Constants.CONTROL_JUMP:
                BindInput(ref jumpKey);
                return;

            case Constants.CONTROL_SPRINT:
                BindInput(ref sprintKey);
                return;

            case Constants.CONTROL_LEFT:
                BindInput(ref leftKey);
                return;

            case Constants.CONTROL_RIGHT:
                BindInput(ref rightKey);
                return;

            case Constants.CONTROL_BACK:
                BindInput(ref backKey);
                return;

            case Constants.CONTROL_FOR:
                BindInput(ref forwardKey);
                return;

            default: 
                return;
        }
    }

    private void KeyReset(ref TextButtonParts txtBtnParts, ref KeyCode key, KeyCode defKey)
    {
        txtBtnParts.EditInitText(defKey.ToString());
        txtBtnParts.EditHoverText(defKey.ToString());
        key = defKey;
    }

    public void BindReset(GameObject btnObj)
    {
        string btnName = btnObj.name.Substring(0, btnObj.name.IndexOf("_"));

        switch (btnName)
        {
            case Constants.CONTROL_ATTACK:
                KeyReset(ref attackBtnParts, ref attackKey, Constants.KEY_ATTACK);
                return;

            case Constants.CONTROL_DROP_ITEM:
                KeyReset(ref dropBtnParts, ref dropKey, Constants.KEY_DROP);
                return;

            case Constants.CONTROL_USE:
                KeyReset(ref useBtnParts, ref useKey, Constants.KEY_USE);
                return;

            case Constants.CONTROL_HS1:
                KeyReset(ref hotBar1BtnParts, ref hotBar1Key, Constants.KEY_HOT_BAR_1);
                return;

            case Constants.CONTROL_HS2:
                KeyReset(ref hotBar2BtnParts, ref hotBar2Key, Constants.KEY_HOT_BAR_2);
                return;

            case Constants.CONTROL_HS3:
                KeyReset(ref hotBar3BtnParts, ref hotBar3Key, Constants.KEY_HOT_BAR_3);
                return;

            case Constants.CONTROL_HS4:
                KeyReset(ref hotBar4BtnParts, ref hotBar4Key, Constants.KEY_HOT_BAR_4);
                return;

            case Constants.CONTROL_HS5:
                KeyReset(ref hotBar5BtnParts, ref hotBar5Key, Constants.KEY_HOT_BAR_5);
                return;

            case Constants.CONTROL_HS6:
                KeyReset(ref hotBar6BtnParts, ref hotBar6Key, Constants.KEY_HOT_BAR_6);
                return;

            case Constants.CONTROL_HS7:
                KeyReset(ref hotBar7BtnParts, ref hotBar7Key, Constants.KEY_HOT_BAR_7);
                return;

            case Constants.CONTROL_HS8:
                KeyReset(ref hotBar8BtnParts, ref hotBar8Key, Constants.KEY_HOT_BAR_8);
                return;

            case Constants.CONTROL_HS9:
                KeyReset(ref hotBar9BtnParts, ref hotBar9Key, Constants.KEY_HOT_BAR_9);
                return;

            case Constants.CONTROL_INVENTORY:
                KeyReset(ref inventoryBtnParts, ref inventoryKey, Constants.KEY_INVENTORY);
                return;

            case Constants.CONTROL_JUMP:
                KeyReset(ref jumpBtnParts, ref jumpKey, Constants.KEY_JUMP);
                return;

            case Constants.CONTROL_SPRINT:
                KeyReset(ref sprintBtnParts, ref sprintKey, Constants.KEY_SPRINT);
                return;

            case Constants.CONTROL_LEFT:
                KeyReset(ref leftBtnParts, ref leftKey, Constants.KEY_LEFT);
                return;

            case Constants.CONTROL_RIGHT:
                KeyReset(ref rightBtnParts, ref rightKey, Constants.KEY_RIGHT);
                return;

            case Constants.CONTROL_BACK:
                KeyReset(ref backBtnParts, ref backKey, Constants.KEY_BACK);
                return;

            case Constants.CONTROL_FOR:
                KeyReset(ref forwardBtnParts, ref forwardKey, Constants.KEY_FORWARD);
                return;

            default: 
                return;
        }
    }

    public void BindResetAll()
    {
        KeyReset(ref attackBtnParts, ref attackKey, Constants.KEY_ATTACK);
        KeyReset(ref dropBtnParts, ref dropKey, Constants.KEY_DROP);
        KeyReset(ref useBtnParts, ref useKey, Constants.KEY_USE);

        KeyReset(ref hotBar1BtnParts, ref hotBar1Key, Constants.KEY_HOT_BAR_1);
        KeyReset(ref hotBar2BtnParts, ref hotBar2Key, Constants.KEY_HOT_BAR_2);
        KeyReset(ref hotBar3BtnParts, ref hotBar3Key, Constants.KEY_HOT_BAR_3);
        KeyReset(ref hotBar4BtnParts, ref hotBar4Key, Constants.KEY_HOT_BAR_4);
        KeyReset(ref hotBar5BtnParts, ref hotBar5Key, Constants.KEY_HOT_BAR_5);
        KeyReset(ref hotBar6BtnParts, ref hotBar6Key, Constants.KEY_HOT_BAR_6);
        KeyReset(ref hotBar7BtnParts, ref hotBar7Key, Constants.KEY_HOT_BAR_7);
        KeyReset(ref hotBar8BtnParts, ref hotBar8Key, Constants.KEY_HOT_BAR_8);
        KeyReset(ref hotBar9BtnParts, ref hotBar9Key, Constants.KEY_HOT_BAR_9);

        KeyReset(ref inventoryBtnParts, ref inventoryKey, Constants.KEY_INVENTORY);

        KeyReset(ref jumpBtnParts, ref jumpKey, Constants.KEY_JUMP);
        KeyReset(ref sprintBtnParts, ref sprintKey, Constants.KEY_SPRINT);
        KeyReset(ref leftBtnParts, ref leftKey, Constants.KEY_LEFT);
        KeyReset(ref rightBtnParts, ref rightKey, Constants.KEY_RIGHT);
        KeyReset(ref backBtnParts, ref backKey, Constants.KEY_BACK);
        KeyReset(ref forwardBtnParts, ref forwardKey, Constants.KEY_FORWARD);
    }

    static public void CursorLock(bool symbol)
    {
        if (symbol)
        {
            Cursor.visible = false;
            Cursor.lockState = CursorLockMode.Locked;
        }
        else
        {
            Cursor.visible = true;
            Cursor.lockState = CursorLockMode.None;
        }
    }

    public static Vector2 GetMouseAxis()
    {
        Vector2 rtVec = new Vector2(Input.GetAxis("Mouse X"), Input.GetAxis("Mouse Y"));
        rtVec.x *= sen;
        rtVec.y *= sen;
        return rtVec;
    }

    static public bool IsKey(string control)
    {
        switch (control)
        {
            case Constants.CONTROL_ATTACK:
                return Input.GetKey(attackKey);

            case Constants.CONTROL_DROP_ITEM:
                return Input.GetKey(dropKey);

            case Constants.CONTROL_USE:
                return Input.GetKey(useKey);

            case Constants.CONTROL_HS1:
                return Input.GetKey(hotBar1Key);

            case Constants.CONTROL_HS2:
                return Input.GetKey(hotBar2Key);

            case Constants.CONTROL_HS3:
                return Input.GetKey(hotBar3Key);

            case Constants.CONTROL_HS4:
                return Input.GetKey(hotBar4Key);

            case Constants.CONTROL_HS5:
                return Input.GetKey(hotBar5Key);

            case Constants.CONTROL_HS6:
                return Input.GetKey(hotBar6Key);

            case Constants.CONTROL_HS7:
                return Input.GetKey(hotBar7Key);

            case Constants.CONTROL_HS8:
                return Input.GetKey(hotBar8Key);

            case Constants.CONTROL_HS9:
                return Input.GetKey(hotBar9Key);

            case Constants.CONTROL_INVENTORY:
                return Input.GetKey(inventoryKey);

            case Constants.CONTROL_JUMP:
                return Input.GetKey(jumpKey);

            case Constants.CONTROL_SPRINT:
                return Input.GetKey(sprintKey);

            case Constants.CONTROL_LEFT:
                return Input.GetKey(leftKey);

            case Constants.CONTROL_RIGHT:
                return Input.GetKey(rightKey);

            case Constants.CONTROL_BACK:
                return Input.GetKey(backKey);

            case Constants.CONTROL_FOR:
                return Input.GetKey(forwardKey);

            default: return false;
        }
    }

    static public bool IsKeyDown(string control)
    {
        switch (control)
        {
            case Constants.CONTROL_ATTACK:
                return Input.GetKeyDown(attackKey);

            case Constants.CONTROL_DROP_ITEM:
                return Input.GetKeyDown(dropKey);

            case Constants.CONTROL_USE:
                return Input.GetKeyDown(useKey);

            case Constants.CONTROL_HS1:
                return Input.GetKeyDown(hotBar1Key);

            case Constants.CONTROL_HS2:
                return Input.GetKeyDown(hotBar2Key);

            case Constants.CONTROL_HS3:
                return Input.GetKeyDown(hotBar3Key);

            case Constants.CONTROL_HS4:
                return Input.GetKeyDown(hotBar4Key);

            case Constants.CONTROL_HS5:
                return Input.GetKeyDown(hotBar5Key);

            case Constants.CONTROL_HS6:
                return Input.GetKeyDown(hotBar6Key);

            case Constants.CONTROL_HS7:
                return Input.GetKeyDown(hotBar7Key);

            case Constants.CONTROL_HS8:
                return Input.GetKeyDown(hotBar8Key);

            case Constants.CONTROL_HS9:
                return Input.GetKeyDown(hotBar9Key);

            case Constants.CONTROL_INVENTORY:
                return Input.GetKeyDown(inventoryKey);

            case Constants.CONTROL_JUMP:
                return Input.GetKeyDown(jumpKey);

            case Constants.CONTROL_SPRINT:
                return Input.GetKeyDown(sprintKey);

            case Constants.CONTROL_LEFT:
                return Input.GetKeyDown(leftKey);

            case Constants.CONTROL_RIGHT:
                return Input.GetKeyDown(rightKey);

            case Constants.CONTROL_BACK:
                return Input.GetKeyDown(backKey);

            case Constants.CONTROL_FOR:
                return Input.GetKeyDown(forwardKey);

            default: return false;
        }
    }

    static public bool IsKeyUp(string control)
    {
        switch (control)
        {
            case Constants.CONTROL_ATTACK:
                return Input.GetKeyUp(attackKey);

            case Constants.CONTROL_DROP_ITEM:
                return Input.GetKeyUp(dropKey);

            case Constants.CONTROL_USE:
                return Input.GetKeyUp(useKey);

            case Constants.CONTROL_HS1:
                return Input.GetKeyUp(hotBar1Key);

            case Constants.CONTROL_HS2:
                return Input.GetKeyUp(hotBar2Key);

            case Constants.CONTROL_HS3:
                return Input.GetKeyUp(hotBar3Key);

            case Constants.CONTROL_HS4:
                return Input.GetKeyUp(hotBar4Key);

            case Constants.CONTROL_HS5:
                return Input.GetKeyUp(hotBar5Key);

            case Constants.CONTROL_HS6:
                return Input.GetKeyUp(hotBar6Key);

            case Constants.CONTROL_HS7:
                return Input.GetKeyUp(hotBar7Key);

            case Constants.CONTROL_HS8:
                return Input.GetKeyUp(hotBar8Key);

            case Constants.CONTROL_HS9:
                return Input.GetKeyUp(hotBar9Key);

            case Constants.CONTROL_INVENTORY:
                return Input.GetKeyUp(inventoryKey);

            case Constants.CONTROL_JUMP:
                return Input.GetKeyUp(jumpKey);

            case Constants.CONTROL_SPRINT:
                return Input.GetKeyUp(sprintKey);

            case Constants.CONTROL_LEFT:
                return Input.GetKeyUp(leftKey);

            case Constants.CONTROL_RIGHT:
                return Input.GetKeyUp(rightKey);

            case Constants.CONTROL_BACK:
                return Input.GetKeyUp(backKey);

            case Constants.CONTROL_FOR:
                return Input.GetKeyUp(forwardKey);

            default: return false;
        }
    }
    
}
