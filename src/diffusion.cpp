//
// Created by sotnem on 07.04.17.
//

#include "../include/diffusion.h"
#include <cmath>
#include <chrono>
#include <iostream>

//static const std::array<Point<int>, 6> dv = {{{1, 0, 0}, {-1, 0, 0},
//                                             {0, 1, 0}, {0, -1, 0},
//                                             {0, 0, 1}, {0, 0, -1}} };

Diffusion::Diffusion(DiffusionParams params) : params(params), cells(params.finalNumberCells) {

    conc = new ConcMatrix({0, 0, 0}, {(float)params.L, (float)params.L, (float)params.L});

    cells_count = 2;
    cells[0].pos = {0.58, 0.5, 0.5};
    cells[1].pos = {0.42, 0.5, 0.5};
    cells[0].divisions = 0;
    cells[1].divisions = 0;

//    for (int i = 0; i < params.finalNumberCells; i++) {
//        Cell &cell = cells[i];
//        cell.pos = {0.5, 0.5, 0.5};
//        cell.currMov = {0, 0, 0};
//        cell.pathTraveled = 0;
//    }/

}

Diffusion::~Diffusion() {

    delete conc;

}

void Diffusion::produceSubstances() {

    // increases the concentration of substances at the location of the cells
//    float sideLength = 1.f/params.L; // length of a side of a diffusion voxel
    for (int i = 0; i < cells_count; i++) {
        auto& c = cells[i];
//    for ( auto& c : cells ) {
//        ConcMatrix::P p = c.pos/sideLength;
        ConcMatrix::P p = c.pos*params.L;

//        auto concCell = (*conc)(p);
//        if (c.type==1) {
//            concCell = (*conc)(p)[0];
//        }
        auto& val = conc->at(p);
        val += 0.1;
        if (val > 1) {
            val = 1;
        }
    }
}

// computes the changes in substance concentrations due to diffusion
void Diffusion::runDiffusionStep() {
//    using namespace std::chrono;
//    high_resolution_clock clock;
//
//    auto begin = clock.now();

    const ConcMatrix prevConc(*conc);
    auto& cell_size = conc->cellSize();

    for (auto& p : prevConc) {
        for (auto &v : dv) {
            const auto &neighbor = prevConc.at(p.first + v * cell_size);
            auto delta = (neighbor - p.second) * params.D / 6.f;
            auto &val = conc->at(p.first);
            val += delta;
        }
    }
}

void Diffusion::runDecayStep() {
    // computes the changes in substance concentrations due to decay
    for (auto p : *conc) {
        p.second *= (1-params.mu);
    }
}

void Diffusion::cellMovementAndDuplication() {

    using P = ConcMatrix::P;
    for (int i=0; i < cells_count; i++) {
        Cell& currentCell = cells[i];

        // random cell movement
        P currentCellMovement = P::random() - P{0.5,0.5,0.5};

        currentCell.pos += 0.05 * currentCellMovement / currentCellMovement.normal();
        currentCell.pathTraveled += 0.05;

        // cell duplication if conditions fulfilled
        if (currentCell.divisions < params.divThreshold && currentCell.pathTraveled > params.pathThreshold) {
            currentCell.pathTraveled -= params.pathThreshold;

            currentCell.divisions += 1; // update number of divisions this cell has undergone
            cells_count++; // update number of cells in the simulation

            if( cells.size() < cells_count)
                cells.push_back(Cell{});
            Cell &duplicatedCell = cells[cells_count-1];
//            Cell &duplicatedCell = *(--cells.end());

            duplicatedCell.divisions = currentCell.divisions; // update number of divisions the duplicated cell has undergone
            duplicatedCell.type = -currentCell.type; // assign type of duplicated cell (opposite to current cell)

            // assign location of duplicated cell
            P duplicatedCellOffset = P::random() - P{0.5, 0.5, 0.5};
            duplicatedCell.pos = currentCell.pos + 0.05 * duplicatedCellOffset / duplicatedCellOffset.normal();

        }

    }

}

void Diffusion::runDiffusionClusterStep() {
    // computes movements of all cells based on gradients of the two substances
    using P = ConcMatrix::P;
    float sideLength = 1.f/params.L; // length of a side of a diffusion voxel

    P gradSub;
    const P &cell_size = conc->cellSize();


    float normGrad1, normGrad2;

    for (int c = 0; c < cells_count; c++) {

        Cell &cell = cells[c];
        auto p = cell.pos / sideLength;
//        for (int i = 0; i < 2; i++) {

            auto &grad = gradSub;
            for (int j = 0; j < 3; j++) {
                P pd;
                pd.coords[j] = cell_size.coords[j];
                P p1 = p + pd;
                P p2 = p - pd;

                p1.coords[j] = p1.coords[j] < (params.L - 1) ? p1.coords[j] : (params.L - 1);
                p2.coords[j] = p2.coords[j] > 0 ? p2.coords[j] : 0;

//                grad.coords[j] = ((*conc)(p1)[i] - (*conc)(p2)[i]) / sideLength * (p1 + p2).coords[j];
//                grad.coords[j] = ((*conc)(p1) - (*conc)(p2)) / sideLength * (p1 + p2).coords[j];
            }
//        }

        normGrad1 = gradSub.normal();
//        normGrad2 = gradSub[1].normal();
//        if( normGrad1 != 0 && normGrad2 != 0 ) {
//            cell.currMov = cell.type*(gradSub[0]/normGrad1-gradSub[1]/normGrad2)*params.speed;
//        } else {
//            cell.currMov = P{0,0,0};
//        }

    }
}

