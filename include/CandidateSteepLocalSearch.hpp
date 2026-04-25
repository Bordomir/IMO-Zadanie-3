#pragma once

#include <vector>
#include <string>
#include <memory>

#include "LocalSearch.hpp"
#include "DataLoader.hpp"
#include "Solver.hpp"


class CandidateSteepLocalSearch : public LocalSearch
{
public:
    CandidateSteepLocalSearch(std::unique_ptr<Solver>& solver, int k);

    std::string getAlgorithmName() override;

    void setMoveSet() override;
    std::optional<Move> chooseMove() override;
    void updateMoveSet(const Move& move) override;

private:
    std::vector<std::vector<int>> candidates_;
    int k_;

    void initializeCandidates();
    void generateCandidateMoves();
};
