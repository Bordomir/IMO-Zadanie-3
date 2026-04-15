#pragma once

#include <vector>
#include <string>
#include <memory>

#include "LocalSearch.hpp"
#include "DataLoader.hpp"
#include "Solver.hpp"

using namespace std;

class CandidateSteepLocalSearch : public LocalSearch
{
public:
    CandidateSteepLocalSearch(unique_ptr<Solver> &solver, MoveType neighbourhood = MoveType::SwapEdges) : LocalSearch(solver, neighbourhood) {};
    CandidateSteepLocalSearch(DataLoader &data, vector<int> solution, MoveType neighbourhood = MoveType::SwapEdges) : LocalSearch(data, solution, neighbourhood) {};
    string getAlgorithmName() override;
    void setMoveSet() override;
    optional<Move> chooseMove() override;
    void updateMoveSet(const Move &move) override;
};