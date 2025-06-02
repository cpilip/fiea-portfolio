using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

// A manager implementing the A* pathfinding algorithm.
// By Christina Pilip
public class OptAStarPathfinding : MonoBehaviour
{
    NavGrid grid;

    void Awake()
    {
        grid = GetComponent<NavGrid>();
    }

    /// <summary>
    /// Find a path between two world space positions.
    /// </summary>
    /// <returns>
    /// A list of nodes on the navgrid comprising the path. If no path could be found, return null.
    /// </returns>
    public List<Node> FindPath(Vector3 startPos, Vector3 targetPos)
    {
        Node startNode = grid.NodeFromWorldPoint(startPos);
        Node targetNode = grid.NodeFromWorldPoint(targetPos);

        if (!targetNode.navigable)
        {
            Debug.LogError("Not a navigable target.");
            return null;
        }
        if (!startNode.navigable)
        {
            Debug.LogError("Not a navigable start.");
            return null;
        }

        PriorityQueue<Node> openSet = new PriorityQueue<Node>(grid.gridSize);
        HashSet<Node> closedSet = new HashSet<Node>();

        openSet.Enqueue(startNode);

        while (openSet.Count() > 0)
        {
            Node currentNode = openSet.Dequeue();

            closedSet.Add(currentNode);

            if (currentNode == targetNode)
            {

                return ReconstructPath(startNode, targetNode);
            }

            foreach (Node neighbour in currentNode.neighbours)
            {

                if (!neighbour.pathable || closedSet.Contains(neighbour))
                {
                    continue;
                }

                int newScore = currentNode.gScore + GetDistance(currentNode, neighbour);
                
                if (newScore < neighbour.gScore || !openSet.Contains(neighbour))
                {
                    neighbour.gScore = newScore;
                    neighbour.hScore = GetDistance(neighbour, targetNode);
                    neighbour.parent = currentNode;

                    if (!openSet.Contains(neighbour))
                    {
                        openSet.Enqueue(neighbour);
                    }
                    else
                    {
                        openSet.UpdateItem(neighbour);
                    }
                }
            }

        }
        return null;

    }

    /// <summary>
    /// Reconstruct the full path between two nodes.
    /// </summary>
    /// <returns>
    /// A list of nodes comprising the full path.
    /// </returns>
    List<Node> ReconstructPath(Node startNode, Node endNode)
    {
        List<Node> path = new List<Node>();
        Node currentNode = endNode;

        while (currentNode != startNode)
        {
            path.Add(currentNode);
            currentNode = currentNode.parent;
        }

        path.Add(startNode);
        path.Reverse();

        return path;
    }

    /// <summary>
    /// Get the distance between two nodes.
    /// </summary>
    /// <returns>
    /// The distance.
    /// </returns>
    int GetDistance(Node a, Node b)
    {
        int distX = Mathf.Abs(a.gridX - b.gridX);
        int distY = Mathf.Abs(a.gridY - b.gridY);
        int distZ = Mathf.Abs(a.gridZ - b.gridZ);

        int distMin = Mathf.Min(distX, distY, distZ);
        int distMax = Mathf.Max(distX, distY, distZ);
        int distMid = distX + distY + distZ - distMin - distMax;

        // Octile distance for three dimensions, with a scalar of 10 units
        return (17 - 14) * distMin + (14 - 10) * distMid + 10 * distMax;
    }

}
