#pragma once

#include <optional>
#include <print>
#include <vector>
#include <string>
#include <memory>

#include "DataLoader.hpp"
#include "Solver.hpp"

using namespace std;

enum class MoveType
{
    InsertNode = 0, // wstawienie wierzchołka data[node1] po wierzchołku solution[node2]
    RemoveNode = 1, // usunięcie wierzchołka data[node1] z miejsca node2 = solution[data[node1]]
    SwapNodes = 2, // zamiana wierzchołków solution[node1] i solution[node2] (node1 < node2)
    SwapEdges = 3 // zamiana krawędzi solution[node1] -> solution[node1 + 1] z krawędzią solution[node2] -> solution[node2 + 1] (node1 < node2)
};

struct Move 
{
    MoveType type;
    int node1;
    optional<int> node2;
    optional<int> deltaScore;

    Move(MoveType type, int node1, optional<int> node2 = nullopt, optional<int> deltaScore = nullopt) : type(type), node1(node1), node2(node2), deltaScore(deltaScore) {}
    void print() const
    {
        println("type: {:<11} | node1: {:<3} | node2: {:<3}",
            static_cast<int>(type), node1, *node2);
    }
};

class LocalSearch
{
public:
    DataLoader *data;
    vector<int> solution;
    int solutionScore;
    MoveType neighbourhoodUsed;
    vector<Move> moveSet;

    LocalSearch(unique_ptr<Solver> &solver, MoveType neighbourhood) : 
        data(solver->data), \
        solution(solver->solution),
        neighbourhoodUsed(neighbourhood)
    {};
    LocalSearch(DataLoader &data, vector<int> solution, MoveType neighbourhood) : 
        data(&data), 
        solution(solution),
        neighbourhoodUsed(neighbourhood) 
    {};
    virtual string getAlgorithmName() = 0;
    void improve();
    int calculateDeltaScore(const Move &move);
    int getNodeFromSolution(int solutionIndex);
    virtual optional<Move> chooseMove() = 0;
    void changeSolution(const Move &move);
    virtual void setMoveSet() = 0;
    virtual void updateMoveSet(const Move &move) = 0;

    int calculateLength();
    int calculateScore();
    void print();
    void saveToFile(const string &filename = "");
};