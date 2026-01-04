using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MenuManager : Manager
{
    [SerializeField] private GameObject wndBuild;
    [SerializeField] private GameObject wndLoad;
    [SerializeField] private GameObject wndTitle;
    [SerializeField] private GameObject wndOption;
    [SerializeField] private GameObject wndVideoSetting;
    [SerializeField] private GameObject wndSoundSetting;
    [SerializeField] private GameObject wndControlSetting;
    [SerializeField] private GameObject wndControlSettingScroll;
    [SerializeField] private GameObject wndSinglePlayer;
    [SerializeField] private GameObject wndSinglePlayerScroll;

    [SerializeField] private int loadingShowFrame = 120;

    [SerializeField] private float controlScrollBottom;
    [SerializeField] private float singlePlayerScrollBottom;

    private int loadingFrame = 0;

    [SerializeField] private McSounds mcSounds;

    private int isStartBuildWorld = 0;

    public override void BaseAwake()
    {
        // Managerに設定されているすべてのWindowを初期化
        Init();

        // MenuWindowを表示
        ShowWindow(wndLoad.name);
    }

    public override void BaseStart()
    {
        // 各ウィンドウの処理を実行
        ExecuteWindows();

        // MUSIC再生
        mcSounds.PlayMusic(Constants.SCENE_MENU);

        // スクロールされている場合、ウィンドウを移動
        if (wndControlSetting.GetComponent<MenuWindow>().IsOpening)
        {
            ScrollWindows(controlScrollBottom);
        }
        else if (wndSinglePlayer.GetComponent<MenuWindow>().IsOpening)
        {
            ScrollWindows(singlePlayerScrollBottom);
        }
    }

    public override void BaseUpdate()
    {
        // 各ウィンドウの処理を実行
        ExecuteWindows();

        // スクロールされている場合、ウィンドウを移動
        if (wndControlSetting.GetComponent<MenuWindow>().IsOpening)
        {
            ScrollWindows(controlScrollBottom);
        }
        else if (wndSinglePlayer.GetComponent<MenuWindow>().IsOpening)
        {
            ScrollWindows(singlePlayerScrollBottom);
        }

        if (loadingFrame < loadingShowFrame)
        {
            loadingFrame++;
        }
        else if (loadingFrame == loadingShowFrame)
        {
            loadingFrame++; 
            CloseWindow(wndLoad.name);
            ShowWindow(wndTitle.name);
        }

        if (isStartBuildWorld == 1)
        {
            isStartBuildWorld++;
        }
        else if (isStartBuildWorld == 2)
        {
            isStartBuildWorld = 0;
            Param.msg = Constants.MSG_CHANGE_SCENE;
            Param.strPar = Constants.SCENE_PLAY;
        }
    }

    public override void BaseExit()
    {
        // Managerの終了処理を実行
        Dispose();
    }

    public void ShowOption()
    {
        CloseWindow(wndTitle.name);
        ShowWindow(wndOption.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void CloseOption()
    {
        CloseWindow(wndOption.name);
        ShowWindow(wndTitle.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
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

    public void ShowSinglePlayer()
    {
        CloseWindow(wndTitle.name);

        ShowWindow(wndSinglePlayer.name);
        ShowWindow(wndSinglePlayerScroll.name);

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void CloseSinglePlayer()
    {
        CloseWindow(wndSinglePlayer.name);
        CloseWindow(wndSinglePlayerScroll.name);

        ShowWindow(wndTitle.name);

        Param.popUpWindowDone = true;

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void QuitGame()
    {
        Param.msg = Constants.MSG_QUIT_GAME;

        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void PlaySelectWorld()
    {
        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }

    public void CreateNewWorld()
    {
        mcSounds.PlayUI(Constants.SOUND_CLICK);
        ShowWindow(wndBuild.name);

        isStartBuildWorld = 1;
    }

    public void DestroySelectWorld()
    {
        mcSounds.PlayUI(Constants.SOUND_CLICK);
    }
}