float Diffusion::getEnergy(int targetN) {

    // Computes an energy measure of clusteredness within a subvolume. The size of the subvolume
    // is computed by assuming roughly uniform distribution within the whole volume, and selecting
    // a volume comprising approximately targetN cells.

    using P = ConcMatrix::P;
    Cell *subvol = new Cell[cells_count];

    float subVolMax = (float)std::pow(1.f * targetN / cells_count, 1.0 / 3.0) / 2;

    int nrCellsSubVol = 0;

    float intraClusterEnergy = 0.0;
    float extraClusterEnergy = 0.0;
    float nrSmallDist=0.0;

    for (int i = 0; i < cells_count; i++) {

        Cell &cell = cells[i];
        P v = cell.pos - P{0.5,0.5,0.5};

        if( std::abs(v.x) < subVolMax &&
            std::abs(v.y) < subVolMax &&
            std::abs(v.z) < subVolMax) {

            subvol[nrCellsSubVol].pos = cell.pos;
            subvol[nrCellsSubVol].type = cell.type;

            nrCellsSubVol++;
        }
    }

    for (int i = 0; i < nrCellsSubVol; i++) {
        for(int j = i+1; j < nrCellsSubVol; j++) {

            Cell &c1 = subvol[i];
            Cell &c2 = subvol[j];
            float currDist = c1.pos.getL2Distance(c2.pos);

            if (currDist < params.spatialRange ) {
                nrSmallDist += 1 ;//currDist/spatialRange;

                if (c1.type*c2.type>0) {
                    intraClusterEnergy += std::min(100.0f,params.spatialRange/currDist);
                } else {
                    extraClusterEnergy += std::min(100.0f,params.spatialRange/currDist);
                }
            }
        }
    }

    delete[] subvol;

    float totalEnergy = (extraClusterEnergy-intraClusterEnergy)/(1.0f+100.0f*nrSmallDist);
    return totalEnergy;
}

bool Diffusion::getCriterion(int targetN) {

    // Returns 0 if the cell locations within a subvolume of the total system, comprising approximately targetN cells,
    // are arranged as clusters, and 1 otherwise.
    using P = ConcMatrix::P;

    int nrClose=0;      // number of cells that are close (i.e. within a distance of spatialRange)
    int sameTypeClose=0; // number of cells of the same type, and that are close (i.e. within a distance of spatialRange)
    int diffTypeClose=0; // number of cells of opposite types, and that are close (i.e. within a distance of spatialRange)

    Cell *subvol = new Cell[cells_count];

    float subVolMax = (float)std::pow(1.f * targetN / cells_count, 1.0 / 3.0) / 2;

    int nrCellsSubVol = 0;

    // the locations of all cells within the subvolume are copied to array posSubvol
    for (int i = 0; i < cells_count; i++) {
        Cell &cell = cells[i];
        P v = cell.pos - P{0.5,0.5,0.5};

        if( std::abs(v.x) < subVolMax &&
            std::abs(v.y) < subVolMax &&
            std::abs(v.z) < subVolMax) {

            subvol[nrCellsSubVol].pos = cell.pos;
            subvol[nrCellsSubVol].type = cell.type;

            nrCellsSubVol++;
        }
    }

//    if(quiet < 1)
//        printf("number of cells in subvolume: %d\n", nrCellsSubVol);


    // If there are not enough cells within the subvolume, the correctness criterion is not fulfilled
    if ( 1.f * nrCellsSubVol /targetN < 0.25f ) {
//        getCriterion_sw.mark();
//        if(quiet < 2)
//            printf("not enough cells in subvolume: %d\n", nrCellsSubVol);
        return false;
    }

    // If there are too many cells within the subvolume, the correctness criterion is not fulfilled
    if ((((float)(nrCellsSubVol))/(float)targetN) > 4) {
//        if(quiet < 2)
//            printf("too many cells in subvolume: %d\n", nrCellsSubVol);
        return false;
    }
    for (int i = 0; i < nrCellsSubVol; i++) {
        for(int j = i+1; j < nrCellsSubVol; j++) {

            Cell &c1 = subvol[i];
            Cell &c2 = subvol[j];
            float currDist = c1.pos.getL2Distance(c2.pos);

            if (currDist < params.spatialRange ) {
                nrClose++;
                if (c1.type * c2.type < 0) {
                    diffTypeClose++;
                } else {
                    sameTypeClose++;
                }
            }
        }
    }

    float correctness_coefficient = diffTypeClose/(nrClose+1.0f);

    // check if there are many cells of opposite types located within a close distance, indicative of bad clustering
    if (correctness_coefficient > 0.1) {
//        if(quiet < 2)
//            printf("cells in subvolume are not well-clustered: %f\n", correctness_coefficient);
        return false;
    }

    // check if clusters are large enough, i.e. whether cells have more than 100 cells of the same type located nearby
    float avgNeighbors = 1.f * sameTypeClose/nrCellsSubVol;
//    if(quiet < 1)
//        printf("average neighbors in subvolume: %f\n", avgNeighbors);
    if (avgNeighbors < 100) {
//        if(quiet < 2)
//            printf("cells in subvolume do not have enough neighbors: %f\n", avgNeighbors);
        return false;
    }

//    if(quiet < 1)
//        printf("correctness coefficient: %f\n", correctness_coefficient);

    return true;
}
