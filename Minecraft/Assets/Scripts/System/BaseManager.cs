using System.Collections;
using System.Collections.Generic;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

public class BaseManager : MonoBehaviour
{
    // Managerクラスを継承したクラスを持つゲームオブジェクトを設定
    [SerializeField] private GameObject managerObject;
    private Manager manager;
    [SerializeField] private McOption option;

    private void Awake()
    {
        // メッセージ処理のためのパラメータを初期化
        Param.Init();

        // Managerクラスを継承したクラスを持つゲームオブジェクトからManagerクラスを取得
        manager = managerObject.GetComponent<Manager>();

        // Managerクラスを継承したクラスのAwake関数を実行
        manager.BaseAwake();
    }

    void Start()
    {
        // 各種設定の初期化
        option.Init();

        // メッセージ処理のためのパラメータを初期化
        Param.Init();

        // FPSを設定
        SetFps();

        // 解像度を設定
        SetResolution();

        // Managerクラスを継承したクラスのStart関数を実行
        manager.BaseStart();

        // メッセージ処理
        ProcessParam();
    }

    void Update()
    {
        // メッセージ処理のためのパラメータを初期化a
        Param.Init();

        // Managerクラスを継承したクラスのUpdate関数を実行
        manager.BaseUpdate();

        // メッセージ処理
        ProcessParam();
    }

    // シーンが切り替わる際に呼び出される関数
    private void Exit()
    {
        // Managerクラスを継承したクラスのExit関数を実行
        manager.BaseExit();
    }

    public void ProcessParam()
    {
        switch (Param.msg)
        {
            case Constants.MSG_NULL:
                break;
            case Constants.MSG_CHANGE_SCENE:
                ChangeScene();
                break;
            case Constants.MSG_QUIT_GAME:
                QuitGame();
                break;
            default:
                break;
        }
    }

    public void QuitGame()
    {
        option.Save();

    #if UNITY_EDITOR
        EditorApplication.isPlaying = false;
    #else
        Application.Quit();
    #endif
    }

    // 各シーンで必ず一度実行する。FPSの設定を行う。
    public int SetFps()
    {
        QualitySettings.vSyncCount = 0;
        Application.targetFrameRate = Constants.SPECIFIED_FPS;

        return Constants.MSG_SUCCESS;
    }

    public int SetResolution()
    {
        Screen.SetResolution(Constants.SCREEN_WIDTH, Constants.SCREEN_HEIGHT, true);

        return Constants.MSG_SUCCESS;
    }

    // paramに格納されたシーン名を元に、シーンを変更する。
    public int ChangeScene()
    {
        if (Param.strPar == Constants.SCENE_MENU)
        {
            Exit();
            UnityEngine.SceneManagement.SceneManager.LoadScene(Constants.SCENE_MENU);
        }
        else if (Param.strPar == Constants.SCENE_PLAY)
        {
            Exit();
            UnityEngine.SceneManagement.SceneManager.LoadScene(Constants.SCENE_PLAY);
        }
        else
        {
            return Constants.MSG_FAILED;
        }

        return Constants.MSG_FAILED;
    }
}
