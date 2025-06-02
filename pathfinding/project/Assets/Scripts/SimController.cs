using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

// The main class managing the simulation.
// By Christina Pilip
public class SimController : MonoBehaviour
{
    // For the simulation
    [SerializeField] int numberOfNPCS;
    [SerializeField] GameObject npcPrefab;
    [SerializeField] OptAStarPathfinding aStar;
    static NavGrid grid;
    HashSet<NPC> npcs = new HashSet<NPC>();
    
    // NPCs waiting to be teleported
    static Dictionary<NPC, Node> teleportersUpBucket;
    static Dictionary<NPC, Node> teleportersDownBucket;

    // For cycling whether the teleporters move NPCs down to the lower level or up to the upper level
    float startTime;
    bool down = true;

    // For statistics
    public static int totalPaths;
    public static int repaths;
    public static int abandonedPaths;
    public static int pathfindingCalls;
    public static double totalAlgorithmTime;
    float simStartTime;
    public static float pathfindingCallStartTime;
    bool printOnce = true;

    void Awake()
    {
        // Initialize
        grid = GetComponent<NavGrid>();
        teleportersUpBucket = new Dictionary<NPC, Node>();
        teleportersDownBucket = new Dictionary<NPC, Node>();
        startTime = Time.time;
        simStartTime = Time.time;
        totalAlgorithmTime = 0;
    }

    private void Start()
    {
        // Spawn x number of NPCs on pathable nodes
        List<Node> spawnLocations = grid.GetNRandomNodes(true, numberOfNPCS);
        int index = 0;

        foreach (Node n in spawnLocations)
        {
            NPC npc = Instantiate(npcPrefab, n.worldPosition, Quaternion.identity).GetComponent<NPC>();
            NavGrid.PathableNodeNotAvailable(n);

            npc.manager = aStar;
            npc.text.text = "" + index;
            index++;

            npcs.Add(npc);
        }

        // Choose x targets for these NPCs
        List<Node> targets = grid.GetNRandomNodes(true, numberOfNPCS);

        index = 0;
        foreach (NPC npc in npcs)
        {
            npc.targetPosition = targets[index].worldPosition;
            index++;
        }
    }

    private void Update()
    {
        // Run the simulation
        foreach (NPC npc in npcs)
        {
            // If the NPC is not currently traversing a path
            if (npc.notOnPath)
            {
                pathfindingCallStartTime = Time.realtimeSinceStartup;
                npc.AttemptPath();
            }
        }
        
        // Cycle teleporters
        if (Time.time - startTime > 1f)
        {
            startTime = Time.time;
            cycleTeleporters();
            down = !down;
        }

        // Output statistics
        // Debug.Log(Time.time - simStartTime);
        if (Time.time - simStartTime > 120f)
        {
            if (printOnce)
            {
                Debug.Log("Total paths: " + totalPaths);
                Debug.Log("Total repaths: " + repaths);
                Debug.Log("Total abandonedPaths: " + abandonedPaths);
                Debug.Log("Total pathfindingCalls: " + pathfindingCalls);
                Debug.Log("Total totalAlgorithmTime: " + totalAlgorithmTime);
                printOnce = false;
            }
            
        }
        
    }

    /// <summary>
    /// Get a new target location from the navmesh.
    /// </summary>
    /// <returns>
    /// A vector containing the target position in world space.
    /// </returns>
    public static Vector3 NewTargetPosition()
    {
        return grid.GetNRandomNodes(true, 1)[0].worldPosition;
    }

    /// <summary>
    /// Activate the teleporters.
    /// </summary>
    private void cycleTeleporters()
    {
        Dictionary<NPC, Node> bucketToUse;
        if (down)
        {
            bucketToUse = teleportersDownBucket;
        }
        else
        {
            bucketToUse = teleportersUpBucket;
        }

        System.Random rand = new System.Random();
        int totalNPCs = Mathf.Min(bucketToUse.Count, 3);

        // Choose 3 or less NPCs that want to use the teleporter and teleport them
        for (int i = 0; i < totalNPCs; i++)
        {
            KeyValuePair<NPC, Node> g = bucketToUse.ElementAt(rand.Next(0, bucketToUse.Count));
            g.Key.inWaitingArea = false;
            g.Key.transform.position = g.Value.worldPosition;
            bucketToUse.Remove(g.Key);
        }
    }

    /// <summary>
    /// Add an NPC to the teleporter's waiting area.
    /// </summary>
    public static void AddToWaitingArea(Node from, Node to, NPC npc)
    {
        if (from.gridZ == 5)
        {
            teleportersDownBucket.Add(npc, to);
        }
        else
        {
            teleportersUpBucket.Add(npc, to);
        }
    }

}
