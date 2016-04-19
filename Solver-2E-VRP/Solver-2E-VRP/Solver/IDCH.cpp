//
// Created by Youcef on 14/03/2016.
//

#include "IDCH.h"
#include "../Utility.h"
#include "./SDVRPSolver.h"
#include "MoleJamesonHeuristic.h"
#include <algorithm>
#include "../Config.h"
#include "../Statistic.h"
/****************************************************
 *  
 *                IDCH HEURISTICS
 *  
 ****************************************************/
/* Version Classique de l'IDCH
 */
/*void IDCH::heuristicIDCH(Solution &bestSolution) {
    int n = this->problem->getClients().size(),
            iter = 0,
            itermax = n * n;

    if (bestSolution.getTotalCost() == 0) {
        this->doGreedyInsertion(bestSolution);
    }

    Solution solution = bestSolution;

    while (iter < itermax) {

        // Large destruction step
        if ((iter + 1) % (n/10) == 0) {
            this->doDestroyLarge(solution);
        }
            // Small destruction step
        else {
            this->doDestroySmall(solution);
        }

        // Perturbation de la solution
        this->apply2OptOnEachTour(solution);

        // Réparation de la solution
        this->doRepair(solution);

        if(solution.getTotalCost() == Config::DOUBLE_INFINITY) {
            solution = bestSolution;
            iter++;
            continue;
        }
        else {
            // Recherche locale
            this->doLocalSearch(solution);
            solution.recomputeCost();
            //solution.print();

            if (solution.getTotalCost() - bestSolution.getTotalCost() < -0.01) {
                bestSolution = solution;
                iter = 0;
            } else iter++;
        }

    }

}*/


/* Version utilisant BestSolution à chaque fois
 * */
/*void IDCH::heuristicIDCH(Solution &bestSolution) {
    int n = this->problem->getClients().size(),
            iter = 0,
            itermax = n * n;

    if (bestSolution.getTotalCost() == 0) {
        this->doGreedyInsertion(bestSolution);
    }

    this->doLocalSearch(bestSolution);
    Solution solution = bestSolution;

    while (iter < itermax) {

        // Large destruction step
        if ((iter + 1) % (n/5) == 0) {
            this->doDestroyLarge(solution);
        }// Small destruction step
        else {
            doDestroySmall(solution);
        }
        // Perturbation de la solution
        this->apply2OptOnEachTour(solution);

        // Réparation de la solution
        this->doRepair(solution);

        if(solution.getTotalCost() == Config::DOUBLE_INFINITY) {
            solution = bestSolution;
            iter++;
            continue;
        }
        else {
            // Recherche locale
            this->doLocalSearch(solution);
            solution.recomputeCost();
            //solution.print();

            if (solution.getTotalCost() - bestSolution.getTotalCost() < -0.01) {
                bestSolution = solution;
                iter = 0;
            } else {
                iter++;
            }
        }

    }

}*/

/* Version commençant par une destruction petite puis agrandi*/
void IDCH::heuristicIDCH(Solution &bestSolution) {
    int n = this->problem->getClients().size(),
            iter = 0,
            itermax = n * n;

    if (bestSolution.getTotalCost() == 0) {
        this->doGreedyInsertion(bestSolution);
    }
    this->doLocalSearch(bestSolution);
    Solution solution = bestSolution;

    double dmax = 3;
    double beta;

    while (iter < itermax) {

        //cout << "debut boucle" << endl;
        solution = bestSolution;
        // Large destruction step
        if ((iter + 1) % (n/5) == 0) {
            this->doDestroyLarge(solution);
            //cout << "apres destroy large" << endl;
        }
            // Small destruction step
        else {
            beta = Utility::randomDouble(1./(double)problem->getClients().size(), dmax/(double)problem->getClients().size() );
            this->doRandomRemoval(solution, beta);
            //cout << "apres destroy small" << endl;
        }

        // Perturbation de la solution
        this->apply2OptOnEachTour(solution);

        // Réparation de la solution
        //cout << "avant repair" << endl;
        this->doRepair(solution);

        if(solution.getTotalCost() == Config::DOUBLE_INFINITY) {
            solution = bestSolution;
            iter++;
            continue;
        }
        else {
            // Recherche locale
            this->doLocalSearch(solution);
            solution.recomputeCost();
            //solution.print();

            if (solution.getTotalCost() - bestSolution.getTotalCost() < -0.01) {
             bestSolution = solution;
             Statistic::dmaxs.push_back(dmax);
             dmax = 3;
             iter = 0;
            } else {
            iter++;
            if(dmax/(double)problem->getClients().size() < 0.75) dmax++;
            }
        }


    }

}


