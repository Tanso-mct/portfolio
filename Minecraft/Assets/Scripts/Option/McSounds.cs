 using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class McSounds : MonoBehaviour
{
    private string currentScene = "";
    private bool hasSaveData = false;

    [SerializeField] SelectBarParts masterSb;
    [SerializeField] SelectBarParts musicSb;
    [SerializeField] SelectBarParts blocksSb;
    [SerializeField] SelectBarParts uiSb;
    [SerializeField] SelectBarParts playersSb;

    private static int masterVolume = 50;

    private static int musicVolume = 50;

    private static int blocksVolume = 50;

    private static int uiVolume = 50;

    private static int playersVolume = 50;

    [SerializeField] private AudioSource sourceMusic;
    [SerializeField] private AudioSource sourceBlock;
    [SerializeField] private AudioSource sourceUI;
    [SerializeField] private AudioSource sourcePlayer;

    [SerializeField] private List<AudioClip> musicClips;
    [SerializeField] private List<AudioClip> blocksClips;
    [SerializeField] private List<AudioClip> uiClips;
    [SerializeField] private List<AudioClip> playersClips;

    private Dictionary<string, int> musicClipsIndex;
    private Dictionary<string, int> blocksClipsIndex;
    private Dictionary<string, int> uiClipsIndex;
    private Dictionary<string, int> playersClipsIndex;

    [SerializeField] private float clickStartTime = 0.5f;

    public void Init()
    {
        currentScene = "";

        if (hasSaveData)
        {
            // Load data
        }

        masterSb.Init(masterVolume);
        SetMaster();

        musicSb.Init(musicVolume);
        SetMusic();

        blocksSb.Init(blocksVolume);
        SetBlocks();

        uiSb.Init(uiVolume);
        SetUi();

        playersSb.Init(playersVolume);
        SetPlayers();

        musicClipsIndex = new Dictionary<string, int>();
        for (int i = 0; i < musicClips.Count; i++)
        {
            musicClipsIndex.Add(musicClips[i].name, i);
        }

        blocksClipsIndex = new Dictionary<string, int>();
        for (int i = 0; i < blocksClips.Count; i++)
        {
            blocksClipsIndex.Add(blocksClips[i].name, i);
        }

        uiClipsIndex = new Dictionary<string, int>();
        for (int i = 0; i < uiClips.Count; i++)
        {
            uiClipsIndex.Add(uiClips[i].name, i);
        }

        playersClipsIndex = new Dictionary<string, int>();
        for (int i = 0; i < playersClips.Count; i++)
        {
            playersClipsIndex.Add(playersClips[i].name, i);
        }

        sourceMusic.volume = (musicVolume / 100f) * (masterVolume / 100f);
        sourceBlock.volume = (blocksVolume / 100f) * (masterVolume / 100f);
        sourceUI.volume = (uiVolume / 100f) * (masterVolume / 100f);
        sourcePlayer.volume = (playersVolume / 100f) * (masterVolume / 100f);

    }

    public void Save()
    {
        // Save data
    }

    void Update()
    {
        if (currentScene != "")
        {
            if (!sourceMusic.isPlaying)
            {
                PlayMusic(currentScene);
            }
        }
    }

    public void PlayMusic(string scene)
    {
        currentScene = scene;
        if (scene == Constants.SCENE_MENU)
        {
            int randomValue = Random.Range(1, 5);
            if (musicClipsIndex.ContainsKey(Constants.SOUND_MENU + randomValue))
            {
                sourceMusic.clip = musicClips[musicClipsIndex[Constants.SOUND_MENU + randomValue]];
                sourceMusic.Play();
            }
        }
        else if (scene == Constants.SCENE_PLAY)
        {
            int randomValue = Random.Range(1, 5);
            if (musicClipsIndex.ContainsKey(Constants.SOUND_HAL + randomValue))
            {
                sourceMusic.clip = musicClips[musicClipsIndex[Constants.SOUND_HAL + randomValue]];
                sourceMusic.Play();
            }
        }
    }

    public void PlayBlock(string clipName)
    {
        int randomValue = Random.Range(1, 5);
        if (blocksClipsIndex.ContainsKey(clipName + randomValue))
        {
            sourceBlock.clip = blocksClips[blocksClipsIndex[clipName + randomValue]];
            sourceBlock.Play();
        }
    }

    public void PlayUI(string clipName)
    {
        if (uiClipsIndex.ContainsKey(clipName))
        {
            sourceUI.clip = uiClips[uiClipsIndex[clipName]];
            sourceUI.time = clickStartTime;
            sourceUI.Play();
        }
    }

    public void PlayPlayer(string clipName)
    {
        if (playersClipsIndex.ContainsKey(clipName))
        {
            sourcePlayer.clip = playersClips[playersClipsIndex[clipName]];
            sourcePlayer.Play();
        }
    }

    public void SetMaster()
    {
        masterVolume = (int)masterSb.Val;

        sourceMusic.volume = (musicVolume / 100f) * (masterVolume / 100f);
        sourceBlock.volume = (blocksVolume / 100f) * (masterVolume / 100f);
        sourceUI.volume = (uiVolume / 100f) * (masterVolume / 100f);
        sourcePlayer.volume = (playersVolume / 100f) * (masterVolume / 100f);

        masterSb.EditTxt("Master Volume: " + masterVolume + "%");
    }

    public void SetMusic()
    {
        musicVolume = (int)musicSb.Val;

        sourceMusic.volume = (musicVolume / 100f) * (masterVolume / 100f);

        musicSb.EditTxt("Music: " + musicVolume + "%");
    }

    public void SetBlocks()
    {
        blocksVolume = (int)blocksSb.Val;

        sourceBlock.volume = (blocksVolume / 100f) * (masterVolume / 100f);

        blocksSb.EditTxt("Blocks: " + blocksVolume + "%");
    }

    public void SetUi()
    {
        uiVolume = (int)uiSb.Val;

        sourceUI.volume = (uiVolume / 100f) * (masterVolume / 100f);

        uiSb.EditTxt("UI: " + uiVolume + "%");
    }

    public void SetPlayers()
    {
        playersVolume = (int)playersSb.Val;

        sourcePlayer.volume = (playersVolume / 100f) * (masterVolume / 100f);

        playersSb.EditTxt("Players: " + playersVolume + "%");
    }
}
