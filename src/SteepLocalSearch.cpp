#include "../include/SteepLocalSearch.hpp"

#include <string>
#include <format>
#include <optional>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

string SteepLocalSearch::getAlgorithmName()
{
    return format("Steep_{}", neighbourhoodUsed == MoveType::SwapNodes ? "SwapNodes" : "SwapEdges");
}

void SteepLocalSearch::setMoveSet()
{
    moveSet.clear();
    int n = solution.size();
    vector<bool> inSolution(data->numNodes, false);

    // RemoveNode moves - usunięcie wierzchołka solution[node1]
    for (int i = 0; i < n; i++)
    {
        inSolution[solution[i]] = true;
        moveSet.emplace_back(MoveType::RemoveNode, solution[i], i);
    }

    // InsertNode moves - wstawienie wierzchołka data[node1] po wierzchołku solution[node2]
    for (int node1 = 0; node1 < data->numNodes; node1++)
    {
        if (inSolution[node1])
            continue;

        if (n == 0)
        {
            moveSet.emplace_back(MoveType::InsertNode, node1, -1);
            continue;
        }

        for (int node2 = 0; node2 < n; node2++)
        {
            moveSet.emplace_back(MoveType::InsertNode, node1, node2);
        }
    }

    // SwapNodes moves - zamiana wierzchołków solution[node1] i solution[node2] (node1 < node2)
    // or SwapEdges moves- zamiana krawędzi solution[node1] -> solution[node1 + 1] z krawędzią solution[node2] -> solution[node2 + 1] (node1 < node2)
    for (int node1 = 0; node1 < n; node1++)
    {
        for (int node2 = node1 + 1; node2 < n; node2++)
        {
            moveSet.emplace_back(neighbourhoodUsed, node1, node2);
        }
    }
}

optional<Move> SteepLocalSearch::chooseMove()
{
    if (moveSet.empty())
        return nullopt;

    Move bestMove = moveSet[0];
    bestMove.deltaScore = -1;
    for (Move &move : moveSet)
    {
        move.deltaScore = calculateDeltaScore(move);
        if (*move.deltaScore > *bestMove.deltaScore)
        {
            bestMove = move;
        }
    }
    if (*bestMove.deltaScore > 0)
    {
        return bestMove;
    }
    return nullopt;
}

void SteepLocalSearch::updateMoveSet(const Move &move)
{
    setMoveSet();
}
