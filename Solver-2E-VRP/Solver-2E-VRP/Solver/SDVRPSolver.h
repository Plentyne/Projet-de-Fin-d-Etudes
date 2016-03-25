//
// Created by Youcef on 10/03/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_SDVRPSOLVER_H
#define PROJET_DE_FIN_D_ETUDES_SDVRPSOLVER_H


#include "../Model/Solution.h"

class SDVRPSolver {
public:
    SDVRPSolver() : problem(nullptr), w(0) { }

    SDVRPSolver(const Problem *problem) : problem(problem), w(0) { }

// Constructive Heuristic proposed by Aleman
    void constructiveHeuristic(Solution &solution);

    // Cost for changing satellite demand by dDemand
    double insertionCost(Solution &solution, int satelliteId, int dDemand);

    // Add dDmand to satellite satelliteId in current solution
    void insert(Solution &solution, int satelliteId, int dDemand);

private:
    const Problem *problem;

    int w;

    double apply2Opt(E1Route &route);

    double applyRelocate(Solution &solution);

    double applySwap(Solution &solution);

    /* Does Best Improvement*/
    double applyOrOpt(E1Route &route, int seqLength); /* Does Best Improvement*/

};


#endif //PROJET_DE_FIN_D_ETUDES_SDVRPSOLVER_H
