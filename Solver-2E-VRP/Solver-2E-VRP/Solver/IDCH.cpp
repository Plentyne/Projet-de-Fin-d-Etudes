//
// Created by Youcef on 14/03/2016.
//

#include "IDCH.h"
#include "../Utility.h"
#include "./SDVRPSolver.h"
#include <algorithm>

/****************************************************
 *  
 *                IDCH HEURISTICS
 *  
 ****************************************************/
void IDCH::heuristicIDCH(Solution &s) {

}

void IDCH::heuristicFastIDCH(Solution &bestSolution) {
    int n = this->problem->getClients().size(),
            iter = 0,
            itermax = n;

    if (bestSolution.getTotalCost() == 0) {
        this->doGreedyInsertion(bestSolution);
    }

    Solution solution = bestSolution;

    //this->doGreedyInsertion(si);

    // Todo enlever
    SDVRPSolver sdvrpSolver(this->problem);

    while (iter < itermax) {
        // Destruction de la solution
        this->doDestroy(solution);

        // Perturbation de la solution
        this->apply2OptOnEachTour(solution);

        // Réparation de la solution
        this->doGreedyInsertion(solution);

        if (solution.getTotalCost() < bestSolution.getTotalCost()) {
            bestSolution = solution;
            iter = 0;
        } else iter++;
    }
}

/****************************************************
 *  
 *                REPAIR OPERATORS
 *  
 ****************************************************/
void IDCH::doGreedyInsertion(Solution &solution) {
    this->insertion.GreedyInsertionHeuristic(solution);
}

void IDCH::doGreedyInsertionPerturbation(Solution &solution) {

}

void IDCH::doMoleAndJamesonInsertion(Solution &solution) {

}

/****************************************************
 *  
 *                DESTROY OPERATORS
 *  
 ****************************************************/
void IDCH::doRandomRemoval(Solution &solution, double p1) {
    int n = problem->numberOfClients();
    // nc : number of customers to remove
    int nc = static_cast<int>(lround(p1 * n));
    int rt, // Route number
            rc;  // Customer customer

    double cost;

    int i = 0;
    while (i < nc && i < n) {
        // Find a customer to remove
        rt = Utility::randomInt(0, solution.getE2Routes().size());
        rc = Utility::randomInt(0, solution.getE2Routes()[rt].tour.size());
        // Compute removal cost
        cost = removalCost(solution, rc, rt);
        // Insert customer into list of unrouted customers
        solution.unroutedCustomers.push_back(solution.getE2Routes()[rt].tour[rc]);

        // Remove route if it contains only one customer
        if (solution.getE2Routes()[rt].tour.size() == 1) {
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    solution.getE2Routes()[rt].tour[rc]).getDemand();
            // remove route
            solution.getE2Routes().erase(solution.getE2Routes().begin() + rt);
        }
        else {
            // Update route cost
            solution.getE2Routes()[rt].cost += cost;
            // Update route load
            solution.getE2Routes()[rt].load -= problem->getClient(solution.getE2Routes()[rt].tour[rc]).getDemand();
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    solution.getE2Routes()[rt].tour[rc]).getDemand();
            // Remove the customer from the route it is in
            solution.getE2Routes()[rt].tour.erase(solution.getE2Routes()[rt].tour.begin() + rc);
        }
        // Update solution cost
        solution.setTotalCost(solution.getTotalCost() + cost);
        // Go to the next customer
        i++;
    }
}

void IDCH::doWorstRemoval(Solution &solution, double p2) {

    int n = problem->numberOfClients();
    // nc : number of customers to remove
    int nc = static_cast<int>(lround(p2 * n));
    int rt, // Route number
            rc;  // Customer customer

    double cost, tmpCost;

    int i = 0;
    while (i < nc && i < n) {
        cost = 0;
        // Find a customer to remove
        for (int u = 0; u < solution.getE2Routes().size(); ++u) {
            E2Route &route = solution.getE2Routes()[u];
            for (int v = 0; v < route.tour.size(); ++v) {
                tmpCost = removalCost(solution, v, u);
                if (tmpCost > cost) {
                    rt = u;
                    rc = v;
                    cost = tmpCost;
                }

            }
        }
        // Insert customer into list of unrouted customers
        solution.unroutedCustomers.push_back(solution.getE2Routes()[rt].tour[rc]);

        // Remove route if it contains only one customer
        if (solution.getE2Routes()[rt].tour.size() == 1) {
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    solution.getE2Routes()[rt].tour[rc]).getDemand();
            // remove route
            solution.getE2Routes().erase(solution.getE2Routes().begin() + rt);
        }
        else {
            // Update route cost
            solution.getE2Routes()[rt].cost += cost;
            // Update route load
            solution.getE2Routes()[rt].load -= problem->getClient(solution.getE2Routes()[rt].tour[rc]).getDemand();
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    solution.getE2Routes()[rt].tour[rc]).getDemand();
            // Remove the customer from the route it is in
            solution.getE2Routes()[rt].tour.erase(solution.getE2Routes()[rt].tour.begin() + rc);
        }
        // Update solution cost
        solution.setTotalCost(solution.getTotalCost() + cost);
        // Go to the next customer
        i++;
    }

}

void IDCH::doRelatedRemoval(Solution &solution, double p3) {

}

void IDCH::doRouteRemoval(Solution &solution, double p4) {

}

void IDCH::doRemoveSingleNodeRoutes(Solution &solution) {

}

void IDCH::doSatelliteRemoval(Solution &solution, double p6) {

}

void IDCH::doSatelliteOpening(Solution &solution, double p7) {

}


/****************************************************
 *  
 *           PERTRBATION AND LOCAL SEARCH
 *  
 ****************************************************/
bool IDCH::apply2OptOnEachTour(Solution &solution) {
    bool improvement = false;
    double imp = 0;
    for (E2Route route : solution.getE2Routes()) {
        imp -= route.cost;
        improvement = improvement || lsSolver.apply2OptOnTour(route);
        imp += route.cost;
    }
    if (imp < -0.0001) solution.setTotalCost(solution.getTotalCost() + imp);
    return improvement;
}

/****************************************************
 *  
 *                OTHER METHODS
 *  
 ****************************************************/
double IDCH::removalCost(Solution &solution, int customer, int route) {

    if (solution.getE2Routes()[route].tour.size() == 1)
        return 2 * problem->getDistance(problem->getSatellite(solution.getE2Routes()[route].departureSatellite),
                                        problem->getClient(solution.getE2Routes()[route].tour[customer]));

    Node p, q, c;
    c = problem->getClient(solution.getE2Routes()[route].tour[customer]);
    if (customer == 0) {
        p = problem->getSatellite(solution.getE2Routes()[route].departureSatellite);
    }
    else {
        p = problem->getClient(solution.getE2Routes()[route].tour[customer - 1]);
    }

    if (customer == solution.getE2Routes()[route].tour.size() - 1) {
        q = problem->getSatellite(solution.getE2Routes()[route].departureSatellite);
    }
    else {
        q = problem->getClient(solution.getE2Routes()[route].tour[customer + 1]);
    }
    return problem->getDistance(p, q) - (problem->getDistance(p, c) + problem->getDistance(c, q));
}

bool IDCH::doLocalSearch(Solution &solution) {
    return false;
}

void IDCH::doDestroy(Solution &solution) {
    /* Todo 1 changer les paramètres
     * Todo 2 implémenter un schéma pour l'utilisation des opérateurs
    */
    this->doWorstRemoval(solution, 0.3);
}

void IDCH::doRepair(Solution &solution) {

}
