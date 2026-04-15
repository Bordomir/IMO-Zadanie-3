#pragma once

#include <optional>
#include <print>
#include <vector>
#include <string>
#include <memory>
#include <limits>

#include "DataLoader.hpp"
#include "Solver.hpp"

using namespace std;

enum class MoveType
{
    InsertNode = 0, // wstawienie wierzchołka data[node1] po wierzchołku solution[node2]
    RemoveNode = 1, // usunięcie wierzchołka data[node1] z miejsca node2 = solution[data[node1]]
    SwapNodes = 2, // zamiana wierzchołków solution[node1] i solution[node2] (node1 < node2)
    SwapEdges = 3 // zamiana krawędzi solution[node1] -> solution[node1 + 1] z krawędzią solution[node2] -> solution[node2 + 1] (node1 < node2)
    // Zmiana w interpretacji
    // InsertNode - (node, prev, next = nodes) wstawienie wierzchołka node pomiędzy wierzchołki prev i next lub na sam początek jeżeli prev < 0 lub node == prev
    // RemoveNode - (node, prev, next = nodes) usunięcie wierzchołka node który był pomiędzy wierzchołkami prev i next lub na samym początku jeżeli prev < 0 lub node == prev
    // SwapNodes - (p1, c1, n1, p2, c2, n2 = nodes) zamiana wierchołków c1 i c2 w otoczeniach p1, n1 i p2, n2
    // SwapEdges - (c1, n1, c2, n2 = nodes) zamiana krawędzi c1 -> n1 z krawędzią c2 -> n2
};

struct Move 
{
    MoveType type;
    array<int, 6> nodes;
    optional<int> deltaScore;
    
    // int node1;
    // optional<int> node2;
    // optional<int> deltaScore;
    // optional<int> succNode1; // = solution[node1 + 1]
    // optional<int> succNode2; // = solution[node2 + 1]

    // Move(MoveType type, int node1, optional<int> node2 = nullopt, optional<int> deltaScore = nullopt, optional<int> succNode1 = nullopt, optional<int> succNode2 = nullopt) : type(type), node1(node1), node2(node2), deltaScore(deltaScore), succNode1(succNode1), succNode2(succNode2) {}
    Move(MoveType type, vector<int> &&nodes, optional<int> deltaScore = nullopt) : type(type), nodes(), deltaScore(deltaScore) 
    {
        this->nodes.fill(-1);
        for(size_t i = 0; i < nodes.size() && i < 6; i++)
            this->nodes[i] = nodes[i];
    }

    bool operator<(const Move &other) const
    {
        return deltaScore.value_or(numeric_limits<int>::min()) < other.deltaScore.value_or(numeric_limits<int>::min());
    }

void print() const
{
    // Używamy printf lub cout dla maksymalnej kompatybilności
    println("type: {:<1} | nodes: [{:<3}, {:<3}, {:<3}, {:<3}, {:<3}, {:<3}] | delta: {:<6}",
        static_cast<int>(type),
        nodes[0], 
        nodes[1], 
        nodes[2], 
        nodes[3], 
        nodes[4], 
        nodes[5],
        deltaScore.value_or(0)
    );
}
};

class LocalSearch
{
public:
    DataLoader *data;
    vector<int> solution;
    int solutionScore;
    MoveType neighbourhoodUsed;
    vector<int> inSolution;
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
    optional<Move> createMove(MoveType type, int node1, int node2 = -1);
    virtual optional<Move> chooseMove() = 0;
    void changeSolution(const Move &move);
    virtual void setMoveSet() = 0;
    virtual void updateMoveSet(const Move &move) = 0;

    int calculateLength();
    int calculateScore();
    void print();
    void saveToFile(const string &filename = "");
};