#include "../include/MemorySteepLocalSearch.hpp"

#include <string>
#include <format>
#include <optional>
#include <vector>
#include <numeric>
#include <algorithm>
#include <queue>

using namespace std;

string MemorySteepLocalSearch::getAlgorithmName()
{
    return format("Memory_Steep_{}", neighbourhoodUsed == MoveType::SwapNodes ? "SwapNodes" : "SwapEdges");
}

void MemorySteepLocalSearch::setMoveSet()
{
    moveSet.clear();
    moveSetQueue = priority_queue<int, vector<int>, MoveIndexComparator>(MoveIndexComparator{&moveSet});
    
    int n = solution.size();

    for (int i = 0; i < n; i++)
    {
        addImprovingMove<true>(MoveType::RemoveNode, i);
    }

    for (int node = 0; node < data->numNodes; node++)
    {
        if (inSolution[node] > -1)
            continue;

        if (n == 0)
        {
            addImprovingMove<true>(MoveType::InsertNode, node);
            continue;
        }

        for (int i = 0; i < n; i++)
        {
            addImprovingMove<true>(MoveType::InsertNode, node, i);
        }
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            addImprovingMove<true>(neighbourhoodUsed, i, j);
        }
    }

    removedMoves = vector<int>();
    validMoves = vector<int>();
}

optional<Move> MemorySteepLocalSearch::chooseMove()
{
    while(!moveSetQueue.empty())
    {
        int bestIndex = moveSetQueue.top();
        moveSetQueue.pop();

        Move &m = moveSet[bestIndex];

        int res = isMoveApplicable(m);
        if(res == -1)
        {
            validMoves.push_back(bestIndex);
        } else
        {
            removedMoves.push_back(bestIndex);
        }

        if(res == 1) return m;
    }
    return nullopt;
}

void MemorySteepLocalSearch::updateMoveSet(const Move &move)
{
    while(!validMoves.empty())
    {
        int index = validMoves.back();
        validMoves.pop_back();
        moveSetQueue.push(index);
    }
    switch (move.type)
    {
        case MoveType::InsertNode:
        {
            auto [node, prev, next, u1, u2, u3] = move.nodes;

            // InsertNode moves
            int nodeIndex = inSolution[node];
            int prevIndex = getNodeFromSolution(nodeIndex - 1);
            for (int i = 0; i < data->numNodes; i++)
            {
                if (inSolution[i] > -1) continue;

                addImprovingMove<true>(MoveType::InsertNode, i, nodeIndex);
                addImprovingMove<true>(MoveType::InsertNode, i, prevIndex);
            }

            // RemoveNode moves
            addImprovingMove<false>(MoveType::RemoveNode, node);
            addImprovingMove<false>(MoveType::RemoveNode, prev);
            addImprovingMove<false>(MoveType::RemoveNode, next);

            // SwapEdges moves
            addImprovingMove<true>(MoveType::SwapEdges, prevIndex, nodeIndex);
            int n = solution.size();
            for (int i = 0; i < n; i++)
            {
                if(i == nodeIndex || i == prevIndex) continue;
                addImprovingMove<true>(MoveType::SwapEdges, i, nodeIndex);
                addImprovingMove<true>(MoveType::SwapEdges, i, prevIndex);
            }

            break;
        }
        case MoveType::RemoveNode:
        {
            auto [node, prev, next, u1, u2, u3] = move.nodes;

            if(solution.empty())
            {
                for (int i = 0; i < data->numNodes; i++)
                {
                    addImprovingMove<true>(MoveType::InsertNode, i);
                }
                break;
            }

            // InsertNode moves
            int direction = checkEdge(prev, next);
            int prevIndex = direction > 0 ? inSolution[prev] : inSolution[next];
            int nextIndex = direction > 0 ? inSolution[next] : inSolution[prev];
            for (int i = 0; i < data->numNodes; i++)
            {
                if (inSolution[i] > -1) continue;

                addImprovingMove<true>(MoveType::InsertNode, i, prevIndex);
            }
            int n = solution.size();
            for (int i = 0; i < n; i++)
            {
                addImprovingMove<true>(MoveType::InsertNode, node, i);
            }

            // RemoveNode moves
            addImprovingMove<true>(MoveType::RemoveNode, prevIndex);
            addImprovingMove<true>(MoveType::RemoveNode, nextIndex);

            // SwapEdges moves
            for (int i = 0; i < n; i++)
            {
                if(i == prevIndex) continue;
                addImprovingMove<true>(MoveType::SwapEdges, i, prevIndex);
            }

            break;
        }
        case MoveType::SwapNodes:
        {
            // auto [p1, c1, n1, p2, c2, n2] = move.nodes;
            // Not used in this local search
            break;
        }
        case MoveType::SwapEdges:
        {
            auto [c1, n1, c2, n2, u1, u2] = move.nodes;

            // InsertNode moves
            int node1Index = inSolution[c1];
            int node2Index = inSolution[n1];
            for (int i = 0; i < data->numNodes; i++)
            {
                if (inSolution[i] > -1) continue;

                addImprovingMove<true>(MoveType::InsertNode, i, node1Index);
                addImprovingMove<true>(MoveType::InsertNode, i, node2Index);
            }

            // RemoveNode moves
            addImprovingMove<true>(MoveType::RemoveNode, node1Index);
            addImprovingMove<true>(MoveType::RemoveNode, node2Index);
            addImprovingMove<false>(MoveType::RemoveNode, c2);
            addImprovingMove<false>(MoveType::RemoveNode, n2);

            // SwapEdges moves
            addImprovingMove<true>(MoveType::SwapEdges, node1Index, node2Index);
            int n = solution.size();
            for (int i = 0; i < n; i++)
            {
                if(i == node1Index || i == node2Index) continue;
                addImprovingMove<true>(MoveType::SwapEdges, i, node1Index);
                addImprovingMove<true>(MoveType::SwapEdges, i, node2Index);
            }

            break;
        }
    }
}

