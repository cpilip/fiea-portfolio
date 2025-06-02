using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// A class defining an NPC agent.
// By Christina Pilip
public class NPC : MonoBehaviour
{
    // Color and label
    Color color;
    public TextMesh text;

    // Each NPC has a pathfinding manager initialized by the simulation controller
    public OptAStarPathfinding manager;
    public Vector3 targetPosition;
    int pathIndex;
    List<Node> path;
    public bool notOnPath;
    public float speed;
    public bool inWaitingArea = false;

    // After NPC finishes pathing, pause for some time
    float startTime;
    float pauseTime;
    System.Random pauseRand = new System.Random();

    private void Awake()
    {
        speed = 10f;

        // Label the NPCs
        color = UnityEngine.Random.ColorHSV(0f, 1f, 1f, 1f, 0.5f, 1f); 
        GetComponent<Renderer>().material.SetColor("_Color", color);
        
        // Pick an initial random pause time
        startTime = Time.time;
        pauseTime = (pauseRand.Next(700, 1000)) / 1000f;
        notOnPath = true;
    }

    /// <summary>
    /// Try to find a path to the target.
    /// </summary>
    public void AttemptPath()
    {
        // Find a path
        SimController.totalPaths++;
        SimController.pathfindingCalls++;
        
        path = manager.FindPath(this.transform.position, targetPosition);

        SimController.totalAlgorithmTime += (Time.realtimeSinceStartup - SimController.pathfindingCallStartTime);
        //Debug.Log(SimController.totalAlgorithmTime);

        // Pathfinding successful
        if (path != null)
        {
            notOnPath = false;
            StopCoroutine("FollowPath");
            StartCoroutine(FollowPath(AttemptPath));
        }

        // Pathfinding failed - pause and retarget
        if (path == null)
        {
            notOnPath = true;
            SimController.abandonedPaths++;
            Pause();
        }
        
    }

    /// <summary>
    /// Pause and wait before finding a new target.
    /// </summary>
    void Pause()
    {
        if (Time.time - startTime > pauseTime)
        {
            //Debug.LogError(text.text + " unpaused.");
            startTime = Time.time;
            pauseTime = (pauseRand.Next(700, 1000)) / 1000f;

            targetPosition = SimController.NewTargetPosition();
        }
    }

    /// <summary>
    /// Follow the current path.
    /// </summary>
    IEnumerator FollowPath(Action callback)
    {
        Node currentNode = path[0];
        currentNode.pathable = false;
        
        while (true)
        {
            // At current position, heading to next node in path
            if (transform.position == currentNode.worldPosition)
            {
                pathIndex++;

                // Done with path
                if (pathIndex >= path.Count)
                {
                    //Debug.Log(text.text + " reached end of path!");
                    pathIndex = 0;
                    path = null;
                    NavGrid.PathableNodeNotAvailable(currentNode);
                    notOnPath = true;
                    yield break;
                }


                Node prevNode = currentNode;
                currentNode = path[pathIndex];

                // Transitioning between teleporters - wait for cycling
                if (prevNode.isTeleporter && currentNode.isTeleporter && prevNode.gridZ != currentNode.gridZ)
                {
                    inWaitingArea = true;
                    SimController.AddToWaitingArea(prevNode, currentNode, this);
                    //Debug.Log(text.text + " wants to teleport.");
                }
                else
                {
                    // Blocked by another NPC (or obstacle, etc.)
                    if (!currentNode.pathable)
                    {
                        SimController.repaths++;
                        pathIndex = 0;
                        path = null;
                        NavGrid.PathableNodeNotAvailable(prevNode);

                        //Debug.Log(text.text + " blocked, reattempting path!");

                        callback();

                        yield break;
                    }

                    NavGrid.PathableNodeAvailable(prevNode);
                    NavGrid.PathableNodeNotAvailable(currentNode);
                }
            }

            // Update position over time so that the NPCs have smooth movement, but teleport instantly if using teleporter
            if (!inWaitingArea)
            {
                transform.position = Vector3.MoveTowards(transform.position, currentNode.worldPosition, speed * Time.deltaTime);
            }

            yield return null;
        }
    }

    /// <summary>
    /// Draw path in editor.
    /// </summary>
    private void OnDrawGizmos()
    {
        if (path != null)
        {
            foreach (Node n in path)
            {
                if (!n.pathable)
                {
                    Gizmos.color = Color.black;
                }
                Gizmos.color = color;
                Gizmos.DrawSphere(n.worldPosition, .25f);
            }
        }
    }

}
