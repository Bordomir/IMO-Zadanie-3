#pragma once

#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <unordered_map>

#include "LocalSearch.hpp"
#include "DataLoader.hpp"
#include "Solver.hpp"

using namespace std;

class MemorySteepLocalSearch : public LocalSearch
{
public:
    priority_queue<Move> moveSetQueue;
    // unordered_map<MoveType, unordered_map<int, unordered_map<int, int>>> moveSetMap;

    MemorySteepLocalSearch(unique_ptr<Solver> &solver, MoveType neighbourhood = MoveType::SwapEdges) : LocalSearch(solver, neighbourhood) {};
    MemorySteepLocalSearch(DataLoader &data, vector<int> solution, MoveType neighbourhood = MoveType::SwapEdges) : LocalSearch(data, solution, neighbourhood) {};
    string getAlgorithmName() override;
    void setMoveSet() override;
    optional<Move> chooseMove() override;
    void updateMoveSet(const Move &move) override;
    template<bool areIndicesGiven = true>
    void addImprovingMove(MoveType type, int index1, int index2 = -1);
    void extendQueue();
};


template<bool areIndicesGiven>
void MemorySteepLocalSearch::addImprovingMove(MoveType type, int index1, int index2)
{
    Move m = createMove<areIndicesGiven>(type, index1, index2);
    m.deltaScore = calculateDeltaScore(m);
    if(*m.deltaScore > 0)
    {
        moveSet.push_back(move(m));
    }
    if(type == MoveType::SwapEdges)
    {
        const int solutionIndex1 = areIndicesGiven ? index1 : inSolution[index1];
        const int solutionIndex2 = areIndicesGiven ? index2 : inSolution[index2];
        const int c1 = areIndicesGiven ? solution[index1] : index1;
        const int n2 = areIndicesGiven ? solution[index2] : index2;
        const int n1 = solution[getNodeFromSolution(solutionIndex1 + 1)];
        const int c2 = solution[getNodeFromSolution(solutionIndex2 + 1)];
        Move m2{MoveType::SwapEdges, {c1, n1, c2, n2}};
        m2.deltaScore = calculateDeltaScore(m2);
        if(*m2.deltaScore > 0)
        {
            moveSet.push_back(move(m2));
        }
    }
}