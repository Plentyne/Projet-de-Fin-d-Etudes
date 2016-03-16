//
// Created by Youcef on 14/03/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_IDCH_H
#define PROJET_DE_FIN_D_ETUDES_IDCH_H

#include "../Model/Solution.h"
#include "Insertion.h"
#include "LSSolver.h"

class IDCH {
private:
    Problem *problem;
    Insertion insertion;
    LSSolver lsSolver;

    double removalCost(Solution &solution, int customer, int route);

public:

    IDCH(Problem *problem) : problem(problem), insertion(problem), lsSolver(problem) { }

/*  iterative destruction construction heuristic (original form)
     *  stop at n^2 iterations without improvement
     *  and the search is disturbed every n iterations */
    void heuristicIDCH(Solution &s);

    /*  fast version of IDCH, stop at n iterations without improvement (no perturbation) */
    void heuristicFastIDCH(Solution &bestSolution);

    void doRepair(Solution &solution);

    void doDestroy(Solution &solution);

    /****************************
    /* Repair operators
    /****************************/
    void doGreedyInsertion(Solution &solution);

    void doGreedyInsertionPerturbation(Solution &solution);

    void doMoleAndJamesonInsertion(Solution &solution);

    /****************************
    /* Destroy operators
    /****************************/
    /* Random Removal : randomly removes q customers from the solution given
     * in parametre and puts them into the pool of customers to be re-inserted.
     * p1 represents the ratio ( q / total number of customers )
     * */ void doRandomRemoval(Solution &solution, double p1);

    /* Worst Removal :
     * */ void doWorstRemoval(Solution &solution, double p2);

    /* Related Removal
     * */ void doRelatedRemoval(Solution &solution, double p3);

    /* Route Removal :
     * */ void doRouteRemoval(Solution &solution, double p4);

    /* Remove Single Node Routes : removes all routes that only contain one
     * customer. Customers are, then, put into the pool of unrouted customers.
     * */ void doRemoveSingleNodeRoutes(Solution &solution);

    /* Satellite Removal :
     * */ void doSatelliteRemoval(Solution &solution, double p6);

    /* Satellite Opening :
     * */ void doSatelliteOpening(Solution &solution, double p7);

    /*********************************
    /* Perturbation and local search
    /*********************************/
    bool apply2OptOnEachTour(Solution &solution);

    bool doLocalSearch(Solution &solution);

};


#endif //PROJET_DE_FIN_D_ETUDES_IDCH_H
