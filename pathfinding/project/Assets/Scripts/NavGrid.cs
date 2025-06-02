using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

// Creates a navgrid - a 3D grid formed by a set of nodes.
// By Christina Pilip
public class NavGrid : MonoBehaviour
{
    // Nodes in the grid
    Node[,,] grid;
    [SerializeField] Vector3 gridWorldSize;

    // For editor
    [SerializeField] float nodeRadius;
    [SerializeField] LayerMask unpathable;
    [SerializeField] LayerMask unnavigable;
    [SerializeField] LayerMask blockObstacles;
    [SerializeField] LayerMask teleporter;

    public GameObject obstaclePrefab;

    // Grid dimensions
    float nodeSize;
    int gridSizeX, gridSizeY, gridSizeZ;
    public int gridSize;

    // Navigable = navmesh
    // Pathable = unblocked by obstacles or NPCs
    static HashSet<Node> navigableNodes = new HashSet<Node>();
    static HashSet<Node> pathableNodes = new HashSet<Node>();

    // Nodes where obstacles can be spawned
    static HashSet<Node> canSpawnObstaclesNodes = new HashSet<Node>(); 
    
    private void Awake()
    {
        nodeSize = 2 * nodeRadius;

        // Split grid world dimensions/size into clean discrete 3D grid of nodes
        gridSizeX = Mathf.RoundToInt(gridWorldSize.x / nodeSize);
        gridSizeY = Mathf.RoundToInt(gridWorldSize.y / nodeSize);
        gridSizeZ = Mathf.RoundToInt(gridWorldSize.z / nodeSize);
        gridSize = gridSizeX * gridSizeY * gridSizeZ;

        CreateGrid();
        ScatterObstacles();
    }

    /// <summary>
    /// Get a node from the navgrid based on a worldspace position.
    /// </summary>
    public Node NodeFromWorldPoint(Vector3 worldPosition)
    {
        float percentX = (worldPosition.x + gridWorldSize.x / 2) / gridWorldSize.x;
        float percentY = (worldPosition.z + gridWorldSize.y / 2) / gridWorldSize.y;
        float percentZ = (worldPosition.y + gridWorldSize.z / 2) / gridWorldSize.z;

        percentX = Mathf.Clamp01(percentX);
        percentY = Mathf.Clamp01(percentY);
        percentZ = Mathf.Clamp01(percentZ);

        int x = Mathf.RoundToInt((gridSizeX - 1) * percentX);
        int y = Mathf.RoundToInt((gridSizeY - 1) * percentY);
        int z = Mathf.RoundToInt((gridSizeZ - 1) * percentZ);

        return grid[x, y, z];
    }

    /// <summary>
    /// Get n random nodes from the navgrid - either pathable, or not.
    /// </summary>
    /// <returns>
    /// The list of nodes.
    /// </returns>
    public List<Node> GetNRandomNodes(bool mustBePathable, int n)
    {
        // Shuffle using Fisher–Yates
        System.Random rand = new System.Random();
        List<Node> shuffled = (mustBePathable) ? new List<Node>(pathableNodes) : new List<Node>(canSpawnObstaclesNodes);

        int count = shuffled.Count;
        while (count > 1)
        {
            count--;
            int k = rand.Next(count + 1);
            Node value = shuffled[k];
            shuffled[k] = shuffled[count];
            shuffled[count] = value;
        }

        // Take first n shuffled nodes
        return new List<Node>(shuffled.Take(n));
    }

    /// <summary>
    /// Make a node no longer pathable.
    /// </summary>
    public static void PathableNodeNotAvailable(Node n)
    {
        n.pathable = false;
        pathableNodes.Remove(n);
    }

    /// <summary>
    /// Make a node pathable.
    /// </summary>
    public static void PathableNodeAvailable(Node n)
    {
        n.pathable = true;
        pathableNodes.Add(n);
    }

    /// <summary>
    /// Scatter ten obstacles randomly on the navgrid.
    /// </summary>
    void ScatterObstacles()
    {
        List<Node> locations = GetNRandomNodes(false, 10);

        // Scatter obstacles centered on 10 nodes
        foreach (Node n in locations)
        {
            PathableNodeNotAvailable(n);
            n.canSpawnObstacle = false;
            
            System.Random rand = new System.Random();
            int length = rand.Next(2); //0 = 1-wide cube, 1 = 3-wide cube
          
            if (length == 1)
            {
                OrientObstacle(n);
            }
            else
            {
                Instantiate(obstaclePrefab, n.worldPosition, Quaternion.identity);
            }
        }

    }

