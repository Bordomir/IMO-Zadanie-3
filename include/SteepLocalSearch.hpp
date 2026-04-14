#pragma once

#include <vector>
#include <string>
#include <memory>

#include "LocalSearch.hpp"
#include "DataLoader.hpp"
#include "Solver.hpp"

using namespace std;

class SteepLocalSearch : public LocalSearch
{
public:
    SteepLocalSearch(unique_ptr<Solver> &solver, MoveType neighbourhood) : LocalSearch(solver, neighbourhood) {};
    SteepLocalSearch(DataLoader &data, vector<int> solution, MoveType neighbourhood) : LocalSearch(data, solution, neighbourhood) {};
    string getAlgorithmName() override;
    void setMoveSet() override;
    optional<Move> chooseMove() override;
    void updateMoveSet(const Move &move) override;
};