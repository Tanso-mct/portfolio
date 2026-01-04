using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class PlayerTrigger : MonoBehaviour
{
    [SerializeField] private Player player;

    void OnTriggerEnter(Collider other)
    {
        if (other.gameObject.tag == Constants.TAG_ENTITY)
        {
            player.PickUpItem(other.gameObject);
        }
    }   
}
