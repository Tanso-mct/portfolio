using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayManager : Manager
{
    [SerializeField] private GameObject wndPlay;

    [SerializeField] private GameObject wndMenu;
    [SerializeField] private GameObject wndOption;
    [SerializeField] private GameObject wndVideoSetting;
    [SerializeField] private GameObject wndSoundSetting;
    [SerializeField] private GameObject wndControlSetting;
    [SerializeField] private GameObject wndControlSettingScroll;

    [SerializeField] private Player player;

    [SerializeField] private GameObject wndPlayerInventory;
    [SerializeField] private GameObject wndCreativeContainer;
    [SerializeField] private GameObject wndCreativeContainer2;

    [SerializeField] private float controlScrollBottom;

    [SerializeField] private McSounds mcSounds;

    static private int createdTimes = 0;

    public override void BaseAwake()
    {
        // Managerに設定されているすべてのWindowを初期化
        Init();

        // PlayWindowを表示
        ShowWindow(wndPlay.name);

        //ShowWindow(wndOption.name);
    }

    public override void BaseStart()
    {
        // 各ウィンドウの処理を実行
        ExecuteWindows();

        // MUSIC再生
        mcSounds.PlayMusic(Constants.SCENE_PLAY);

        // スクロールされている場合、ウィンドウを移動
        ScrollWindows();

        if (createdTimes != 0)
        {
            ShowMenu();
        }

        createdTimes++;
    }

    public override void BaseUpdate()
    {
        if 
        (
            Input.GetKeyUp(KeyCode.Escape) && 
            wndPlay.gameObject.GetComponent<PlayWindow>().IsOpening &&
            !player.isInventoryOpen
        ){
            ShowMenu();
        }

        if (McControls.IsKeyDown(Constants.CONTROL_INVENTORY) && !player.isInventoryOpen)
        {
            ShowCreativeContainer();
        }
        else if (McControls.IsKeyDown(Constants.CONTROL_INVENTORY) && player.isInventoryOpen)
        {
            ClosePlayerInventory();
        }

        // 各ウィンドウの処理を実行
        ExecuteWindows();

        // スクロールされている場合、ウィンドウを移動
        if (wndControlSetting.GetComponent<MenuWindow>().IsOpening) ScrollWindows(controlScrollBottom);
    }

    public override void BaseExit()
    {
        // Managerの終了処理を実行
        Dispose();
    }

    public void ShowMenu()
    {
        CloseWindow(wndPlay.name);
        Physics.simulationMode = SimulationMode.Script;
        McControls.CursorLock(false);
        
        ShowWindow(wndMenu.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void CloseMenu()
    {
        CloseWindow(wndMenu.name);
        Physics.simulationMode = SimulationMode.FixedUpdate;
        McControls.CursorLock(true);

        ShowWindow(wndPlay.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void SaveAndQuit()
    {
        Param.msg = Constants.MSG_CHANGE_SCENE;
        Param.strPar = Constants.SCENE_MENU;
    }

    public void ShowOption()
    {
        CloseWindow(wndMenu.name);
        ShowWindow(wndOption.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void CloseOption()
    {
        CloseWindow(wndOption.name);
        ShowWindow(wndMenu.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void ShowPlayerInventory()
    {
        CloseWindow(wndCreativeContainer.name);
        CloseWindow(wndCreativeContainer2.name);

        McControls.CursorLock(false);
        player.isInventoryOpen = true;
        ShowWindow(wndPlayerInventory.name);
    }

    public void ClosePlayerInventory()
    {
        McControls.CursorLock(true);
        player.isInventoryOpen = false;

        CloseWindow(wndPlayerInventory.name);
        CloseWindow(wndCreativeContainer.name);
        CloseWindow(wndCreativeContainer2.name);
    }

    public void ShowCreativeContainer()
    {
        CloseWindow(wndPlayerInventory.name);
        CloseWindow(wndCreativeContainer2.name);

        McControls.CursorLock(false);
        player.isInventoryOpen = true;
        ShowWindow(wndCreativeContainer.name);
    }

    public void ShowCreativeContainer2()
    {
        CloseWindow(wndPlayerInventory.name);
        CloseWindow(wndCreativeContainer.name);

        McControls.CursorLock(false);
        player.isInventoryOpen = true;
        ShowWindow(wndCreativeContainer2.name);
    }

    public void ShowVideoSetting()
    {
        CloseWindow(wndOption.name);
        ShowWindow(wndVideoSetting.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void CloseVideoSetting()
    {
        CloseWindow(wndVideoSetting.name);
        ShowWindow(wndOption.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void ShowSoundSetting()
    {
        CloseWindow(wndOption.name);
        ShowWindow(wndSoundSetting.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void CloseSoundSetting()
    {
        CloseWindow(wndSoundSetting.name);
        ShowWindow(wndOption.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void ShowControlSetting()
    {
        CloseWindow(wndOption.name);

        ShowWindow(wndControlSetting.name);
        ShowWindow(wndControlSettingScroll.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void CloseControlSetting()
    {
        CloseWindow(wndControlSetting.name);
        CloseWindow(wndControlSettingScroll.name);

        ShowWindow(wndOption.name);

        Param.popUpWindowDone = true;

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }
}
