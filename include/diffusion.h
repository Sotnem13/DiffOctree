//
// Created by sotnem on 07.04.17.
//

#ifndef OCTREE_1_DIFFUSION_H
#define OCTREE_1_DIFFUSION_H

#include <vector>
#include <array>

#include "octree.h"
#include "point.h"

//enum class CellType {
//
//};

struct DiffusionParams {
    float    speed;             // Multiplicative factor for speed of gradient-based movement of the cells
    int64_t  T;                 // Number of time steps of simulated cell movements
    int64_t  L;                 // Defines resolution of diffusion mesh
    float    D;                 // Diffusion constant
    float    mu;                // Decay constant
    unsigned divThreshold;      // Number of divisions a cell can maximally undergo (relevant only for the first phase of the simulation)
    int64_t  finalNumberCells;  // Number of cells after cells have recursively duplicated
    float    pathThreshold;     // Maximum path length a cell can traversed before division

    float    spatialRange;      // Defines the maximal spatial extend of the clusters. This parameter is only used for computing the energy function and the correctness criterion
};


struct Cell {
    Point<float> pos;       // Cell position
    Point<float> currMov;   // Cell movement at the last time point

    int type;       // specifying cell type (+1 or -1)

    unsigned divisions;   // The number of divisions the cell has undergone
    float pathTraveled;   // Traversed path length to cell division
};


class Diffusion {
public:
    Diffusion(DiffusionParams);
    ~Diffusion();

    void produceSubstances();
    void runDiffusionStep();
    void runDecayStep();
    void cellMovementAndDuplication();

    void runDiffusionClusterStep();

    float getEnergy(int targetN);
    bool getCriterion(int targetN);


    // 3D concentration matrix
    using ConcMatrix = Octree< float >;
    ConcMatrix* conc;

    unsigned cells_count;


    std::vector<Cell> cells;  // array of all 3 dimensional cell

private:
    DiffusionParams params;

};

#endif //OCTREE_1_DIFFUSION_H


