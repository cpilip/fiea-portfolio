using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

// An abridged priority queue backed by a min-heap.
// By Christina Pilip
public class PriorityQueue<T> where T : Node
{ 
    private Node[] nodes;
    public int indexCount;

    public PriorityQueue(int size)
    {
        nodes = new Node[size];
    }

    /// <summary>
    /// Add a node to the queue.
    /// </summary>
    public void Enqueue(Node n)
    {
        n.pqIndex = indexCount;
        nodes[indexCount] = n;

        MoveUp(n);
        indexCount++;
    }

    /// <summary>
    /// Get the node with the lowest priority/cost.
    /// </summary>
    /// <returns>
    /// The node.
    /// </returns>
    public Node Dequeue()
    {
        Node first = nodes[0];
        indexCount--;

        
        nodes[0] = nodes[indexCount];
        nodes[0].pqIndex = 0;

        MoveDown(nodes[0]);
        
        return first;
    }

    /// <summary>
    /// Increase a node's priority in the queue.
    /// </summary>
    public void MoveUp(Node n)
    {
        int parentIndex = (n.pqIndex - 1) / 2;

        while (true)
        {
            Node parentNode = nodes[parentIndex];
            if (n.CompareTo(parentNode) > 0)
            {
                Swap(n, parentNode);
            }
            else
            {
                break;
            }

            parentIndex = (n.pqIndex - 1) / 2;
        }
    }

    /// <summary>
    /// Decrease a node's priority in the queue.
    /// </summary>
    public void MoveDown(Node n)
    {
        while (true)
        {
            int childIndexLeft = n.pqIndex * 2 + 1;
            int childIndexRight = n.pqIndex * 2 + 2;

            int toSwapIndex = 0;

            if (childIndexLeft < indexCount)
            {
                toSwapIndex = childIndexLeft;

                if (childIndexRight < indexCount)
                {
                    if (nodes[childIndexLeft].CompareTo(nodes[childIndexRight]) < 0)
                    {
                        toSwapIndex = childIndexRight;
                    }
                }

                if (n.CompareTo(nodes[toSwapIndex]) < 0)
                {
                    Swap(n, nodes[toSwapIndex]);
                } 
                else
                {
                    return;
                }
            }
            else
            {
                return;
            }

        }
    }

    public int Count()
    {
        return indexCount;
    }

    /// <summary>
    /// Check if the queue contains a node.
    /// </summary>
    /// <returns>
    /// True if so, false if not.
    /// </returns>
    public bool Contains(Node n)
    {
        return Equals(nodes[n.pqIndex], n);
    }

    public void UpdateItem(Node n)
    {
        MoveUp(n);
    }

    void Swap(Node a, Node b)
    {
        nodes[a.pqIndex] = b;
        nodes[b.pqIndex] = a;
        int itemAIndex = a.pqIndex;
        a.pqIndex = b.pqIndex;
        b.pqIndex = itemAIndex;
    }

} 


