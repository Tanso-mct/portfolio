using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class McHitBoxAdmin : MonoBehaviour
{
    int id = 0;
    private List<KeyValuePair<int, Vector3>> boxPosList = new List<KeyValuePair<int, Vector3>>();
    [HideInInspector] public Vector3[] boxPosAry;

    private List<KeyValuePair<int, Vector3>> boxSizeList = new List<KeyValuePair<int, Vector3>>();
    [HideInInspector] public Vector3[] boxSizeAry;

    [HideInInspector] public int[] hitBlockTypeAry;

    public int RegisterHitBox(Vector3 pos, Vector3 size, Vector3 moveVec)
    {
        id++;

        boxPosList.Add(new KeyValuePair<int, Vector3>(id, pos));
        boxSizeList.Add(new KeyValuePair<int, Vector3>(id, size));

        return id;
    }

    public void UpdatePos(int id, Vector3 pos)
    {
        for (int i = 0; i < boxPosList.Count; i++)
        {
            if (boxPosList[i].Key == id)
            {
                boxPosList[i] = new KeyValuePair<int, Vector3>(id, pos);
                break;
            }
        }
    }

    public void UnRegister(int id)
    {
        for (int i = 0; i < boxPosList.Count; i++)
        {
            if (boxPosList[i].Key == id)
            {
                boxPosList.RemoveAt(i);
                break;
            }
        }

        for (int i = 0; i < boxSizeList.Count; i++)
        {
            if (boxSizeList[i].Key == id)
            {
                boxSizeList.RemoveAt(i);
                break;
            }
        }
    }

    public int GetHitBlockType(int hitBoxId)
    {
        for (int i = 0; i < hitBlockTypeAry.Length; i++)
        {
            if (hitBlockTypeAry[i] == hitBoxId)
            {
                return (int)hitBlockTypeAry[i];
            }
        }

        return 0;
    }

    public int GetHitBoxAmount()
    {
        return boxPosList.Count;
    }

    public void CreateAry()
    {
        boxPosAry = new Vector3[boxPosList.Count];
        for (int i = 0; i < boxPosList.Count; i++)
        {
            boxPosAry[i] = boxPosList[i].Value;
        }

        boxSizeAry = new Vector3[boxSizeList.Count];
        for (int i = 0; i < boxSizeList.Count; i++)
        {
            boxSizeAry[i] = boxSizeList[i].Value;
        }

        hitBlockTypeAry = new int[boxPosList.Count];
    }



}
