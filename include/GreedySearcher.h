#pragma once

#include <vector>
#include <unordered_set>
#include "Point.h"
#include "Graph.h"

using namespace std;

template <typename T>
class GreedySearcher
{
public:
    pair<vector<Point<T>>, vector<Point<T>>> FilteredGreedySearch(
        const Graph<T> &graph,
        const Point<T> &queryPoint,
        const unordered_set<T> &queryFilters, // Filters (Fq) for the query
        const vector<Point<T>> &startPoints,  // Initial set of nodes (S)
        int numResults,                       // Number of results (k)
        int maxCandidates) const;             // Maximum search list size (L)

    pair<vector<Point<T>>, vector<Point<T>>> GreedySearch(
        const Graph<T> &graph,
        const Point<T> &startPoint,
        const Point<T> &queryPoint,
        int numResults,
        int maxCandidates) const;
};

#include "GreedySearcher.tpp"
