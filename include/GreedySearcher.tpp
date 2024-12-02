#include "GreedySearcher.h"
#include <vector>
#include <set>
#include <unordered_set>
#include <utility>
#include <iostream>

template <typename T>
pair<vector<Point<T>>, vector<Point<T>>> GreedySearcher<T>::FilteredGreedySearch(
    const Graph<T> &graph,
    const Point<T> &queryPoint,
    const unordered_set<T> &queryFilters, // Filters (Fq) for the query
    const vector<Point<T>> &startPoints,  // Initial set of nodes (S)
    int numResults,                       // Number of results (k)
    int maxCandidates) const              // Maximum search list size (L)
{
    set<Point<T>> visited;                  // Set of visited nodes
    set<pair<double, Point<T>>> candidates; // Set of candidates sorted by distance

    // Initialize candidate set with start points filtered by queryFilters
    for (const auto &startPoint : startPoints)
    {
        if (queryFilters.count(startPoint.GetFilter()) > 0) // Check filter condition
        {
            double dist = startPoint.SquaredDistanceTo(queryPoint);
            candidates.insert({dist, startPoint});
        }
    }

    // Continue until we process all candidates or exceed maxCandidates
    while (!candidates.empty())
    {
        // Get the closest candidate that hasn't been visited
        auto pStarIter = candidates.begin();
        while (pStarIter != candidates.end() && visited.count(pStarIter->second) > 0)
        {
            ++pStarIter;
        }

        // If all candidates have been visited, terminate the loop
        if (pStarIter == candidates.end())
        {
            break;
        }

        Point<T> pStar = pStarIter->second; // Current best candidate
        visited.insert(pStar);              // Mark p* as visited

        // Explore neighbors of pStar that satisfy the filter and haven't been visited
        for (const auto &neighbor : graph.GetNeighbors(pStar))
        {
            if (visited.count(neighbor) == 0 && queryFilters.count(neighbor.GetFilter()) > 0)
            {
                double dist = neighbor.SquaredDistanceTo(queryPoint);
                candidates.insert({dist, neighbor});
            }
        }

        // Maintain limited size for candidates by removing the farthest ones
        while ((int)candidates.size() > maxCandidates)
        {
            candidates.erase(--candidates.end());
        }
    }

    // Collect the closest k results
    vector<Point<T>> result;
    for (auto it = candidates.begin(); it != candidates.end() && (int)result.size() < numResults; ++it)
    {
        result.push_back(it->second);
    }

    // Convert visited set to vector
    vector<Point<T>> visitedVector(visited.begin(), visited.end());

    return {result, visitedVector};
}

template <typename T>
pair<vector<Point<T>>, vector<Point<T>>> GreedySearcher<T>::GreedySearch(
    const Graph<T> &graph,
    const Point<T> &startPoint,
    const Point<T> &queryPoint,
    int numResults,
    int maxCandidates) const
{
    set<pair<double, Point<T>>> visited;
    set<pair<double, Point<T>>> candidateSet;
    candidateSet.insert({startPoint.SquaredDistanceTo(queryPoint), startPoint});

    // Continue until we only have visited nodes left in candidateSet
    while (!candidateSet.empty())
    {
        // Find the closest unvisited point in candidateSet
        auto pStarIter = candidateSet.begin();
        while (pStarIter != candidateSet.end() && visited.count(*pStarIter) > 0)
        {
            ++pStarIter;
        }

        // If no unvisited nodes remain in candidateSet, terminate the loop
        if (pStarIter == candidateSet.end())
        {
            break;
        }

        Point<T> pStar = pStarIter->second;
        double pStarDist = pStarIter->first;

        visited.insert({pStarDist, pStar}); // Mark p* as visited

        // Get neighbors of p* and add them to candidates if not already visited
        for (const auto &neighbor : graph.GetNeighbors(pStar))
        {
            double dist = neighbor.SquaredDistanceTo(queryPoint);
            if (visited.count({dist, neighbor}) == 0) // Only consider unvisited neighbors
            {
                candidateSet.insert({dist, neighbor});
            }
        }

        // Maintain limited size for candidateSet by removing the farthest
        while ((int)candidateSet.size() > maxCandidates)
        {
            candidateSet.erase(--candidateSet.end());
        }
    }

    // Extract the closest k points from candidateSet
    vector<Point<T>> result;
    for (auto it = candidateSet.begin(); it != candidateSet.end() && (int)result.size() < numResults; ++it)
    {
        result.push_back(it->second);
    }

    // Convert visited set to vector
    vector<Point<T>> visitedVector;
    for (const auto &pair : visited)
    {
        visitedVector.push_back(pair.second);
    }

    return {result, visitedVector};
}
