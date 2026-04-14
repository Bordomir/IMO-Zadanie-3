#include <string>
#include <vector>
#include <memory>
#include <print>
#include <random>
#include <limits>
#include <ranges>
#include <algorithm>
#include <utility>
#include <chrono>

#include "../include/DataLoader.hpp"
#include "../include/Solver.hpp"
#include "../include/RandomSolver.hpp"
#include "../include/KRegret.hpp"
#include "../include/LocalSearch.hpp"
// #include "../include/RandomLocalSearch.hpp"
// #include "../include/GreedyLocalSearch.hpp"
#include "../include/SteepLocalSearch.hpp"

using namespace std;

struct Statistic
{
    string data;
    string solver;
    string localSearch;
    double average = 0;
    double min = numeric_limits<double>::max();
    double max = numeric_limits<double>::min();

    Statistic(string data, string solver, string localSearch) : data(move(data)), solver(move(solver)), localSearch(move(localSearch)) {};
    void update(double value)
    {
        average += value;
        min = std::min(min, value);
        max = std::max(max, value);
    }
    void print() const
    {
        std::println("{};{};{};{:.4f};{:.4f};{:.4f}", data, solver, localSearch, average, min, max);
    }
};

int main()
{
    DataLoader dataA("../data/TSPA.csv", "DataA");
    DataLoader dataB("../data/TSPB.csv", "DataB");

    int startNode = 0;

    vector<unique_ptr<Solver>> solvers;
    solvers.reserve(4);
    solvers.emplace_back(make_unique<RandomSolver>(dataA));
    solvers.emplace_back(make_unique<RandomSolver>(dataB));
    solvers.emplace_back(make_unique<KRegret>(dataA, startNode, 2));
    solvers.emplace_back(make_unique<KRegret>(dataB, startNode, 2));


    vector<unique_ptr<LocalSearch>> localSearches;
    localSearches.reserve(4);
    // localSearches.emplace_back(make_unique<GreedyLocalSearch>(solvers[0], MoveType::SwapNodes));
    // localSearches.emplace_back(make_unique<GreedyLocalSearch>(solvers[0], MoveType::SwapEdges));
    localSearches.emplace_back(make_unique<SteepLocalSearch>(solvers[0], MoveType::SwapNodes));
    localSearches.emplace_back(make_unique<SteepLocalSearch>(solvers[0], MoveType::SwapEdges));

    // Experiment
    mt19937 rng{random_device{}()};
    int numRuns = 100;
    int maxNode = min(dataA.numNodes, dataB.numNodes);
    int maxTestsPossible = min(numRuns, maxNode);

    std::println("Running {} tests", maxTestsPossible);

    auto allNodesView = views::iota(0, maxNode);

    vector<int> startingNodes(maxTestsPossible);
    ranges::sample(allNodesView, startingNodes.begin(), maxTestsPossible, rng);

    vector<Statistic> scoreStatistics;
    scoreStatistics.reserve(solvers.size() * localSearches.size());
    vector<Statistic> timeStatistics;
    timeStatistics.reserve(solvers.size() * localSearches.size());
    vector<Statistic> scoreStatisticsForSolver;
    scoreStatisticsForSolver.reserve(solvers.size());
    vector<Statistic> timeStatisticsForSolver;
    timeStatisticsForSolver.reserve(solvers.size());
    for (auto &solver : solvers)
    {
        scoreStatisticsForSolver.emplace_back(
            solver->data->getName(),
            solver->getAlgorithmName(),
            "None");
        timeStatisticsForSolver.emplace_back(
            solver->data->getName(),
            solver->getAlgorithmName(),
            "None");
        for (auto &localSearch : localSearches)
        {
            scoreStatistics.emplace_back(
                solver->data->getName(),
                solver->getAlgorithmName(),
                localSearch->getAlgorithmName());
            timeStatistics.emplace_back(
                solver->data->getName(),
                solver->getAlgorithmName(),
                localSearch->getAlgorithmName());
        }
    }

    chrono::time_point<chrono::high_resolution_clock> startTime, endTime;
    for (int startNode : startingNodes)
    {
        for (size_t i = 0; i < solvers.size(); i++)
        {
            const auto &solver = solvers[i];

            solver->startNode = startNode;

            startTime = chrono::high_resolution_clock::now();
            solver->solve();
            endTime = chrono::high_resolution_clock::now();

            if (solver->solutionScore > scoreStatisticsForSolver[i].max)
            {
                solver->saveToFile(solver->data->getName());
            }

            scoreStatisticsForSolver[i].update(solver->solutionScore);
            timeStatisticsForSolver[i].update(chrono::duration<double, std::milli>(endTime - startTime).count());

            for (size_t j = 0; j < localSearches.size(); j++)
            {
                const auto &localSearch = localSearches[j];

                localSearch->data = solver->data;
                localSearch->solution = solver->solution;

                startTime = chrono::high_resolution_clock::now();
                localSearch->improve();
                endTime = chrono::high_resolution_clock::now();

                int index = i * localSearches.size() + j;
                if (localSearch->solutionScore > scoreStatistics[index].max)
                {
                    localSearch->saveToFile(format("{}_{}", solver->data->getName(), solver->getAlgorithmName()));
                }

                scoreStatistics[index].update(localSearch->solutionScore);
                timeStatistics[index].update(chrono::duration<double, std::milli>(endTime - startTime).count());
            }
        }
    }
    for (auto &stat : scoreStatisticsForSolver)
        stat.average /= maxTestsPossible;
    for (auto &stat : timeStatisticsForSolver)
        stat.average /= maxTestsPossible;
    for (auto &stat : scoreStatistics)
        stat.average /= maxTestsPossible;
    double randomTimeLimit = numeric_limits<double>::min();
    for (auto &stat : timeStatistics)
    {
        stat.average /= maxTestsPossible;
        randomTimeLimit = max(randomTimeLimit, stat.average);
    }
    
    // vector<unique_ptr<RandomLocalSearch>> randomLocalSearches;
    // randomLocalSearches.reserve(2);
    // randomLocalSearches.emplace_back(make_unique<RandomLocalSearch>(solvers[0], MoveType::SwapNodes, randomTimeLimit));
    // randomLocalSearches.emplace_back(make_unique<RandomLocalSearch>(solvers[0], MoveType::SwapEdges, randomTimeLimit));

    // vector<Statistic> scoreStatisticsForRandomLocalSearch;
    // scoreStatisticsForRandomLocalSearch.reserve(solvers.size() * randomLocalSearches.size());
    // vector<Statistic> timeStatisticsForRandomLocalSearch;
    // timeStatisticsForRandomLocalSearch.reserve(solvers.size() * randomLocalSearches.size());
    // for (auto &solver : solvers)
    // {
    //     for (auto &localSearch : randomLocalSearches)
    //     {
    //         scoreStatisticsForRandomLocalSearch.emplace_back(
    //             solver->data->getName(),
    //             solver->getAlgorithmName(),
    //             localSearch->getAlgorithmName());
    //         timeStatisticsForRandomLocalSearch.emplace_back(
    //             solver->data->getName(),
    //             solver->getAlgorithmName(),
    //             localSearch->getAlgorithmName());
    //     }
    // }
    // for (int startNode : startingNodes)
    // {
    //     for (size_t i = 0; i < solvers.size(); i++)
    //     {
    //         const auto &solver = solvers[i];

    //         solver->startNode = startNode;
    //         solver->solve();

    //         for (size_t j = 0; j < randomLocalSearches.size(); j++)
    //         {
    //             const auto &localSearch = randomLocalSearches[j];

    //             localSearch->data = solver->data;
    //             localSearch->solution = solver->solution;

    //             startTime = chrono::high_resolution_clock::now();
    //             localSearch->improve();
    //             endTime = chrono::high_resolution_clock::now();

    //             int index = i * randomLocalSearches.size() + j;
    //             if (localSearch->solutionScore > scoreStatistics[index].max)
    //             {
    //                 localSearch->saveToFile(format("{}_{}", solver->data->getName(), solver->getAlgorithmName()));
    //             }

    //             scoreStatisticsForRandomLocalSearch[index].update(localSearch->solutionScore);
    //             timeStatisticsForRandomLocalSearch[index].update(chrono::duration<double, std::milli>(endTime - startTime).count());
    //         }
    //     }
    // }
    // for(auto &stat : scoreStatisticsForRandomLocalSearch) stat.average /= maxTestsPossible;
    // for(auto &stat : timeStatisticsForRandomLocalSearch) stat.average /= maxTestsPossible;

    auto allScoreStatistics = {
        scoreStatisticsForSolver,
        scoreStatistics,
        // scoreStatisticsForRandomLocalSearch
    };

    println("\nScore statistics:");
    for(const auto &stat : allScoreStatistics | views::join)
        stat.print();

    auto allTimeStatistics = {
        timeStatisticsForSolver,
        timeStatistics,
        // timeStatisticsForRandomLocalSearch
    };

    println("\nTime statistics:");
    for(const auto &stat : allTimeStatistics | views::join)
        stat.print();

    return 0;
}