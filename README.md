# Vamana Algorithm Implementation

A C++ implementation of the Vamana and Filtered Vamana algorithms for approximate nearest neighbor search in high-dimensional spaces. This project implements the graph-based indexing approach described in the DiskANN paper, with extensions for filtered search queries.

---

## Table of Contents

- [Overview](#overview)
- [Implemented Algorithms](#implemented-algorithms)
- [Project Structure](#project-structure)
- [Building the Project](#building-the-project)
- [Configuration and Usage](#configuration-and-usage)
- [Implementation Details](#implementation-details)
- [Testing](#testing)
- [References](#references)
- [Contributors](#contributors)

---

## Overview

Approximate Nearest Neighbor (ANN) search is a fundamental operation in many applications involving high-dimensional data. Exact nearest neighbor search becomes computationally prohibitive as dimensionality increases—a phenomenon known as the curse of dimensionality. Graph-based approaches, such as Vamana, address this by constructing a navigable graph structure that enables efficient approximate search.

This implementation provides:

- **Vamana Indexing**: Standard graph construction for ANN search
- **Filtered Vamana Indexing**: Extended algorithm supporting label-based filtered queries
- **Stitched Vamana Indexing**: Alternative construction method that builds per-label subgraphs and merges them

The implementation is templated to support different numeric types and includes serialization capabilities for index persistence.

---

## Implemented Algorithms

### Vamana Index Construction

The standard Vamana algorithm constructs a directed graph where each node represents a data point. The construction process:

1. Initializes the graph with random edges
2. Selects a medoid as the entry point for searches
3. Iteratively refines edges using greedy search and robust pruning
4. Maintains a maximum out-degree bound R for each node

### Filtered Vamana

Extends the base algorithm to support queries with label filters. Key modifications include:

- Per-label medoid computation for filter-aware entry points
- Label-constrained greedy search that only traverses nodes matching the query filter
- Modified pruning that considers label compatibility

### Stitched Vamana

An alternative construction approach that:

1. Partitions data points by their labels
2. Builds separate Vamana indices for each label subset
3. Merges the per-label graphs into a unified index

This can improve recall for filtered queries at the cost of increased construction time.

---

## Project Structure

```
VamanaAlgorithm/
├── include/
│   ├── Vamana.h              # Main algorithm class (templated)
│   ├── Graph.h               # Directed graph with serialization
│   ├── GreedySearcher.h      # Greedy beam search implementation
│   ├── RobustPruner.h        # Edge pruning with alpha parameter
│   ├── Point.h               # Data point with optional label/timestamp
│   ├── Distance.h            # Distance computation utilities
│   ├── Io.h                  # File I/O for datasets
│   ├── ThreadPool.h          # Thread pool for parallel operations
│   └── Utils.h               # Helper functions
├── src/
│   ├── main.cpp              # Main entry point and experiments
│   └── GtGenerator.cpp       # Ground truth generation for evaluation
├── test/
│   └── VamanaTest.cpp        # Unit tests using Google Test
├── third_party/
│   └── googletest/           # Testing framework (submodule)
├── data/                     # Dataset directory
└── Makefile
```

---

## Building the Project

### Requirements

- GCC 9+ (C++17)
- Make

### Compilation

```bash
# Clone with submodules
git clone --recursive https://github.com/sdi0700224/VamanaAlgorithm.git
cd VamanaAlgorithm

# Build main executable
make

# Build ground truth generator
make generator

# Build tests
make test-build
```

If the repository was cloned without submodules:

```bash
git submodule update --init --recursive
```

---

## Configuration and Usage

### Makefile Configuration

All runtime parameters and operations are configured directly in the `Makefile`.

#### Algorithm Parameters

```makefile
K = 50      # Number of nearest neighbors to retrieve
L = 100     # Search list size (beam width)
R = 60      # Maximum out-degree per node
A = 1.2     # Alpha parameter for robust pruning
```

| Parameter | Description | Typical Range |
|-----------|-------------|---------------|
| K | Number of nearest neighbors to return | 1-100 |
| L | Search list size; larger values improve recall at the cost of speed | K to 2K |
| R | Maximum graph degree; controls memory usage and search quality | 10-64 |
| A (α) | Pruning threshold; higher values produce sparser graphs | 1.0-1.5 |

#### Operation Types

The `OPERATION` variable controls what the program does:

```makefile
OPERATION = search    # Current operation mode
```

| Operation | Description |
|-----------|-------------|
| `create-f` | Build a Filtered Vamana index from the base dataset |
| `create-s` | Build a Stitched Vamana index from the base dataset |
| `search` | Run search queries against an existing index |

#### Dataset Configuration

```makefile
DATASET = dummy                    # Dataset name prefix (dummy, contest, etc.)
DATATYPE =                         # Dataset variant suffix (e.g., -release-1m)
GRAPH_NAME = f_50_index            # Name for the saved/loaded index file
EXP_NAME = f_50_exp                # Name for experiment output files
```

The program expects datasets in binary format located in the `data/` directory:

- `{DATASET}-data{DATATYPE}.bin` - Base dataset containing indexed points
- `{DATASET}-queries{DATATYPE}.bin` - Query points for search operations
- `{DATASET}-gt{DATATYPE}.bin` - Ground truth for evaluation (generated or provided)

#### Example Configurations

Building a filtered index with custom parameters:

```makefile
K = 100
L = 150
R = 40
A = 1.1
OPERATION = create-f
DATASET = contest
GRAPH_NAME = filtered_contest_index
```

Running search experiments:

```makefile
K = 50
L = 100
OPERATION = search
GRAPH_NAME = filtered_contest_index
EXP_NAME = recall_experiment_k50
```

### Running the Program

```bash
# Run with current Makefile configuration
make run

# Generate ground truth for a dataset
make run-generator

# Run test suite
make test-run

# Run specific tests
make test-run TEST_FILTER="VamanaTest.SearchTest"
```

### Cleaning Build Artifacts

```bash
make clean        # Remove compiled objects and binaries
make clean-exp    # Remove experiment output files
make clean-ind    # Remove saved index files
make clean-all    # Remove all generated files
```

---

## Implementation Details

### Greedy Search

The search algorithm maintains a priority queue of candidate nodes, ordered by distance to the query. At each step, it expands the closest unvisited candidate by examining its neighbors. The search terminates when L candidates have been visited or no closer candidates remain.

Key optimizations:

- Early termination when the candidate list stabilizes
- Distance caching to avoid redundant computations

### Robust Pruning

The pruning procedure selects which edges to retain for a node. Given a candidate set V, it iteratively:

1. Selects the closest candidate p* to the node
2. Adds edge (node, p*) to the graph
3. Removes candidates that are "covered" by p* (within α × distance)

The α parameter controls the trade-off between graph connectivity and degree. Higher values produce sparser graphs with longer search paths but lower memory usage.

### Medoid Selection

The medoid serves as the entry point for all searches. It is computed as the point minimizing the sum of distances to all other points. For filtered indices, per-label medoids are computed to provide better entry points for filtered queries.

### Graph Representation

The graph is stored as an adjacency list using `unordered_map<int, vector<int>>`. Each node ID maps to its list of outgoing neighbors. This representation supports:

- O(1) average neighbor lookup
- Efficient iteration over neighbors
- Binary serialization for persistence

---

## Testing

The test suite uses Google Test and covers:

- Medoid computation correctness
- Graph construction and serialization
- Search result validity
- Filter functionality
- Edge cases (empty datasets, single-point queries)

```bash
make test-build
make test-run
```

---

## References

1. Subramanya, S. J., et al. "DiskANN: Fast Accurate Billion-point Nearest Neighbor Search on a Single Node." *Advances in Neural Information Processing Systems* 32 (2019).

2. Gollapudi, S., et al. "Filtered-DiskANN: Graph Algorithms for Approximate Nearest Neighbor Search with Filters." *Proceedings of the ACM Web Conference* (2023).

The following ground truth generation utilities have been adapted from the [SIGMOD 2024 Contest repository](https://github.com/KevinZeng08/sigmod-2024-contest.git):

- `GtGenerator.cpp`
- `Distance.h`
- `Io.h`
- `ThreadPool.h`
- `Utils.h`

---

## Contributors

- Vasileios Roussos
- Panagiotis Dimakakos
- Phoebe Orfanakou
