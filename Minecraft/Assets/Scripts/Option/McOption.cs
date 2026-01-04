using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class McOption : MonoBehaviour
{
    public McVideos videos;
    public McControls controls;
    public McSounds sounds;

    public void Init()
    {
        videos.Init();
        controls.Init();
        sounds.Init();
    }

    public void Save()
    {
        videos.Save();
        controls.Save();
        sounds.Save();
    }
}
