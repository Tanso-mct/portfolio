using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class DeleteSlot : MonoBehaviour
{
    [SerializeField] private Container container;

    public void OnHover()
    {
        container.SetNowHoverSlot(-1);
    }

    public void UnHover()
    {
        container.SetNowHoverSlot(0);
    }
}
