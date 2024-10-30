#include "DataLoader.h"
#include <fstream>
#include <iostream>

// Loads .fvecs files with indexed points
vector<Point<float>> DataLoader::LoadFvecs(const string &filename)
{
    ifstream file(filename, ios::binary);
    vector<Point<float>> points;

    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return points;
    }

    int index = 0;
    while (true)
    {
        int dimension;

        // Read dimension
        file.read(reinterpret_cast<char *>(&dimension), sizeof(int));
        if (!file)
        {
            break; // Stop if read fails (end of file)
        }

        // Read vector data
        vector<float> dataVector(dimension);
        file.read(reinterpret_cast<char *>(dataVector.data()), dimension * sizeof(float));
        if (!file)
        {
            break;
        }

        points.emplace_back(dataVector, index); // Create Point with coordinates and index
        ++index;                                // Increment index for next point
    }

    file.close();
    return points;
}

// Loads .ivecs files containing ground truth indices
vector<vector<int>> DataLoader::LoadIvecs(const string &filename)
{
    ifstream file(filename, ios::binary);
    vector<vector<int>> groundTruth;

    if (!file.is_open())
    {
        cerr << "Error opening file: " << filename << endl;
        return groundTruth;
    }

    while (!file.eof())
    {
        int dimension;
        file.read(reinterpret_cast<char *>(&dimension), sizeof(int));
        if (file.eof())
        {
            break;
        }

        vector<int> dataVector(dimension);
        file.read(reinterpret_cast<char *>(dataVector.data()), dimension * sizeof(int));

        groundTruth.push_back(dataVector); // Store the vector as a ground truth entry
    }

    file.close();
    return groundTruth;
}
