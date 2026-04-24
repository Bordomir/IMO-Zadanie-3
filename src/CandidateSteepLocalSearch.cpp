#include "../include/CandidateSteepLocalSearch.hpp"

#include <string>
#include <format>
#include <optional>
#include <vector>
#include <numeric>
#include <algorithm>


CandidateSteepLocalSearch::CandidateSteepLocalSearch(std::unique_ptr<Solver>& solver, int k)
    : LocalSearch(solver, MoveType::SwapEdges)
    , k_(k)
{
    initializeCandidates();
}

std::string CandidateSteepLocalSearch::getAlgorithmName()
{
    return format("Candidate_Steep_{}_SwapEdges", std::to_string(k_));
}

void CandidateSteepLocalSearch::setMoveSet()
{
    initializeCandidates();
    generateCandidateMoves();
}

std::optional<Move> CandidateSteepLocalSearch::chooseMove()
{
    if (moveSet.empty())
    {
        return std::nullopt;
    }

    Move bestMove = moveSet[0];
    bestMove.deltaScore = -1;

    for (Move& move : moveSet)
    {
        move.deltaScore = calculateDeltaScore(move);

        if (move.deltaScore > bestMove.deltaScore)
        {
            bestMove = move;
        }
    }

    if (bestMove.deltaScore > 0)
    {
        return bestMove;
    }

    return std::nullopt;
}

void CandidateSteepLocalSearch::updateMoveSet([[maybe_unused]] const Move& move)
{
    generateCandidateMoves();
}

void CandidateSteepLocalSearch::initializeCandidates()
{
    candidates_ = std::vector<std::vector<int>>(data->numNodes, std::vector<int>());

    for (int i = 0; i < data->numNodes; ++i)
    {
        std::vector<std::pair<int, int>> nodeDistances;

        for (int j = 0; j < data->numNodes; ++j)
        {
            if (i != j)
            {
                nodeDistances.emplace_back(j, data->distanceMatrix[i][j]);
            }
        }

        int kCandidates = std::min(k_, static_cast<int>(nodeDistances.size()));

        std::partial_sort(
            nodeDistances.begin(),
            nodeDistances.begin() + kCandidates,
            nodeDistances.end(),
            [](const std::pair<int, int>& a, const std::pair<int, int>& b)
            {
                return a.second < b.second;
            });

        for (int j = 0; j < kCandidates; ++j)
        {
            candidates_[i].push_back(nodeDistances[j].first);
        }
    }
}

void CandidateSteepLocalSearch::generateCandidateMoves()
{
    moveSet.clear();
    int solutionSize = solution.size();

    for (int i = 0; i < solutionSize; i++)
    {
        addMove<true>(MoveType::RemoveNode, i);
    }

    for (int i = 0; i < solutionSize; i++)
    {
        int n1 = solution[i];
        for (int n2 : candidates_[n1])
        {
            int j = inSolution[n2];

            if (inSolution[n2] < 0)
            {
                addMove<true>(MoveType::InsertNode, n2, i);
                addMove<true>(MoveType::InsertNode, n2, (i - 1 + solutionSize) % solutionSize);
            }
            else
            {
                addMove<true>(neighbourhoodUsed, i, j);
            }
        }
    }
}
