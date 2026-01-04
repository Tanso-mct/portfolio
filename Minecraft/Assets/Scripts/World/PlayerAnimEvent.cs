using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Events;

public class PlayerAnimEvent : MonoBehaviour
{
    [SerializeField] private UnityEvent onArmAnimEnd;

    public void ArmAnimEnd()
    {
        onArmAnimEnd.Invoke();
    }
}
