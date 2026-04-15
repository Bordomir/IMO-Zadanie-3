#include "../include/CandidateSteepLocalSearch.hpp"

#include <string>
#include <format>
#include <optional>
#include <vector>
#include <numeric>
#include <algorithm>

using namespace std;

string CandidateSteepLocalSearch::getAlgorithmName()
{
    return format("Steep_{}", neighbourhoodUsed == MoveType::SwapNodes ? "SwapNodes" : "SwapEdges");
}

void CandidateSteepLocalSearch::setMoveSet()
{
    moveSet.clear();
    int n = solution.size();

    // RemoveNode moves - usunięcie wierzchołka solution[node1]
    for (int i = 0; i < n; i++)
    {
        const int prev = getNodeFromSolution(i - 1);
        const int next = getNodeFromSolution(i + 1);
        moveSet.emplace_back(MoveType::RemoveNode, vector{solution[i], solution[prev], solution[next]});
    }

    // InsertNode moves - wstawienie wierzchołka data[node1] po wierzchołku solution[node2]
    for (int node = 0; node < data->numNodes; node++)
    {
        if (inSolution[node] > -1)
            continue;

        if (n == 0)
        {
            moveSet.emplace_back(MoveType::InsertNode, vector{node});
            continue;
        }

        for (int i = 0; i < n; i++)
        {
            const int prev = getNodeFromSolution(i - 1);
            const int next = getNodeFromSolution(i + 1);
            moveSet.emplace_back(MoveType::InsertNode, vector{node, solution[prev], solution[next]});
        }
    }

    // SwapNodes moves - zamiana wierzchołków solution[node1] i solution[node2] (node1 < node2)
    // or SwapEdges moves- zamiana krawędzi solution[node1] -> solution[node1 + 1] z krawędzią solution[node2] -> solution[node2 + 1] (node1 < node2)
    for (int i = 0; i < n; i++)
    {
        for (int j = i + 1; j < n; j++)
        {
            const int p1 = solution[getNodeFromSolution(i - 1)];
            const int c1 = solution[i];
            const int n1 = solution[getNodeFromSolution(i + 1)];
            const int p2 = solution[getNodeFromSolution(j - 1)];
            const int c2 = solution[j];
            const int n2 = solution[getNodeFromSolution(j + 1)];
            if(neighbourhoodUsed == MoveType::SwapNodes)
                moveSet.emplace_back(MoveType::SwapNodes, vector{p1, c1, n1, p2, c2, n2});
            else
                moveSet.emplace_back(MoveType::SwapEdges, vector{c1, n1, c2, n2});
        }
    }
}

optional<Move> CandidateSteepLocalSearch::chooseMove()
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

void CandidateSteepLocalSearch::updateMoveSet(const Move &move)
{
    setMoveSet();
}