void IDCH::heuristicFastIDCH(Solution &bestSolution) {
    int n = this->problem->getClients().size(),
            iter = 0,
            itermax = n;

    if (bestSolution.getTotalCost() == 0) {
        this->doGreedyInsertion(bestSolution);
        //MoleJamesonHeuristic solver(this->problem, 1, 1);
        //solver.solve(bestSolution);
    }

    Solution solution = bestSolution;

    while (iter < itermax) {
        // Destruction de la solution
        //this->doDestroySmall(solution);
        this->doDestroyLarge(solution);
        // Perturbation de la solution
        this->apply2OptOnEachTour(solution);

        // Réparation de la solution
        this->doRepair(solution);

        // local search
        this->doLocalSearch(solution);

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
    this->insertion.GreedyInsertionNoiseHeuristic(solution);
}

void IDCH::doMoleAndJamesonInsertion(Solution &solution) {

}

/****************************************************
 *  
 *                DESTROY OPERATORS
 *  
 ****************************************************/
void IDCH::doRandomRemoval(Solution &solution, double p1) {
    if (solution.getE2Routes().size()==0)
        return;
    int n = problem->numberOfClients();
    // nc : number of customers to remove
    int nc = static_cast<int>(lround(p1 * n));
    n -= solution.unroutedCustomers.size();
    nc = min(nc, n);
    int rt, // Route number
            rc;  // Customer customer

    double cost;

    // Todo améliorer l'implémentation
    int remaining = n;
    vector<bool> tried(problem->getClients().size(), false);
    for (int j = 0; j < solution.unroutedCustomers.size(); ++j) {
        tried[solution.unroutedCustomers[j]] = true;
        remaining--;
    }

    int i = 0;
    while (i < nc && remaining > 0) {
        // Find a customer to remove
        do {
            rt = Utility::randomInt(0, solution.getE2Routes().size());
            rc = Utility::randomInt(0, solution.getE2Routes()[rt].tour.size());
            // In case we already tried to remove the customer but couldn't do it, ignore it
            if (tried[solution.getE2Routes()[rt].tour[rc]]) continue;
            // Mark the customer as already tried
            tried[solution.getE2Routes()[rt].tour[rc]] = true;
            remaining--;
            // Compute removal cost
            cost = removalCost(solution, rc, rt);
        } while (cost == Config::DOUBLE_INFINITY);
        // Insert customer into list of unrouted customers
        solution.unroutedCustomers.push_back(solution.getE2Routes()[rt].tour[rc]);

        // Remove route if it contains only one customer
        if (solution.getE2Routes()[rt].tour.size() == 1) {
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    solution.getE2Routes()[rt].tour[rc]).getDemand();
            // Update assigned routes
            solution.satelliteAssignedRoutes[solution.getE2Routes()[rt].departureSatellite]--;
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

// Todo normalize distances
void IDCH::doWorstRemoval(Solution &solution, double p2) {
    if (solution.getE2Routes().size()==0)
        return;
    int n = problem->numberOfClients();
    // nc : number of customers to remove
    int nc = static_cast<int>(lround(p2 * n));
    n -= solution.unroutedCustomers.size();
    nc = min(nc, n);
    int rt, // Route number
            rc;  // Customer customer

    double cost, tmpCost;

    int i = 0;
    while (i < nc) {
        cost = 0;
        rt = -1;
        rc = -1;
        // Find a customer to remove
        for (int u = 0; u < solution.getE2Routes().size(); ++u) {
            E2Route &route = solution.getE2Routes()[u];
            for (int v = 0; v < route.tour.size(); ++v) {
                //tmpCost = removalCost(solution, v, u)/averageArcCost[route.tour[v]];
                tmpCost = removalCost(solution, v, u);
                if (tmpCost < cost) {
                    rt = u;
                    rc = v;
                    cost = tmpCost;
                }

            }
        }
        // If no customer can be removed, then stop removal
        if (rt == -1 && rc == -1)
            return;
        // Insert customer into list of unrouted customers
        solution.unroutedCustomers.push_back(solution.getE2Routes()[rt].tour[rc]);

        // Remove route if it contains only one customer
        if (solution.getE2Routes()[rt].tour.size() == 1) {
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    solution.getE2Routes()[rt].tour[rc]).getDemand();
            // Update assigned routes
            solution.satelliteAssignedRoutes[solution.getE2Routes()[rt].departureSatellite]--;
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

struct Neighbor {
    int id;
    int rt;
    int rc;
    double distance;
};

bool neighborSort(Neighbor x, Neighbor y) { return x.distance < y.distance; }

void IDCH::doRelatedRemoval(Solution &solution, double p3) {
    if (solution.getE2Routes().size()==0)
        return;
    int n = problem->numberOfClients();
    // nc : number of customers to remove
    int nc = static_cast<int>(lround(p3 * n));
    n -= solution.unroutedCustomers.size();
    nc = min(nc, n);
    int rt, // Route number
            rc;  // Customer customer

    double cost;

    // Chose a seed customer
    int seedRt = Utility::randomInt(0, solution.getE2Routes().size());
    int seedPos = Utility::randomInt(0, solution.getE2Routes()[seedRt].tour.size());
    Client seed = problem->getClient(solution.getE2Routes()[seedRt].tour[seedPos]);

    // Construct the list L containing the neighbors of seed
    vector<Neighbor> L;
    L.push_back(Neighbor{seed.getClientId(), seedRt, seedPos, 0});
    for (int j = 0; j < solution.getE2Routes().size(); ++j) {
        for (int k = 0; k < solution.getE2Routes()[j].tour.size(); ++k) {
            int id = solution.getE2Routes()[j].tour[k];
            if (id == seed.getClientId()) continue;
            L.push_back(Neighbor{id, j, k, problem->getDistance(seed, problem->getClients()[id])});
        }
    }
    // Sort L by increasing distance to seed
    std::sort(L.begin() + 1, L.end(), neighborSort);

    int i = 0;
    int removed = 0;
    while (removed < nc && i < L.size()) {
        // Find a customer to remove
        rt = L[i].rt;
        rc = L[i].rc;
        // Compute removal cost
        cost = removalCost(solution, rc, rt);

        if (cost == Config::DOUBLE_INFINITY) {
            i++;
            continue;
        }
        // Insert customer into list of unrouted customers
        solution.unroutedCustomers.push_back(L[i].id);

        // Remove route if it contains only one customer
        if (solution.getE2Routes()[rt].tour.size() == 1) {
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    L[i].id).getDemand();
            // Update assigned routes
            solution.satelliteAssignedRoutes[solution.getE2Routes()[rt].departureSatellite]--;
            // remove route
            solution.getE2Routes().erase(solution.getE2Routes().begin() + rt);
            // Update value of rt in L entries
            for (int j = i + 1; j < nc; ++j) {
                if (L[j].rt > rt) L[j].rt--;
            }
        }
        else {
            // Update route cost
            solution.getE2Routes()[rt].cost += cost;
            // Update route load
            solution.getE2Routes()[rt].load -= problem->getClient(L[i].id).getDemand();
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    L[i].id).getDemand();
            // Remove the customer from the route it is in
            solution.getE2Routes()[rt].tour.erase(solution.getE2Routes()[rt].tour.begin() + rc);
            // Update value of rc in L entries
            for (int j = i + 1; j < nc; ++j) {
                if (L[j].rt == rt && L[j].rc > rc) L[j].rc--;
            }
        }
        // Update solution cost
        solution.setTotalCost(solution.getTotalCost() + cost);
        // Update number of removed customers
        removed++;
        // Go to the next customer
        i++;
    }
}

void IDCH::doRouteRemoval(Solution &solution, int p4) {
    if (solution.getE2Routes().size()==0)
        return;

    vector<E2Route> &routes = solution.getE2Routes();
    int size = routes.size();
    // nr : number of routes to remove
    int nr = Utility::randomInt(0, 1 + min(p4, size));
    // index of the route to be removed
    int r;
    while (nr > 0 && routes.size() > 0) {
        r = Utility::randomInt(0, routes.size());
        // Update solution cost
        solution.setTotalCost(solution.getTotalCost() - routes[r].cost);
        // Update satellite demand
        solution.getSatelliteDemands()[routes[r].departureSatellite] -= routes[r].load;
        // Insert customers into pool
        for (int i = 0; i < routes[r].tour.size(); ++i) {
            solution.unroutedCustomers.push_back(routes[r].tour[i]);
        }
        // Update number of routes assigned to satellite
        solution.satelliteAssignedRoutes[routes[r].departureSatellite]--;
        // Remove route
        routes.erase(routes.begin() + r);
        nr--;
    }

}

void IDCH::doRemoveSingleNodeRoutes(Solution &solution) {
    if (solution.getE2Routes().size()==0)
        return;

    int i = 0;
    while (i < solution.getE2Routes().size()) {
        E2Route &route = solution.getE2Routes()[i];
        // If it's a single customer trip
        if (route.tour.size() == 1) {
            // Insert customer into the unrouted customer's pool
            solution.unroutedCustomers.push_back(route.tour[0]);
            // Update satellite demand
            solution.getSatelliteDemands()[route.departureSatellite] -= route.load;
            // Update solution cost
            solution.setTotalCost(solution.getTotalCost() - route.cost);
            // Update assigned routes
            solution.satelliteAssignedRoutes[solution.getE2Routes()[i].departureSatellite]--;
            // remove route
            solution.getE2Routes().erase(solution.getE2Routes().begin() + i);
        } else i++;
    }
}

void IDCH::doSatelliteRemoval(Solution &solution) {
    if (solution.getE2Routes().size()==0)
        return;

    if (solution.openSatellites <= (problem->getK2()/problem->getMaxCf()+ ((problem->getK2()%problem->getMaxCf())? 1 : 0) )) return;
    // Chose the satellite to close
    int sat;
    do {
        sat = Utility::randomInt(0, solution.satelliteState.size());
    } while (solution.satelliteState[sat] == Solution::CLOSED ||
             solution.satelliteAssignedRoutes[sat]==0);
    // Remove all routes originating from the chosen satellite
    vector<E2Route> &routes = solution.getE2Routes();
    int r = 0;
    while (r < routes.size()) {
        // If the route originates from sat, then remove it
        if (routes[r].departureSatellite == sat) {
            // Update solution cost
            solution.setTotalCost(solution.getTotalCost() - routes[r].cost);
            // Insert customers into pool
            for (int i = 0; i < routes[r].tour.size(); ++i) {
                solution.unroutedCustomers.push_back(routes[r].tour[i]);
            }
            // Remove route
            routes.erase(routes.begin() + r);
        }
            // Else ignore it
        else r++;
    }
    // Update satellite demand
    solution.getSatelliteDemands()[sat] = 0;
    // Update routes assigned to satellite
    solution.satelliteAssignedRoutes[sat] = 0;
    // Close satellite
    solution.satelliteState[sat] = Solution::CLOSED;
    solution.openSatellites--;
    // Open another satellite if they are all closed
    if (solution.openSatellites == 0) {
        sat = Utility::randomInt(0, solution.satelliteState.size());
        solution.satelliteState[sat] = Solution::OPEN;
        solution.openSatellites = 1;
    }

}

void IDCH::doSatelliteOpening(Solution &solution, double p7) {
    int n = problem->numberOfClients();
    // nc : number of customers to remove
    int nc = static_cast<int>(lround(p7 * n));
    n -= solution.unroutedCustomers.size();
    nc = min(nc, n);

    int rt, // Route number
            rc;  // Customer customer

    double cost;

    // Chose a satellite to open
    int satId;
    do {
        satId = Utility::randomInt(0, solution.satelliteState.size());
    } while (solution.satelliteState[satId] == Solution::OPEN);

    Satellite sat = problem->getSatellite(satId);

    // Open the satellite
    solution.satelliteState[satId] = Solution::OPEN;
    solution.openSatellites++;
    // Construct the list L containing the neighbors of sat
    vector<Neighbor> L;
    for (int j = 0; j < solution.getE2Routes().size(); ++j) {
        for (int k = 0; k < solution.getE2Routes()[j].tour.size(); ++k) {
            int id = solution.getE2Routes()[j].tour[k];
            L.push_back(Neighbor{id, j, k, problem->getDistance(sat, problem->getClients()[id])});
        }
    }
    // Sort L by increasing distance to sat
    std::sort(L.begin(), L.end(), neighborSort);

    // Remove the nc customers closest to sat
    int i = 0;
    while (i < nc && i < n) {
        // Find a customer to remove
        rt = L[i].rt;
        rc = L[i].rc;
        // Compute removal cost
        cost = removalCost(solution, rc, rt);
        // Insert customer into list of unrouted customers
        solution.unroutedCustomers.push_back(L[i].id);

        // Remove route if it contains only one customer
        if (solution.getE2Routes()[rt].tour.size() == 1) {
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    L[i].id).getDemand();
            // Update assigned routes
            solution.satelliteAssignedRoutes[solution.getE2Routes()[rt].departureSatellite]--;
            // remove route
            solution.getE2Routes().erase(solution.getE2Routes().begin() + rt);
            // Update value of rt in L entries
            for (int j = i + 1; j < nc; ++j) {
                if (L[j].rt > rt) L[j].rt--;
            }
        }
        else {
            // Update route cost
            solution.getE2Routes()[rt].cost += cost;
            // Update route load
            solution.getE2Routes()[rt].load -= problem->getClient(L[i].id).getDemand();
            // Update satellite demand
            solution.getSatelliteDemands()[solution.getE2Routes()[rt].departureSatellite] -= problem->getClient(
                    L[i].id).getDemand();
            // Remove the customer from the route it is in
            solution.getE2Routes()[rt].tour.erase(solution.getE2Routes()[rt].tour.begin() + rc);
            // Update value of rc in L entries
            for (int j = i + 1; j < nc; ++j) {
                if (L[j].rt == rt && L[j].rc > rc) L[j].rc--;
            }
        }
        // Go to the next customer
        i++;
    }
    solution.doQuickEvaluation();
}

void IDCH::doOpenAllSatellites(Solution &solution) {
    std::fill(solution.satelliteState.begin(), solution.satelliteState.end(), Solution::OPEN);
    solution.openSatellites = solution.satelliteState.size();
}

/****************************************************
 *  
 *           PERTURBATION AND LOCAL SEARCH
 *  
 ****************************************************/
bool IDCH::apply2OptOnEachTour(Solution &solution) {
    bool improvement = false;
    double imp = 0;
    for (E2Route &route : solution.getE2Routes()) {
        improvement = improvement || lsSolver.apply2OptOnTour(route);
    }
    return improvement;
}

/****************************************************
 *
 *        Destroy and Repair for IDCH
 *
 ****************************************************/
void IDCH::doDestroyLarge(Solution &solution) {
    /* Todo 1 changer les paramètres
     * Todo 2 implémenter un schéma pour l'utilisation des opérateurs
    */
    this->doRandomRemoval(solution, 0.15);
    this->doWorstRemoval(solution, 0.35);
    this->doRelatedRemoval(solution, 0.1);
    this->doRouteRemoval(solution, 5);
    this->doRemoveSingleNodeRoutes(solution);

    double p = Utility::randomDouble(0,1);
    //if(p<Config::p5) this->doRemoveSingleNodeRoutes(solution);
    //p = Utility::randomDouble(0,1);
    if(p<Config::p6) this->doSatelliteRemoval(solution);
    //this->doSatelliteOpening(solution, 0.1);
    p = Utility::randomDouble(0,1);
    if(p<Config::p7) this->doOpenAllSatellites(solution);
}

void IDCH::doDestroySmall(Solution &solution) {
/* Todo 1 changer les paramètres
     * Todo 2 implémenter un schéma pour l'utilisation des opérateurs
    */
    this->doRandomRemoval(solution, Config::p1);
    this->doWorstRemoval(solution, Config::p2);
    this->doRelatedRemoval(solution, Config::p3);
    this->doRouteRemoval(solution, Config::p4);
    /*double p = Utility::randomDouble(0,1);
    if(p<0.2) this->doRemoveSingleNodeRoutes(solution);
    p = Utility::randomDouble(0,1);
    if(p<0.18) this->doSatelliteRemoval(solution, 0);
    //this->doSatelliteOpening(solution, 0.1);
    p = Utility::randomDouble(0,1);
    if(p<0.05) this->doOpenAllSatellites(solution);*/
    /******************************/
    /*int choice = Utility::randomInt(0, 4);
    switch (choice) {
        case 0:
            this->doRandomRemoval(solution, 0.3);
            break;
        case 1:
            this->doWorstRemoval(solution, 0.4);
            break;
        case 2:
            this->doRouteRemoval(solution, 5);
            break;
        case 3:
            this->doRemoveSingleNodeRoutes(solution);
            break;
        default:
            this->doRandomRemoval(solution, 0.4);
            break;
    }*/
}

// Todo : implement a repair method
void IDCH::doRepair(Solution &solution) {
    //this->insertion.solveBestInsertion(solution);
    this->doGreedyInsertion(solution);
    //this->doGreedyInsertionPerturbation(solution);
}

/****************************************************
 *  
 *                OTHER METHODS
 *  
 ****************************************************/
double IDCH::removalCost(Solution &solution, int customer, int route) {

    if (solution.getE2Routes()[route].tour.size() == 1)
        return -solution.getE2Routes()[route].cost;

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

    if (problem->getDistance(p, q) == Config::DOUBLE_INFINITY)
        return Config::DOUBLE_INFINITY;
    else return problem->getDistance(p, q) - (problem->getDistance(p, c) + problem->getDistance(c, q));
}

// Todo Implement Local Search Step for IDCH
bool IDCH::doLocalSearch(Solution &solution) {

    /*double imp;
    do {
        imp = solution.getTotalCost();
        lsSolver.apply2optStar(solution);
        lsSolver.applyRelocate(solution);
        lsSolver.applySwap(solution);
        for (E2Route &e2route : solution.getE2Routes()) {
            solution.setTotalCost(solution.getTotalCost() - e2route.cost);
            lsSolver.apply2OptOnTour(e2route);
            solution.setTotalCost(solution.getTotalCost() + e2route.cost);
        }
        //lsSolver.doChangeSatellite(solution);
        imp = solution.getTotalCost() - imp;
    } while (imp < -0.01);

    this->doRepair(solution);

    return true;*/

    bool improvement = false;

    int i, selected,
            nneighbor = 4,
            nusage[] = {0, 0, 0, 0},
            nusagemax[] = {1, 1, 1, 1};
    bool stop = false;

    while (!stop) {
        do { selected = rand() % nneighbor; } while (nusage[selected] >= nusagemax[selected]);

        switch (selected) {
            case 0:
                if (this->apply2OptOnEachTour(solution)) {
                    for (i = 0; i < nneighbor; i++) nusage[i] = 0;
                    improvement = true;
                } else {
                    nusage[selected]++;
                }
                break;

            case 1:
                if (lsSolver.apply2optStar(solution)) {
                    for (i = 0; i < nneighbor; i++) nusage[i] = 0;
                    improvement = true;
                } else {
                    nusage[selected]++;
                }
                break;

            case 2:
                if (lsSolver.applyRelocate(solution)) {
                    for (i = 0; i < nneighbor; i++) nusage[i] = 0;
                    improvement = true;
                } else {
                    nusage[selected]++;
                }
                break;

            case 3:
                if (lsSolver.applySwap(solution)) {
                    for (i = 0; i < nneighbor; i++) nusage[i] = 0;
                    improvement = true;
                } else {
                    nusage[selected]++;
                }
                break;
           /* case 4:
                if (lsSolver.doChangeSatellite(solution)) {
                    for (i = 0; i < nneighbor; i++) nusage[i] = 0;
                    improvement = true;
                } else {
                    nusage[selected]++;
                }
                break;*/

            default:
                break;
        }

        for (i = 0; i < nneighbor; i++) {
            if (nusage[i] < nusagemax[i]) break;
        }

        solution.doQuickEvaluation();

        if (i >= nneighbor) stop = true;
    }

    if (improvement) this->doRepair(solution);

    return improvement;
}

