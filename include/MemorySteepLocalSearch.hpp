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
    unordered_map<MoveType, unordered_map<int, unordered_map<int, int>>> moveSetMap;

    MemorySteepLocalSearch(unique_ptr<Solver> &solver, MoveType neighbourhood = MoveType::SwapEdges) : LocalSearch(solver, neighbourhood) {};
    MemorySteepLocalSearch(DataLoader &data, vector<int> solution, MoveType neighbourhood = MoveType::SwapEdges) : LocalSearch(data, solution, neighbourhood) {};
    string getAlgorithmName() override;
    void setMoveSet() override;
    optional<Move> chooseMove() override;
    void updateMoveSet(const Move &move) override;
};