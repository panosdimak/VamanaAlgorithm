#include "gtest/gtest.h"
#include "RobustPrune.h"
#include "Graph.h"
#include "Point.h"
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

template <typename T>
class RobustPruneTest : public ::testing::Test
{
protected:
    Graph<T> graph;
    Point<T> p;
    RobustPrune<T> pruner;

    void SetUp() override
    {
        p = Point<T>({0.0, 0.0});
        graph.AddPoint(p);
    }

    void TearDown() override
    {
        graph = Graph<T>(); // Reset the graph
    }

    // Helper function to add candidate neighbors
    void AddCandidateNeighbors(const vector<vector<T>> &coordsList, vector<Point<T>> &candidates)
    {
        for (const auto &coords : coordsList)
        {
            Point<T> neighbor(coords);
            graph.AddPoint(neighbor);
            candidates.push_back(neighbor);
        }
    }
};

typedef RobustPruneTest<double> RobustPruneDoubleTest;

TEST_F(RobustPruneDoubleTest, PruneReducesToDegreeBound)
{
    vector<Point<double>> candidateNeighbors;
    AddCandidateNeighbors({{1.0, 0.0},
                           {0.0, 1.0},
                           {1.0, 1.0},
                           {-1.0, 0.0},
                           {0.0, -1.0}},
                          candidateNeighbors);

    double distanceThreshold = 1.5;
    int degreeBound = 3;

    pruner.Prune(graph, p, candidateNeighbors, distanceThreshold, degreeBound);

    auto neighbors = graph.GetNeighbors(p);
    EXPECT_EQ(neighbors.size(), degreeBound) << "Failed at PruneReducesToDegreeBound: Expected " << degreeBound << " neighbors, but got " << neighbors.size();
}

TEST_F(RobustPruneDoubleTest, PruneWithDistanceThreshold)
{
    vector<Point<double>> candidateNeighbors;
    AddCandidateNeighbors({{0.5, 0.0},
                           {0.0, 0.5},
                           {-0.5, 0.0},
                           {0.0, -0.5},
                           {1.0, 0.0}},
                          candidateNeighbors);

    double distanceThreshold = 0.6;
    int degreeBound = 4;

    pruner.Prune(graph, p, candidateNeighbors, distanceThreshold, degreeBound);

    // Assert the number of neighbors does not exceed the degree bound
    auto neighbors = graph.GetNeighbors(p);
    EXPECT_LE(neighbors.size(), degreeBound) << "Failed at PruneWithDistanceThreshold: Expected neighbors to be within degree bound, but got " << neighbors.size();

    // Ensure all neighbors are within the distance threshold of p
    for (const auto &neighbor : neighbors)
    {
        double dist = p.SquaredDistanceTo(neighbor);
        EXPECT_LE(dist, distanceThreshold * distanceThreshold) << "Failed at PruneWithDistanceThreshold: Neighbor at distance " << sqrt(dist) << " exceeds threshold";
    }

    // Ensure neighbors meet the distance threshold requirement with each other
    for (size_t i = 0; i < neighbors.size(); ++i)
    {
        for (size_t j = i + 1; j < neighbors.size(); ++j)
        {
            double distBetweenNeighbors = neighbors[i].SquaredDistanceTo(neighbors[j]);
            EXPECT_GE(distBetweenNeighbors, distanceThreshold * distanceThreshold)
                << "Failed at PruneWithDistanceThreshold: Neighbors " << neighbors[i] << " and " << neighbors[j] << " are too close";
        }
    }
}

TEST_F(RobustPruneDoubleTest, PruneWithDegreeBoundGreaterThanCandidates)
{
    vector<Point<double>> candidateNeighbors;
    AddCandidateNeighbors({{1.0, 0.0},
                           {0.0, 1.0}},
                          candidateNeighbors);

    double distanceThreshold = 1.5;
    int degreeBound = 5;

    pruner.Prune(graph, p, candidateNeighbors, distanceThreshold, degreeBound);

    auto neighbors = graph.GetNeighbors(p);
    EXPECT_EQ(neighbors.size(), candidateNeighbors.size()) << "Failed at PruneWithDegreeBoundGreaterThanCandidates: Expected " << candidateNeighbors.size() << " neighbors, but got " << neighbors.size();

    for (const auto &neighbor : neighbors)
    {
        EXPECT_NE(find(candidateNeighbors.begin(), candidateNeighbors.end(), neighbor), candidateNeighbors.end())
            << "Failed at PruneWithDegreeBoundGreaterThanCandidates: Unexpected neighbor " << neighbor;
    }
}

TEST_F(RobustPruneDoubleTest, PruneWithInitialNeighbors)
{
    Point<double> existingNeighbor({-1.0, -1.0});
    graph.AddPoint(existingNeighbor);
    graph.SetNeighbors(p, {existingNeighbor});

    vector<Point<double>> candidateNeighbors;
    AddCandidateNeighbors({{1.0, 0.0},
                           {0.0, 1.0}},
                          candidateNeighbors);

    double distanceThreshold = 1.5;
    int degreeBound = 3;

    pruner.Prune(graph, p, candidateNeighbors, distanceThreshold, degreeBound);

    auto neighbors = graph.GetNeighbors(p);
    EXPECT_EQ(neighbors.size(), 3) << "Failed at PruneWithInitialNeighbors: Expected 3 neighbors, but got " << neighbors.size();

    vector<Point<double>> expectedNeighbors = {
        existingNeighbor,
        Point<double>({1.0, 0.0}),
        Point<double>({0.0, 1.0})};

    for (const auto &neighbor : neighbors)
    {
        EXPECT_NE(find(expectedNeighbors.begin(), expectedNeighbors.end(), neighbor), expectedNeighbors.end())
            << "Failed at PruneWithInitialNeighbors: Unexpected neighbor " << neighbor;
    }
}

TEST_F(RobustPruneDoubleTest, PruneWithNoCandidates)
{
    vector<Point<double>> candidateNeighbors;
    double distanceThreshold = 1.5;
    int degreeBound = 3;

    pruner.Prune(graph, p, candidateNeighbors, distanceThreshold, degreeBound);

    auto neighbors = graph.GetNeighbors(p);
    EXPECT_TRUE(neighbors.empty()) << "Failed at PruneWithNoCandidates: Expected empty vector when there are no candidate neighbors";
}
