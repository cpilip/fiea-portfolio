using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// A class defining a node for the navgrid.
// By Christina Pilip
public class Node
{
    // Properties
    public bool navigable;
    public bool pathable;
    public bool isTeleporter;
    public bool canSpawnObstacle;

    // Positions w.r.t both navgrid and world
    public Vector3 worldPosition;
    public int gridX;
    public int gridY;
    public int gridZ;

    // For pathfinding
    public int gScore;
    public int hScore;
    public int pqIndex;
    public Node parent;

    public HashSet<Node> neighbours;
    public Node(bool aPathable, bool aNavigable, bool aCanSpawnObstacle, bool aIsTeleporter, Vector3 aWorldPosition, int aGridX, int aGridY, int aGridZ)
    {
        pathable = aPathable;
        navigable = aNavigable;
        canSpawnObstacle = aCanSpawnObstacle;
        isTeleporter = aIsTeleporter;
        worldPosition = aWorldPosition;
        gridX = aGridX;
        gridY = aGridY;
        gridZ = aGridZ;
    }

    /// <summary>
    /// Get the cost of a node - f(x) = g(x) + h(x).
    /// </summary>
    /// <returns>
    /// The cost.
    /// </returns>
    public int fCost()
    {
        return gScore + hScore;
    }

    /// <returns>
    /// Compare whether this node would be "cheaper" to path to than another node. 
    /// </returns>
    /// <returns>
    /// Positive if this node is cheaper, negative if not.
    /// </returns>
    public int CompareTo(Node n)
    {
        int compare = fCost().CompareTo(n.fCost());

        if (compare == 0)
        {
            compare = hScore.CompareTo(n.hScore);
        }

        return -1*compare;
    }
}


