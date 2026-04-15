#include "../include/MemorySteepLocalSearch.hpp"

#include <string>
#include <format>
#include <optional>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

string MemorySteepLocalSearch::getAlgorithmName()
{
    return format("Steep_{}", neighbourhoodUsed == MoveType::SwapNodes ? "SwapNodes" : "SwapEdges");
}

void MemorySteepLocalSearch::setMoveSet()
{
    moveSet.clear();
    int n = solution.size();

    // RemoveNode moves - usunięcie wierzchołka solution[node1]
    for (int i = 0; i < n; i++)
    {
        addMove<true>(MoveType::RemoveNode, i);
    }

    // InsertNode moves - wstawienie wierzchołka data[node1] po wierzchołku solution[node2]
    for (int node = 0; node < data->numNodes; node++)
    {
        if (inSolution[node] > -1)
            continue;

        if (n == 0)
        {
            addMove<true>(MoveType::InsertNode, node);
            continue;
        }

        for (int i = 0; i < n; i++)
        {
            addMove<true>(MoveType::InsertNode, node, i);
        }
    }

    // SwapNodes moves - zamiana wierzchołków solution[node1] i solution[node2] (node1 < node2)
    // or SwapEdges moves- zamiana krawędzi solution[node1] -> solution[node1 + 1] z krawędzią solution[node2] -> solution[node2 + 1] (node1 < node2)
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            addMove<true>(neighbourhoodUsed, i, j);
        }
    }
}

optional<Move> MemorySteepLocalSearch::chooseMove()
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

void MemorySteepLocalSearch::updateMoveSet(const Move &move)
{
    setMoveSet();
}
