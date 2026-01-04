using System.Collections;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

public class EntityItem : MonoBehaviour
{
    public bool isThrow = false;
    private Vector3 throwDirection;
    private Rigidbody rb;
    private BoxCollider bc;

    private int id = 0;
    public int ID { get { return id; } }

    private float destructionTime = 30f; // ïb
    private float rotateSpeed = 1f;

    public GameObject meshParent;

    void Update()
    {
        if (isThrow && !IsMoving())
        {
            isThrow = false;
            rb.velocity = Vector3.zero;
            rb.angularVelocity = Vector3.zero;
        }

        Vector3 rot = transform.rotation.eulerAngles;
        rot.y += rotateSpeed;
        transform.rotation = Quaternion.Euler(rot);
    }

    public void ThrowIt(Vector3 direction, int sourceId)
    {
        rb = GetComponent<Rigidbody>();
        bc = GetComponent<BoxCollider>();

        isThrow = true;
        throwDirection = direction;
        id = sourceId;

        PhysicMaterial material = new PhysicMaterial();
        material.dynamicFriction = 1.0f;
        material.staticFriction = 1.0f;
        material.frictionCombine = PhysicMaterialCombine.Maximum;

        bc.material = material;

        rb.AddForce(throwDirection, ForceMode.Impulse);
        StartCoroutine(DestroyAfterTime(destructionTime));
    }

    public void ThrowIt(Vector3 direction, int sourceId, Texture texture)
    {
        rb = GetComponent<Rigidbody>();
        bc = GetComponent<BoxCollider>();

        List<GameObject> children = SupportFunc.GetChildren(meshParent);
        for (int i = 0; i < children.Count; i++)
        {
            children[i].GetComponent<MeshRenderer>().material.mainTexture = texture;
        }

        isThrow = true;
        throwDirection = direction;
        id = sourceId;

        PhysicMaterial material = new PhysicMaterial();
        material.dynamicFriction = 1.0f;
        material.staticFriction = 1.0f;
        material.frictionCombine = PhysicMaterialCombine.Maximum;

        bc.material = material;

        rb.AddForce(throwDirection, ForceMode.Impulse);
        StartCoroutine(DestroyAfterTime(destructionTime));
    }

    public void ThrowIt(Vector3 direction, int sourceId, List<Texture> texture)
    {
        rb = GetComponent<Rigidbody>();
        bc = GetComponent<BoxCollider>();

        List<GameObject> children = SupportFunc.GetChildren(meshParent);
        for (int i = 0; i < children.Count; i++)
        {
            children[i].GetComponent<MeshRenderer>().material.mainTexture = texture[i];
        }

        isThrow = true;
        throwDirection = direction;
        id = sourceId;

        PhysicMaterial material = new PhysicMaterial();
        material.dynamicFriction = 1.0f;
        material.staticFriction = 1.0f;
        material.frictionCombine = PhysicMaterialCombine.Maximum;

        bc.material = material;

        rb.AddForce(throwDirection, ForceMode.Impulse);
        StartCoroutine(DestroyAfterTime(destructionTime));
    }

    IEnumerator DestroyAfterTime(float time)
    {
        // éwíËÇµÇΩéûä‘ë“ã@
        yield return new WaitForSeconds(time);

        // é©êgÇDestroy
        Destroy(gameObject);
    }

    private bool IsMoving()
    {
        return rb.velocity != Vector3.zero;
    }
}