    /// <summary>
    /// Determine if a obstacle can be placed at a node. 
    /// </summary>
    void OrientObstacle(Node n)
    {
        int left = n.gridX - 1, right = n.gridX + 1, down = n.gridY - 1, up = n.gridY + 1;
        
        // Do a bunch of checks on the left and right / up and down of the center location chosen for the long obstacle
        // If the long obstacle can be placed, do it - otherwise, just place a normal obstacle
        if (left > 0 && right < gridSizeX && CanExtendObstacle(grid[left, n.gridY, n.gridZ], grid[right, n.gridY, n.gridZ]))
        {
            PathableNodeNotAvailable(grid[left, n.gridY, n.gridZ]);
            grid[left, n.gridY, n.gridZ].canSpawnObstacle = false;

            PathableNodeNotAvailable(grid[right, n.gridY, n.gridZ]);
            grid[right, n.gridY, n.gridZ].canSpawnObstacle = false;

            Transform o = Instantiate(obstaclePrefab, n.worldPosition, Quaternion.identity).transform;
            o.localScale = new Vector3(2.25f, 1f, .75f);
        }
        else if (down > 0 && up < gridSizeY && CanExtendObstacle(grid[n.gridX, up, n.gridZ], grid[n.gridX, down, n.gridZ]))
        {
            PathableNodeNotAvailable(grid[n.gridX, up, n.gridZ]);
            grid[n.gridX, up, n.gridZ].canSpawnObstacle = false;

            PathableNodeNotAvailable(grid[n.gridX, down, n.gridZ]);
            grid[n.gridX, down, n.gridZ].canSpawnObstacle = false;

            Transform o = Instantiate(obstaclePrefab, n.worldPosition, Quaternion.identity).transform;
            o.localScale = new Vector3(.75f, 1f, 2.25f);
        }
        else
        {
            Instantiate(obstaclePrefab, n.worldPosition, Quaternion.identity);
        }
    }

    /// <summary>
    /// Check if an 1-wide obstacle can be extended to a 3-wide obstacle.
    /// </summary>
    /// <returns>
    /// Whether the obstacle can be extended.
    /// </returns>
    bool CanExtendObstacle(Node extensionLeft, Node extensionRight)
    {
        return 
            (extensionLeft.navigable && extensionLeft.canSpawnObstacle && extensionLeft.pathable) 
            &&
            (extensionRight.navigable && extensionRight.canSpawnObstacle && extensionRight.pathable);
    }

    /// <summary>
    /// Construct the navgrid.
    /// </summary>
    void CreateGrid()
    {
        grid = new Node[gridSizeX, gridSizeY, gridSizeZ];
        Vector3 worldCorner = transform.position - Vector3.right * gridWorldSize.x / 2 - Vector3.forward * gridWorldSize.y/2 - Vector3.up * gridWorldSize.z / 2;
        
        // Generate 3D grid
        for (int x = 0; x < gridSizeX; x++)
        {
            for (int y = 0; y < gridSizeY; y++)
            {
                for (int z = 0; z < gridSizeZ; z++)
                {
                    Vector3 worldpoint = worldCorner + Vector3.right * (x * nodeSize + nodeRadius) + Vector3.forward * (y * nodeSize + nodeRadius) + Vector3.up * (z * nodeSize + nodeRadius);
                   
                    // Check layers to determine spawned node's navigability, pathability, open to obstacles, and if colliding with teleporters
                    bool pathable = !Physics.CheckSphere(worldpoint, nodeRadius, unpathable);
                    bool navigable = Physics.CheckSphere(worldpoint, nodeRadius, unnavigable);
                    bool canSpawnObstacle = !Physics.CheckSphere(worldpoint, nodeRadius, blockObstacles);
                    bool isTeleporter = Physics.CheckSphere(worldpoint, nodeRadius, teleporter);
                    
                    grid[x, y, z] = new Node(pathable, navigable, canSpawnObstacle, isTeleporter, worldpoint, x, y, z);

                    // Track nodes with specific properties
                    if (navigable)
                    {
                        navigableNodes.Add(grid[x, y, z]);
                    }
                    if (canSpawnObstacle && navigable && pathable)
                    {
                        canSpawnObstaclesNodes.Add(grid[x, y, z]);
                    }
                    if (pathable && navigable)
                    {
                        pathableNodes.Add(grid[x, y, z]);
                    }
                }
            }
        }

        
        SetNavigableNeighbours();
    }

    /// <summary>
    /// Get the neighbours of a node.
    /// </returns>
    /// <summary>
    /// The set of neighbours.
    /// </returns>
    public HashSet<Node> GetNeighbours(Node node)
    {
        HashSet<Node> neighbours = new HashSet<Node>();

        //Get 3x3x3 grid of neighbours
        for (int x = -1; x <= 1; x++)
        {
            for (int y = -1; y <= 1; y++)
            {
                for (int z = -1; z <= 1; z++)
                {
                    if (x == 0 && y == 0 && z == 0)
                    {
                        continue;
                    }

                    int checkX = node.gridX + x;
                    int checkY = node.gridY + y;
                    int checkZ = node.gridZ + z;

                    if (checkX >= 0 && checkX < gridSizeX && checkY >= 0 && checkY < gridSizeY && checkZ >= 0 && checkZ < gridSizeZ)
                    {
                        //But only the navigable neighbours
                        if (grid[checkX, checkY, checkZ].navigable)
                        {
                            neighbours.Add(grid[checkX, checkY, checkZ]);
                        }
                    }
                }

            }
        }
        
        return neighbours;
    }

    /// <summary>
    /// Precompute the navigable neighbours of all nodes.
    /// </summary>
    void SetNavigableNeighbours()
    {
        foreach (Node n in navigableNodes)
        {
            // Precompute navigable neighbours of a node
            n.neighbours = GetNeighbours(n);

            // Add teleporter node in same column as a neighbour - for pathing purposes, this treats these as adjacent
            if (n.isTeleporter)
            {
                int z = (n.gridZ == 5) ? 0 : 5;               
                n.neighbours.Add(grid[n.gridX, n.gridY, z]);
            }
        }

    }


}
