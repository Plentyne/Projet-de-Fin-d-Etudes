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
    void heuristicIDCH(Solution &bestSolution);

    /*  fast version of IDCH, stop at n iterations without improvement (no perturbation) */
    void heuristicFastIDCH(Solution &bestSolution);

    void doRepair(Solution &solution);

    void doDestroy(Solution &solution);

    void doDestroySmall(Solution &solution);

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

    /* Route Removal : Randomly choses between 0 and p4 routes and removes them from the solution. The customers
     * belonging to these routes are then inserted into the customers' pool and reinserted.
     * */ void doRouteRemoval(Solution &solution, int p4);

    /* Remove Single Node Routes : removes all routes that only contain one
     * customer. Customers are, then, put into the pool of unrouted customers.
     * */ void doRemoveSingleNodeRoutes(Solution &solution);

    /* Satellite Removal : Closes a random chosen satellite and destroys the routes originating from it.
     * The customers of these routes are then inserted into the customers' pool. In case the closed satellite
     * was the last one open, another one is randomly chosen to be opened. This operator is to be used with
     * probability p6 every w iterations, because it hugely destroys the solution.
     * */ void doSatelliteRemoval(Solution &solution, double p6);

    /* Satellite Opening : Opens a randomly chosen satellite. The p7 closest customers to the satellite
     * are then removed from their current routes and reinserted into the customers' pool.
     * */ void doSatelliteOpening(Solution &solution, double p7);

    /* Open All Satellites : Opens all previously closed satellites.
     * */ void doOpenAllSatellites(Solution &solution);

    /*********************************
    /* Perturbation and local search
    /*********************************/
    bool apply2OptOnEachTour(Solution &solution);

    bool doLocalSearch(Solution &solution);

};


#endif //PROJET_DE_FIN_D_ETUDES_IDCH_H
