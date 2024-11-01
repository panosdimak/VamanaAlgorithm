#pragma once

#include <vector>
#include "Point.h"
#include "Graph.h"

using namespace std;

template <typename T>
class RobustPrune
{
public:
    void Prune(Graph<T> &graph, const Point<T> &p, const vector<Point<T>> &candidateNeighbors, double distanceThreshold, int degreeBound);
};

#include "RobustPrune.tpp"
