using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UIElements;

public class BasicNPCManage : MonoBehaviour
{
    //Overide functions
    protected virtual void RemoveCharacter(INPC npc)
    {

    }
    protected virtual void AddCharacter(INPC npc)
    {

    }
    protected virtual void doAwake()
    {

    }
    // Awake is called when the script instance is being loaded within Unity
    private void Awake()
    {
        DeathEventDispatcher death_eventDispatcher = FindObjectOfType<DeathEventDispatcher>();
        death_eventDispatcher.objectEvent.AddListener(RemoveCharacter);
        SpawnEventDispatcher spawn_eventDispatcher = FindObjectOfType<SpawnEventDispatcher>();
        spawn_eventDispatcher.objectEvent.AddListener(AddCharacter);
        doAwake();
    }
}
