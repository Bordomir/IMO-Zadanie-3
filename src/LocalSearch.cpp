#include "../include/LocalSearch.hpp"

#include <optional>
#include <vector>
#include <algorithm>
#include <print>
#include <filesystem>
#include <format>
#include <fstream>

using namespace std;
using namespace filesystem;

void LocalSearch::improve()
{
    solutionScore = calculateScore();
    setMoveSet();
    bool hasImproved = true;
    while (hasImproved)
    {
        hasImproved = false;

        optional<Move> bestMove = chooseMove();

        if (bestMove)
        {
            hasImproved = true;
            changeSolution(*bestMove);
            solutionScore += *bestMove->deltaScore;
            updateMoveSet(*bestMove);
        }
    }
}

int LocalSearch::calculateDeltaScore(const Move &move)
{
    int deltaScore = 0;
    switch (move.type)
    {
        case MoveType::InsertNode:
        {
            if(solution.size() == 0)
            {
                deltaScore += data->nodeProfits[move.node1];
                break;
            }
            int prev = solution[getNodeFromSolution(*move.node2)];
            int next = solution[getNodeFromSolution(*move.node2 + 1)];
            deltaScore += data->nodeProfits[move.node1];
            deltaScore += data->distanceMatrix[prev][next];
            deltaScore -= data->distanceMatrix[prev][move.node1];
            deltaScore -= data->distanceMatrix[move.node1][next];
            break;
        }
        case MoveType::RemoveNode:
        {
            int prev = solution[getNodeFromSolution(*move.node2 - 1)];
            int next = solution[getNodeFromSolution(*move.node2 + 1)];
            deltaScore -= data->nodeProfits[move.node1];
            deltaScore -= data->distanceMatrix[prev][next];
            deltaScore += data->distanceMatrix[prev][move.node1];
            deltaScore += data->distanceMatrix[move.node1][next];
            break;
        }
        case MoveType::SwapNodes:
        {
            if(solution.size() <= 3)
            {
                deltaScore = 0;
                break;
            }
            int prev1 = solution[getNodeFromSolution(move.node1 - 1)];
            int curr1 = solution[move.node1];
            int next1 = solution[getNodeFromSolution(move.node1 + 1)];

            int prev2 = solution[getNodeFromSolution(*move.node2 - 1)];
            int curr2 = solution[*move.node2];
            int next2 = solution[getNodeFromSolution(*move.node2 + 1)];

            if (curr1 == prev2)
            {
                deltaScore += data->distanceMatrix[prev1][curr1];

                deltaScore -= data->distanceMatrix[prev1][curr2];

                deltaScore += data->distanceMatrix[curr2][next2];

                deltaScore -= data->distanceMatrix[curr1][next2];
                break;
            }
            if(curr2 == prev1)
            {
                deltaScore += data->distanceMatrix[curr1][next1];

                deltaScore -= data->distanceMatrix[curr2][next1];

                deltaScore += data->distanceMatrix[prev2][curr2];

                deltaScore -= data->distanceMatrix[prev2][curr1];
                break;
            }

            deltaScore += data->distanceMatrix[prev1][curr1];
            deltaScore += data->distanceMatrix[curr1][next1];
            
            deltaScore -= data->distanceMatrix[prev1][curr2];
            deltaScore -= data->distanceMatrix[curr2][next1];

            deltaScore += data->distanceMatrix[prev2][curr2];
            deltaScore += data->distanceMatrix[curr2][next2];
            
            deltaScore -= data->distanceMatrix[prev2][curr1];
            deltaScore -= data->distanceMatrix[curr1][next2];

            break;
        }
        case MoveType::SwapEdges:
        {
            if (solution.size() <= 3)
            {
                deltaScore = 0;
                break;
            }
            int curr1 = solution[move.node1];
            int next1 = solution[getNodeFromSolution(move.node1 + 1)];

            int curr2 = solution[*move.node2];
            int next2 = solution[getNodeFromSolution(*move.node2 + 1)];

            if (next1 == curr2)
            {
                deltaScore = 0;
                break;
            }

            deltaScore += data->distanceMatrix[curr1][next1];

            deltaScore -= data->distanceMatrix[curr1][curr2];

            deltaScore += data->distanceMatrix[curr2][next2];

            deltaScore -= data->distanceMatrix[next1][next2];

            break;
        }
    }
    return deltaScore;
}

int LocalSearch::getNodeFromSolution(int solutionIndex)
{
    int n = solution.size();
    while(solutionIndex < 0) solutionIndex += n;
    while (solutionIndex >= n) 
    {
        solutionIndex -= n;
    }
    return solutionIndex;
}

void LocalSearch::changeSolution(const Move &bestMove)
{
    // TODO: Test this function, especially SwapEdges
    switch (bestMove.type)
    {
        case MoveType::InsertNode:
        {
            solution.insert(solution.begin() + *bestMove.node2 + 1, bestMove.node1);
            break;
        }
        case MoveType::RemoveNode:
        {
            solution.erase(solution.begin() + *bestMove.node2);
            break;
        }
        case MoveType::SwapNodes:
        {
            swap(solution[bestMove.node1], solution[*bestMove.node2]);
            break;
        }
        case MoveType::SwapEdges:
        {
            reverse(solution.begin() + bestMove.node1 + 1, solution.begin() + *bestMove.node2 + 1);
            break;
        }
    }
}
int LocalSearch::calculateLength()
{
    int score = 0;
    if (solution.size() == 0)
    {
        return score;
    }
    for (size_t currentNode = 1; currentNode < solution.size(); currentNode++)
    {
        // - odległość currentNode - 1 -> currentNode
        score -= data->distanceMatrix[solution[currentNode - 1]][solution[currentNode]];
    }
    if (solution.size() > 1)
    {
        // - odległość lastNode -> currentNode
        score -= data->distanceMatrix[solution[solution.size() - 1]][solution[0]];
    }
    return score;
}

int LocalSearch::calculateScore()
{
    int score = 0;
    if (solution.size() == 0)
    {
        return score;
    }
    // + zysk z odwiedzenia pierwszego wierzchołka
    score = data->nodeProfits[solution[0]];
    for (size_t currentNode = 1; currentNode < solution.size(); currentNode++)
    {
        // - odległość currentNode - 1 -> currentNode
        score -= data->distanceMatrix[solution[currentNode - 1]][solution[currentNode]];
        // + zysk z odwiedzenia wierzchołka currentNode
        score += data->nodeProfits[solution[currentNode]];
    }
    if (solution.size() > 1)
    {
        // - odległość lastNode -> currentNode
        score -= data->distanceMatrix[solution[solution.size() - 1]][solution[0]];
    }
    return score;
}
void LocalSearch::print()
{
    println("Trasa:\n{}", solution);
    println("Funkcja celu: {}", solutionScore);
}
void LocalSearch::saveToFile(const string &filename)
{
    path dir = "../data/solutions";
    create_directories(dir);
    string fullPath = (dir / format("{}_{}.txt", filename, getAlgorithmName())).string();
    ofstream file(fullPath);
    println(file, "{}", solutionScore);
    for (size_t i = 0; i < solution.size(); i++)
    {
        println(file, "{}", solution[i]);
    }
    file.close();
}