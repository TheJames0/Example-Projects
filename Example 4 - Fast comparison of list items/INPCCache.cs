using FasterParallelFor;
using System.Collections.Generic;
using Unity.Mathematics;
using UnityEngine;
using System.Threading.Tasks;
using System;
using Unity.Collections;
using static UnityEngine.UI.Image;
using Unity.Jobs;
using Unity.Burst;

//Defines the two indexes to be compared in a struct for batching.
[BurstCompile]
public struct indexPair
    {
    public void set(int indx1, int indx2)
    {
        index1 = indx1;
        index2 = indx2;
    }
    public int index1;
    public int index2;
    }
//The class compares which player and npc's are visible to players/npc via raycasting.
public class INPCCache : BasicNPCManage
{
    public List<INPC> npcCache;
    public List<Transform> transformCache;
    public List<Vector3> positionCache;
    public List<AIVisionCone> aiVisionConeCache;
    List<Vector3> forwardVectorCache;
    public PlayerLogic Player;
    MultiThreadList<indexPair> raycastList;
    Task distanceJob;

    //Remove character if deleted from game
    protected override void RemoveCharacter(INPC npc)
    {
        transformCache.Remove(npc.transform_data);
        npcCache.Remove(npc);
    }

    //Add character if added to game
    protected override void AddCharacter(INPC npc)
    {
        transformCache.Add(npc.transform_data);
        npcCache.Add(npc);
        aiVisionConeCache.Add(npc.transform_data.GetComponent<AIVisionCone>());
    }
    //This is called on creation of this class object.
    protected override void doAwake()
    {
        npcCache = new List<INPC>
        {
             Player
        };
        Transform playerTransform = Player.transform;
        transformCache = new List<Transform>()
        {
            playerTransform
        };
        positionCache = new List<Vector3>();
        forwardVectorCache = new List<Vector3>();
        aiVisionConeCache = new List<AIVisionCone>()
        {
            Player.transform.GetComponent<AIVisionCone>()
        };
        raycastList = new MultiThreadList<indexPair>();
    }

    //Called every physics update
    private async void FixedUpdate()
    {
       for(int i = 0; i < transformCache.Count; i++)
        {
            //Add new Cache items
            if(i > positionCache.Count - 1)
            {
                positionCache.Add(transformCache[i].position + Vector3.up * 1.5f);
                forwardVectorCache.Add(transformCache[i].forward);
            }
            //Update previous Cache items
            else
            {
                positionCache[i] = transformCache[i].position + Vector3.up * 1.5f;
                forwardVectorCache[i] = transformCache[i].forward;
            }
        }
            raycastList.Clear();

        int length = npcCache.Count;
            int enumerations = (int)(length * (length - 1) / 2);
            if(distanceJob == null || distanceJob.IsCompleted) 
            await (distanceJob = Task.Run(() => preRequisiteCheck(enumerations))); // Prior distance check and angle check
            if(distanceJob.IsCompleted)
            batchRaycasts(raycastList); //Perform raycasts to items on the list
            
            
    }
    // Perform multithreaded distance checks between two characters in the character list. 
    //if within distance, check if within a vision angle and if so add to a list to perform raycast later.

    public async Task preRequisiteCheck(int enumerations)
    {

        FasterParallel.For(enumerations, i => {
            int indexA = (int)((-1 + math.sqrt(1 + 8 * i)) / 2) + 1;
            int indexB = (indexA != 1) ? i % indexA : 0;
            Vector3 start = positionCache[indexA];
            Vector3 end = positionCache[indexB];
            if ((end - start).sqrMagnitude < 100)
            {
                //Index A Spotting
                Vector3 directionToTarget = end - start;
                float angle = Vector3.Angle(forwardVectorCache[indexA], directionToTarget);

                // Define the acceptable angle range (in degrees)
                float acceptableAngle = 80;
                if (angle < acceptableAngle)
                {
                    //Inside view angle
                    indexPair listItem = new();
                    listItem.set(indexA, indexB);
                    raycastList.Add(listItem);
                }
                else
                {
                    //Outside view angle
                    aiVisionConeCache[indexA].UnSpot(npcCache[indexB]);
                }

                //Index B spotting
                Vector3 directionToTarget2 = start - end;
                float angle2 = Vector3.Angle(forwardVectorCache[indexB], directionToTarget2);
                if (angle2 < acceptableAngle)
                {
                    //Inside view angle
                    indexPair listItem = new();
                    listItem.set(indexB, indexA);
                    raycastList.Add(listItem);
                }
                else
                {
                    //Outside view angle
                    aiVisionConeCache[indexB].UnSpot(npcCache[indexA]);
                }
            }
            
            else if ((end - start).sqrMagnitude < 120)
            {
                aiVisionConeCache[indexA].UnSpot(npcCache[indexB]);
                aiVisionConeCache[indexB].UnSpot(npcCache[indexA]);
            }


        });
        await Task.Yield();
    }
    //Multithreaded function to call raycasts in a batch instead of individually.
    public void batchRaycasts(MultiThreadList<indexPair> raycastList)
    {
        int count = raycastList.Count();
        var results = new NativeArray<RaycastHit>(count, Allocator.TempJob);
        var commands = new NativeArray<RaycastCommand>(count, Allocator.TempJob);

        FasterParallel.For(count, i =>
        {
            Vector3 start = positionCache[raycastList.GetAtIndex(i).index1];
            Vector3 end = positionCache[raycastList.GetAtIndex(i).index2];
            Vector3 direction = (end - start).normalized;
            commands[i] = new RaycastCommand(start, direction, QueryParameters.Default);
        });
        JobHandle handle = RaycastCommand.ScheduleBatch(commands, results, 1, 1, default(JobHandle));
        // Wait for the batch processing job to complete
        handle.Complete();
        if (handle.IsCompleted)
        {
            for (int i = 0; i < results.Length; i++)
            {
                if (results[i].collider != null)
                {
                    if (results[i].transform.CompareTag("Character"))
                        aiVisionConeCache[raycastList.GetAtIndex(i).index1].Spot(npcCache[raycastList.GetAtIndex(i).index2]);
                }
            }
        
        results.Dispose();
        commands.Dispose();
        }
    }
    
    //get NPC cached position
    public Vector3 getPositionOfNPC(INPC npc)
    {
        int index = npcCache.IndexOf(npc);
        return positionCache[index];
    }

}
