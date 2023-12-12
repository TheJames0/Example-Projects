using FasterParallelFor;
using FasterParallelForInternals;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Unity.Burst;
using Unity.Collections;
using Unity.Jobs;
using Unity.Mathematics;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.AI;



//Removes objects outside of line of sight of player.
public class CompCullManager : doOnceOnUpdate
{
    //Cull sampler has a position to compare distance with the player and index values for objects to hide or unhide from players.
    [BurstCompile]
    public struct CullSampler
    {
        public Vector3 position;
        public MultiThreadList<int> cullList;
    }
    [SerializeField] Transform playerTransform;
    Vector3 playerPosition;
    ComponentCull[] cullArray;
    Vector3[] cullPositionArray;
    bool[] cullBoolArray;
    CullSampler[] CullSampler;
    JobHandle handle;
    Task distanceJob;

    //This method is called once on the first update loop to initialize the Cull Samplers
    //This involves raycasting to detect objects within its line of sight.
    public override void doUpdate()
    {
        GameObject[] taggedObjects = GameObject.FindGameObjectsWithTag("Tile");
        Vector3[] samplerPositions = new Vector3[taggedObjects.Length];
        for(int i = 0; i < taggedObjects.Length; i++)
        {
            samplerPositions[i] = taggedObjects[i].transform.position;
        }
        //Individual Cull scripts
        cullArray = FindObjectsOfType<ComponentCull>().ToArray();
        cullPositionArray = new Vector3[cullArray.Length];
        cullBoolArray = new bool[cullArray.Length];

        CullSampler = new CullSampler[taggedObjects.Length];

        for (int i = 0; i < cullArray.Length; i++)
        {
            cullPositionArray[i] = cullArray[i].transform.position;
            cullArray[i].enabled = false;
            cullArray[i].disableComps();
        }
        int count = 0;
        foreach (Vector3 pos in samplerPositions)
        {

                CullSampler cullSampler = new CullSampler()
                {
                    position = pos,
                    cullList = new MultiThreadList<int>(),
                };

            CullSampler[count] = cullSampler ;
            count++;
        }
        batchRaycasts(CullSampler.Length, cullArray.Length);

    }
    //Called on every physics update this method
    private async void FixedUpdate()
    {
        //Every frame assign threads to be completed which performs a distance check to each cull sampler, 
        //if the sampler is within a certain distance items visible to the sampler are enabled or else they are disabled.
        Array.Fill(cullBoolArray, false);
        playerPosition = playerTransform.position + Vector3.up * 1.5f;
        if (distanceJob == null || distanceJob.IsCompleted)
            await(distanceJob = Task.Run(() => assignThreads()));
        if (distanceJob.IsCompleted)
            setCullStates();
    }
    public void batchRaycasts(int samplers, int cullObjects)
    {
        int totalculls = 0;
        int count = samplers * cullObjects;
        var results = new NativeArray<RaycastHit>(count, Allocator.TempJob);
        var commands = new NativeArray<RaycastCommand>(count, Allocator.TempJob);

        FasterParallel.For(samplers, i =>
        {
            Vector3 start = CullSampler[i].position;
            for (int j = 0; j < cullObjects; j++)
            {
                Vector3 end = cullPositionArray[j];
                Vector3 direction = (end - start).normalized;
                commands[(i * cullObjects) + j] = new RaycastCommand(start, direction,QueryParameters.Default, (Vector3.Distance(start, end) * 0.9f));
            }
        });
        JobHandle handle = RaycastCommand.ScheduleBatch(commands, results, 1, 1, default(JobHandle));
        // Wait for the batch processing job to complete
        handle.Complete();
        if (handle.IsCompleted)
        {
            for (int i = 0; i < samplers; i++)
            {
                Vector3 start = CullSampler[i].position;
                for (int j = 0; j < cullObjects; j++)
                {
                    RaycastHit hit = results[(i * cullObjects) + j];
                    if (hit.collider == null)
                    {
                        CullSampler[i].cullList.Add(j);
                        totalculls++;
                    }
                }
            }
        }
        results.Dispose();
        commands.Dispose();
        Debug.Log(totalculls);
    }
    //Multithreaded task to perform distance checks to CullSampler.
    public async Task threadDistance(int start, int stop)
    {

        for (int i = start; i < stop; i++)
        {
            {
                Vector3 startvec = CullSampler[i].position;
                if ((playerPosition - startvec).sqrMagnitude < 30)
                {
                    int length = CullSampler[i].cullList.Count();
                    for (int j = 0; j < CullSampler[i].cullList.Count(); j++)
                    {
                        int cullItemIndex = CullSampler[i].cullList.GetAtIndex(j);
                        if (!cullBoolArray[cullItemIndex])
                            cullBoolArray[cullItemIndex] = true;
                    }
                }
            }
            await Task.Yield();
        }
    }
        void setCullStates()
        {
            for (int i = 0; i < cullArray.Length; i++)
            {
                if (cullBoolArray[i])
                {
                    cullArray[i].enableComps();
                }
                else
                {
                    cullArray[i].disableComps();
                }
            }
        }
        async void assignThreads()
        {
            int threads = 10;
            int enumerationsperThread = CullSampler.Length / threads;

            // do first threads with set work stride
            for (int i = 0; i < threads - 1; i++)
            {

                int indexstart = i * enumerationsperThread;
                int indexstop = indexstart + enumerationsperThread;

                await Task.Run(() => threadDistance(indexstart, indexstop));

            }
            // do last thread with the remaining stride
            int lastthreadStart = (threads - 1) * enumerationsperThread;
            int lastthreadEnd = cullArray.Length;

            await Task.Run(() => threadDistance(lastthreadStart, lastthreadEnd));
        }
    

 }
