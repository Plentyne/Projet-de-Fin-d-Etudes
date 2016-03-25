//
// Created by Youcef on 10/03/2016.
//

#include "SDVRPSolver.h"
#include "../Config.h"
#include <vector>
#include <algorithm>

// Todo implémenter routeAngleMechanism
double routeAngleMechanism() {
    return 0;
}

struct L_Entry {
    int id;
    double c;
};

bool sortingCriterion(L_Entry i, L_Entry j) {
    return (i.c < j.c);
}

/******************************
 * Constructive Heuristic
 ******************************/
void SDVRPSolver::constructiveHeuristic(Solution &solution) {
    // List containig the clients
    vector<L_Entry> L; //= vector<L_Entry>(solution.getProblem()->getSatellites().size());
    // List containing the demands of the customers
    const vector<int> Q = solution.getSatelliteDemands();
    // List containing the unserved demand of each customer
    vector<int> U = solution.getSatelliteDemands();
    // List containing customer angles (used for The Route Angle Mechanism)
    vector<double> angles = vector<double>(solution.getProblem()->getSatellites().size());

    // Sort customers by their distance to the depot
    L_Entry l;
    for (int j = 0; j < solution.getSatelliteDemands().size(); ++j) {
        if (solution.getSatelliteDemands()[j] == 0) continue;
        l.id = j;
        l.c = problem->getDistance(problem->getDepot(), problem->getSatellite(j));
        L.push_back(l);
    }
    std::sort(L.begin(), L.end(), sortingCriterion);
    // While there are customers that haven't been served
    int c = 0;
    double cost, minCost;
    int insertRoute, insertPos;
    Node p, q;
    Satellite i;
    while (c < L.size()) {
        // Take the first customer i in L
        i = problem->getSatellite(L[c].id);
        minCost = Config::DOUBLE_INFINITY;
        // Compute the cheapest insertion cost for i. Cost is obtained by adding distance cost and route angle mechanism
        for (int r = 0; r < solution.getE1Routes().size(); ++r) {
            E1Route &e1Route = solution.getE1Routes()[r];
            if (e1Route.load == problem->getE1Capacity()) continue;
            for (int k = 0; k <= e1Route.tour.size(); ++k) {
                if (k == 0) p = problem->getDepot();
                else p = problem->getSatellite(e1Route.tour[k - 1]);

                if (k == e1Route.tour.size()) q = problem->getDepot();
                else q = problem->getSatellite(e1Route.tour[k]);

                cost = problem->getDistance(p, i) + problem->getDistance(i, q) - problem->getDistance(p, q) +
                       routeAngleMechanism();

                if (cost < minCost) {
                    minCost = cost;
                    insertPos = k;
                    insertRoute = r;
                }
            }
        }
        // If the cheapest cost is lower than the cost of a returnig route, insert i int route r yielding the lowest cost.
        if (minCost < 2 * problem->getDistance(problem->getDepot(), i)) {
            E1Route &route = solution.getE1Routes()[insertRoute];
            route.cost += minCost;
            route.tour.insert(route.tour.begin() + insertPos, i.getSatelliteId());
            // Compute the quantity supplied to i by route r : if Sr > U[i] then Sr = Sr-U[i] and U[i]=0, else Sr = 0 and U[i]=U[i]-Sr (split occurs)
            if (U[i.getSatelliteId()] < problem->getE1Capacity() - route.load) {
                route.load += U[i.getSatelliteId()];
                route.satelliteGoods.insert(route.satelliteGoods.begin() + insertPos, U[i.getSatelliteId()]);
                U[i.getSatelliteId()] = 0;
            }
            else {
                route.satelliteGoods.insert(route.satelliteGoods.begin() + insertPos,
                                            problem->getE1Capacity() - route.load);
                U[i.getSatelliteId()] -= (problem->getE1Capacity() - route.load);
                route.load = problem->getE1Capacity();
            }
            // Optimize route using 2-opt move
            double dCost = apply2Opt(route);
            // Update solution cost
            solution.setTotalCost(solution.getTotalCost() + minCost + dCost);
        }
            // Else, initiate a new route
        else {
            E1Route route;
            route.cost = 2 * problem->getDistance(problem->getDepot(), i);
            route.tour.push_back(i.getSatelliteId());
            // Compute the quantity supplied to i by route r : if Sr > U[i] then Sr = Sr-U[i] and U[i]=0, else Sr = 0 and U[i]=U[i]-Sr (split occurs)
            if (U[i.getSatelliteId()] < problem->getE1Capacity()) {
                route.load = U[i.getSatelliteId()];
                route.satelliteGoods.push_back(U[i.getSatelliteId()]);
                U[i.getSatelliteId()] = 0;
            }
            else {
                route.satelliteGoods.push_back(problem->getE1Capacity());
                U[i.getSatelliteId()] -= (problem->getE1Capacity());
                route.load = problem->getE1Capacity();
            }
            // Insert route into solution
            solution.getE1Routes().push_back(route);
            // Update solution cost
            solution.setTotalCost(solution.getTotalCost() + route.cost);
        }
        // Todo enlever après
        this->applyRelocate(solution);
        // If U[i] is fully supplied, go to the next customer in L
        if (U[i.getSatelliteId()] == 0) c++;
    }
    // End While

    // Improve solution with local search Todo
    // this->applyRelocate(solution);
    // Update satellite demands
    for (int m = 0; m < solution.getSatelliteDemands().size(); ++m) {
        solution.getDeliveredQ()[m] = Q[m] - U[m];
    }

}

/******************************
 * LOCAL SEARCH NEIGHBORHOODS
 ******************************/
double SDVRPSolver::apply2Opt(E1Route &route) {
    if (route.tour.size() <= 2) return 0;

    double improved, minImproved;
    double oldCost = route.cost;
    int u, v, i, j;
    double dxu, dvy, dxv, duy;

    do {
        minImproved = 0;
        i = j = 0;

        for (u = 0; u < route.tour.size(); u++) { // inner loop
            if (u == 0)
                dxu = this->problem->getDistance(this->problem->getDepot(),
                                                 this->problem->getSatellite(route.tour[u]));
            else
                dxu = this->problem->getDistance(this->problem->getSatellite(route.tour[u - 1]),
                                                 this->problem->getSatellite(route.tour[u]));

            for (v = u + 1; v < route.tour.size(); v++) {
                if (v == route.tour.size() - 1)
                    dvy = this->problem->getDistance(this->problem->getSatellite(route.tour[v]),
                                                     this->problem->getDepot());
                else
                    dvy = this->problem->getDistance(this->problem->getSatellite(route.tour[v]),
                                                     this->problem->getSatellite(route.tour[v + 1]));

                if (u == 0)
                    dxv = this->problem->getDistance(this->problem->getDepot(),
                                                     this->problem->getSatellite(route.tour[v]));
                else
                    dxv = this->problem->getDistance(this->problem->getSatellite(route.tour[u - 1]),
                                                     this->problem->getSatellite(route.tour[v]));

                if (v == route.tour.size() - 1)
                    duy = this->problem->getDistance(this->problem->getSatellite(route.tour[u]),
                                                     this->problem->getDepot());
                else
                    duy = this->problem->getDistance(this->problem->getSatellite(route.tour[u]),
                                                     this->problem->getSatellite(route.tour[v + 1]));

                improved = dxv + duy - (dxu + dvy);

                if (improved < minImproved) {
                    minImproved = improved;
                    i = u;
                    j = v;
                }
            }
        }
        // inverse sequence order between u and v
        std::reverse(route.tour.begin() + i, route.tour.begin() + j + 1);
        std::reverse(route.satelliteGoods.begin() + i, route.satelliteGoods.begin() + j + 1);
        route.cost += minImproved;
    } while (minImproved < 0);

    return (route.cost - oldCost);
}

double SDVRPSolver::applyOrOpt(E1Route &route, int seqLength) {
    if (seqLength < 1) return 0;
    if (route.tour.size() <= 2) return 0;
    if (route.tour.size() <= seqLength) return 0;

    vector<int> tmpTour;

    int u, v;
    int position;
    int seqBegin;
    double delta = 0;
    double bestDelta;
    double dRemoval, dInsertion;
    double oldCost = route.cost;
    Node p, q, i, j, k, k_1;
    // Repeat
    do {

        bestDelta = 0;
        position = -1;

        // For Each vertex of the route
        for (u = 0; u <= route.tour.size() - seqLength; ++u) {
            // Save the first and last elements (respectively i and j) of the sequence being tried
            v = u + seqLength - 1;
            i = this->problem->getSatellite(route.tour[u]);
            j = this->problem->getSatellite(route.tour[v]);
            tmpTour = route.tour;
            tmpTour.erase(tmpTour.begin() + u, tmpTour.begin() + u + seqLength);
            // Compute the change of cost caused by deleting sequence i-> ... ->j : dRemoval
            // Removed arcs are : (p=i-1,i),(j,q=j+1).  New arc is (i-1,j+1)
            // dRemoval = c(p,q) - ( c(p,i) + c(j,q) )
            if (u == 0)
                p = this->problem->getDepot();
            else
                p = this->problem->getSatellite(route.tour[u - 1]);

            if (v == route.tour.size() - 1)
                q = this->problem->getDepot();
            else
                q = this->problem->getSatellite(route.tour[v + 1]);

            dRemoval = this->problem->getDistance(p, q) -
                       (this->problem->getDistance(p, i) + this->problem->getDistance(j, q));

            // For Each position k in tmpTour
            for (int w = 0; w <= tmpTour.size(); ++w) {

                if (w == u) continue;
                // Compute the cost of inserting i->..->j at position k : dInsertion
                // Removed arc is (k-1,k). New arcs are (k-1,i) and (j,k)
                // dInsertion = c(k-1,i)+c(j,k)-c(k-1,k)
                if (w == 0) {
                    k_1 = this->problem->getDepot();
                    k = this->problem->getSatellite(tmpTour[w]);
                }
                else if (w == tmpTour.size()) {
                    k_1 = this->problem->getSatellite(tmpTour[w - 1]);
                    k = this->problem->getDepot();
                }
                else {
                    k_1 = this->problem->getSatellite(tmpTour[w - 1]);
                    k = this->problem->getSatellite(tmpTour[w]);
                }

                dInsertion = this->problem->getDistance(k_1, i) + this->problem->getDistance(j, k) -
                             this->problem->getDistance(k_1, k);

                // The overall change is defined as : delta = dRemoval + dInsertion
                delta = dRemoval + dInsertion;
                // If ( delta < 0 ) there is improvement
                if (delta < bestDelta) {
                    // Save the insert position and the change in cost
                    bestDelta = delta;
                    position = w;
                    seqBegin = u;
                }

            }// End For Each
        }// End For Each

        // Update the route
        if (position >= 0) {
            // Update route sequence
            vector<int> sequence(route.tour.begin() + seqBegin, route.tour.begin() + seqBegin + seqLength);
            route.tour.erase(route.tour.begin() + seqBegin, route.tour.begin() + seqBegin + seqLength);
            route.tour.insert(route.tour.begin() + position, sequence.begin(), sequence.end());
            // Update goods
            vector<int> goods(route.satelliteGoods.begin() + seqBegin,
                              route.satelliteGoods.begin() + seqBegin + seqLength);
            route.satelliteGoods.erase(route.satelliteGoods.begin() + seqBegin,
                                       route.satelliteGoods.begin() + seqBegin + seqLength);
            route.satelliteGoods.insert(route.satelliteGoods.begin() + position, goods.begin(), goods.end());
            route.cost += bestDelta;
        }

    } while (delta < 0); // While there is improvement

    return (route.cost - oldCost);
}

double SDVRPSolver::applyRelocate(Solution &solution) {
    vector<E1Route> &routes = solution.getE1Routes();

    bool improvement;
    int position;
    int insertRoute;
    double dRemoval, dInsertion;
    double bestRemoval, bestInsertion;
    double oldCost = solution.getTotalCost();
    do {
        improvement = false;
        bestInsertion = 0;
        bestRemoval = 0;

        for (int i = 0; i < routes.size() && !improvement; ++i) {
            for (int j = 0; j < routes[i].tour.size() && !improvement; ++j) {

                Satellite s = problem->getSatellite(routes[i].tour[j]);

                Node p;
                if (j == 0)
                    p = this->problem->getDepot();
                else
                    p = this->problem->getSatellite(routes[i].tour[j - 1]);

                Node q;
                if (j == routes[i].tour.size() - 1)
                    q = this->problem->getDepot();
                else
                    q = this->problem->getSatellite(routes[i].tour[j + 1]);

                dRemoval = this->problem->getDistance(p, q) -
                           (this->problem->getDistance(p, s) + this->problem->getDistance(s, q));

                for (int u = 0; u < routes.size(); ++u) {
                    // If it's the same tour
                    if (u == i) {
                        continue;
                        // Todo Act like Or Opt
                    }
                        // Else
                    else {
                        // if there's not enough free space, then ignore the route
                        if (routes[u].load + routes[i].satelliteGoods[j] > problem->getE1Capacity()) continue;
                        for (int v = 0; v <= routes[u].tour.size(); ++v) {
                            // If it's the same satellite
                            if (routes[u].tour[v] == routes[i].tour[j]) {
                                bestInsertion = 0;
                                bestRemoval = dRemoval;
                                position = v;
                                insertRoute = u;
                                break;
                            }
                            else {
                                // Compute insertion cost
                                Node k, k_1;
                                if (v == 0) {
                                    k_1 = this->problem->getDepot();
                                    k = this->problem->getSatellite(routes[u].tour[v]);
                                }
                                else if (v == routes[u].tour.size()) {
                                    k_1 = this->problem->getSatellite(routes[u].tour[v - 1]);
                                    k = this->problem->getDepot();
                                }
                                else {
                                    k_1 = this->problem->getSatellite(routes[u].tour[v - 1]);
                                    k = this->problem->getSatellite(routes[u].tour[v]);
                                }
                                dInsertion = this->problem->getDistance(k_1, s) + this->problem->getDistance(s, k) -
                                             this->problem->getDistance(k_1, k);


                                if (dRemoval + dInsertion < bestRemoval + bestInsertion) {
                                    // Save the insert position and the change in cost
                                    bestInsertion = dInsertion;
                                    bestRemoval = dRemoval;
                                    position = v;
                                    insertRoute = u;
                                }
                            }
                        }

                    }

                }

                // Update the route
                if (bestRemoval + bestInsertion < -0.0001) {
                    improvement = true;
                    // If customer exists in the route, only move its goods
                    if (routes[i].tour[j] == routes[insertRoute].tour[position]) {
                        routes[insertRoute].load += routes[i].satelliteGoods[j];
                        routes[insertRoute].satelliteGoods[position] += routes[i].satelliteGoods[j];
                    }
                        // Else insert it into the new route
                    else {
                        // update load
                        routes[insertRoute].load += routes[i].satelliteGoods[j];
                        // update cost
                        routes[insertRoute].cost += bestInsertion;
                        // Update tour
                        routes[insertRoute].tour.insert(routes[insertRoute].tour.begin() + position, routes[i].tour[j]);
                        // Update satelliteGoods
                        routes[insertRoute].satelliteGoods.insert(routes[insertRoute].satelliteGoods.begin() + position,
                                                                  routes[i].satelliteGoods[j]);
                    }
                    // Remove customer from irs original route
                    if (routes[i].tour.size() == 1) {
                        solution.getE1Routes().erase(solution.getE1Routes().begin() + i);
                    }
                    else {
                        routes[i].cost += bestRemoval;
                        routes[i].load -= routes[i].satelliteGoods[j];
                        routes[i].tour.erase(routes[i].tour.begin() + j);
                        routes[i].satelliteGoods.erase(routes[i].satelliteGoods.begin() + j);
                    }

                    // Update solution cost
                    solution.setTotalCost(solution.getTotalCost() + bestRemoval + bestInsertion);
                }
            }
        }
    }
    while (improvement);

    return oldCost - solution.getTotalCost();
}

// Todo corriger le swap, il ne fonctionne pas correctement
double SDVRPSolver::applySwap(Solution &solution) {
    vector<E1Route> &routes = solution.getE1Routes();

    bool improvement;
    int xPosition, yPosition;
    int insertRoute;
    double xRemoval, xInsertion;
    double yRemoval, yInsertion;
    double bestXRemoval, bestXInsertion;
    double bestYRemoval, bestYInsertion;
    double oldCost = solution.getTotalCost();
    Node p, q, k, k_1, x, y;
    do {
        improvement = false;
        for (int i = 0; i < routes.size() && !improvement; ++i) {
            if (routes[i].tour.size() == 1) continue;
            for (int j = 0; j < routes[i].tour.size() && !improvement; ++j) {

                x = problem->getSatellite(routes[i].tour[j]);

                if (j == 0)
                    p = this->problem->getDepot();
                else
                    p = this->problem->getSatellite(routes[i].tour[j - 1]);

                if (j == routes[i].tour.size() - 1)
                    q = this->problem->getDepot();
                else
                    q = this->problem->getSatellite(routes[i].tour[j + 1]);

                xRemoval = this->problem->getDistance(p, q) -
                           (this->problem->getDistance(p, x) + this->problem->getDistance(x, q));

                // Remove customer x form route
                E1Route routeX;
                routeX.cost = routes[i].cost + yRemoval;
                routeX.load = routes[i].load - routes[i].satelliteGoods[j];
                routeX.tour = vector<int>(routes[i].tour.begin(), routes[i].tour.begin() + j);
                routeX.tour.insert(routeX.tour.end(), routes[i].tour.begin() + j + 1, routes[i].tour.end());
                routeX.satelliteGoods = vector<int>(routes[i].satelliteGoods.begin(),
                                                    routes[i].satelliteGoods.begin() + j);
                routeX.satelliteGoods.insert(routeX.satelliteGoods.end(), routes[i].satelliteGoods.begin() + j + 1,
                                             routes[i].satelliteGoods.end());


                for (int u = i + 1; u < routes.size() && !improvement; ++u) {
                    // If route u only containes oneit's the same tour
                    if (routes[u].tour.size() == 1) {
                        continue;
                        // Todo Act like Or Opt
                    }
                        // Else
                    else {
                        // For each customer v of route u
                        for (int v = 0; v <= routes[u].tour.size() && !improvement; ++v) {
                            // If it's the same satellite, then ignore it
                            if (routes[u].tour[v] == routes[i].tour[j]) {
                                continue;
                            }
                            else {
                                // if there isn't enough space for exchanging the customers
                                if (routes[i].satelliteGoods[j] >
                                    problem->getE1Capacity() + routes[u].satelliteGoods[v] - routes[u].load
                                    || routes[u].satelliteGoods[v] >
                                       problem->getE1Capacity() + routes[i].satelliteGoods[j] - routes[i].load) {
                                    continue;
                                }

                                // Remove customer y from route u
                                y = problem->getSatellite(routes[u].tour[v]);

                                if (v == 0)
                                    p = this->problem->getDepot();
                                else
                                    p = this->problem->getSatellite(routes[u].tour[v - 1]);

                                if (v == routes[u].tour.size() - 1)
                                    q = this->problem->getDepot();
                                else
                                    q = this->problem->getSatellite(routes[u].tour[v + 1]);

                                yRemoval = this->problem->getDistance(p, q) -
                                           (this->problem->getDistance(p, y) + this->problem->getDistance(y, q));

                                // Remove customer x form route
                                E1Route routeY;
                                routeY.cost = routes[u].cost + yRemoval;
                                routeY.load = routes[u].load - routes[u].satelliteGoods[v];
                                routeY.tour = vector<int>(routes[u].tour.begin(), routes[u].tour.begin() + v);
                                routeY.tour.insert(routeY.tour.end(), routes[u].tour.begin() + v + 1,
                                                   routes[u].tour.end());
                                routeY.satelliteGoods = vector<int>(routes[u].satelliteGoods.begin(),
                                                                    routes[u].satelliteGoods.begin() + v);
                                routeY.satelliteGoods.insert(routeY.satelliteGoods.end(),
                                                             routes[u].satelliteGoods.begin() + v + 1,
                                                             routes[u].satelliteGoods.end());
                                // Find best insertion for customer x in route routeY
                                bestXInsertion = Config::DOUBLE_INFINITY;
                                for (int l = 0; l <= routeY.tour.size(); ++l) {
                                    if (l < routeY.tour.size() && routeY.tour[l] == routes[i].tour[j]) {
                                        bestXInsertion = 0;
                                        xPosition = l;
                                        break;
                                    }
                                    else {
                                        if (l == 0) {
                                            k_1 = this->problem->getDepot();
                                            k = this->problem->getSatellite(routeY.tour[l]);
                                        }
                                        else if (v == routeY.tour.size()) {
                                            k_1 = this->problem->getSatellite(routeY.tour[l - 1]);
                                            k = this->problem->getDepot();
                                        }
                                        else {
                                            k_1 = this->problem->getSatellite(routeY.tour[l - 1]);
                                            k = this->problem->getSatellite(routeY.tour[l]);
                                        }
                                        xInsertion =
                                                this->problem->getDistance(k_1, x) + this->problem->getDistance(x, k) -
                                                this->problem->getDistance(k_1, k);

                                        if (xInsertion < bestXInsertion) {
                                            bestXInsertion = xInsertion;
                                            xPosition = l;
                                        }
                                    }
                                }
                                // Find best insertion for customer y in route X
                                bestYInsertion = Config::DOUBLE_INFINITY;
                                for (int l = 0; l <= routeX.tour.size(); ++l) {
                                    if (l < routeY.tour.size() && routeX.tour[l] == routes[u].tour[v]) {
                                        bestYInsertion = 0;
                                        yPosition = l;
                                        break;
                                    }
                                    else {
                                        if (l == 0) {
                                            k_1 = this->problem->getDepot();
                                            k = this->problem->getSatellite(routeX.tour[l]);
                                        }
                                        else if (v == routeX.tour.size()) {
                                            k_1 = this->problem->getSatellite(routeX.tour[l - 1]);
                                            k = this->problem->getDepot();
                                        }
                                        else {
                                            k_1 = this->problem->getSatellite(routeX.tour[l - 1]);
                                            k = this->problem->getSatellite(routeX.tour[l]);
                                        }
                                        yInsertion =
                                                this->problem->getDistance(k_1, y) + this->problem->getDistance(y, k) -
                                                this->problem->getDistance(k_1, k);

                                        if (yInsertion < bestYInsertion) {
                                            bestYInsertion = yInsertion;
                                            yPosition = l;
                                        }
                                    }
                                }
                                // if there is improvment
                                if (xRemoval + bestXInsertion + yRemoval + bestYInsertion < -0.0001) {
                                    improvement = true;
                                    // Update routes
                                    // Insert x into route Y
                                    // If customer exists in the route, only move its goods
                                    if (routes[i].tour[j] == routeY.tour[xPosition]) {
                                        routeY.load += routes[i].satelliteGoods[j];
                                        routeY.satelliteGoods[xPosition] += routes[i].satelliteGoods[j];
                                    }
                                        // Else insert it into the new route
                                    else {
                                        // update load
                                        routeY.load += routes[i].satelliteGoods[j];
                                        // update cost
                                        routeY.cost += bestXInsertion;
                                        // Update tour
                                        routeY.tour.insert(routeY.tour.begin() + xPosition, routes[i].tour[j]);
                                        // Update satelliteGoods
                                        routeY.satelliteGoods.insert(routeY.satelliteGoods.begin() +
                                                                     xPosition,
                                                                     routes[i].satelliteGoods[j]);
                                    }
                                    // Insert y into route X
                                    // If customer exists in the route, only move its goods
                                    if (routes[u].tour[v] == routeX.tour[yPosition]) {
                                        routeX.load += routes[u].satelliteGoods[v];
                                        routeX.satelliteGoods[yPosition] += routes[u].satelliteGoods[v];
                                    }
                                        // Else insert it into the new route
                                    else {
                                        // update load
                                        routeX.load += routes[u].satelliteGoods[v];
                                        // update cost
                                        routeX.cost += bestYInsertion;
                                        // Update tour
                                        routeX.tour.insert(routeX.tour.begin() + yPosition, routes[u].tour[v]);
                                        // Update satelliteGoods
                                        routeY.satelliteGoods.insert(routeX.satelliteGoods.begin() +
                                                                     yPosition,
                                                                     routes[u].satelliteGoods[v]);
                                    }
                                    // Update solution routes
                                    routes[i].load = routeX.load;
                                    routes[i].cost = routeX.cost;
                                    routes[i].tour.assign(routeX.tour.begin(), routeX.tour.end());
                                    routes[i].satelliteGoods.assign(routeX.satelliteGoods.begin(),
                                                                    routeX.satelliteGoods.end());
                                    routes[u].load = routeY.load;
                                    routes[u].cost = routeY.cost;
                                    routes[u].tour.assign(routeY.tour.begin(), routeY.tour.end());
                                    routes[u].satelliteGoods.assign(routeY.satelliteGoods.begin(),
                                                                    routeY.satelliteGoods.end());
                                    // Update solution costs
                                    solution.setTotalCost(
                                            solution.getTotalCost() + xRemoval + bestXInsertion + yRemoval +
                                            bestYInsertion);
                                }
                            }
                        }

                    }

                }
            }
        }
    }
    while (improvement);

    return oldCost - solution.getTotalCost();
}

/******************************
 * Insertion Heuristics
 ******************************/
double SDVRPSolver::insertionCost(Solution &solution, int satelliteId, int dDemand) {
    if (dDemand == 0)
        return 0;

    Solution s1(solution);
    this->constructiveHeuristic(s1);
    Solution s2(solution);
    s2.getSatelliteDemands()[satelliteId] += dDemand;
    this->constructiveHeuristic(s2);
    return s2.getTotalCost() - s1.getTotalCost();

    /*double totalCost = 0;
    vector<E1Route> routes(solution.getE1Routes().begin(),solution.getE1Routes().end());
    if(dDemand < 0){
        while (dDemand != 0) {
            // Find route containing minimum demand for satelliteID
            // Remove dDemand or par of it from route
            // If there is no more demand for satelliteId then remove it from route
        }
    }
    else {
        double cost,minCost;
        int insertRoute, insertPos;
        Node p,q,s;
        s = problem->getSatellite(satelliteId);
        bool found;
        while (dDemand != 0) {
            minCost = Config::DOUBLE_INFINITY;
            found = false;
            insertPos = -1;
            insertRoute = -1;
            // Find a route containing satelliteId or the cheapest insertion for satellite id
            for (int i = 0; i < routes.size() ; ++i) {
                E1Route &route = routes[i];
                // If there is no more space in route, then ignore it
                if(route.load == problem->getE1Capacity()) continue;
                // Else
                for (int j = 0; j <= route.tour.size(); ++j) {
                    if(j < route.tour.size() && route.tour[j]==satelliteId){
                        // Save positions then break
                        insertPos = j;
                        insertRoute = i;
                        minCost = 0;
                        found = true;
                        break;
                    }
                    else{
                        if (j == 0)
                            p = this->problem->getDepot();
                        else
                            p = this->problem->getSatellite(route.tour[j - 1]);

                        if (j == route.tour.size())
                            q = this->problem->getDepot();
                        else
                            q = this->problem->getSatellite(route.tour[j]);

                        cost = problem->getDistance(p,s) + problem->getDistance(s,q) - problem->getDistance(p,q);

                        if(cost<minCost){
                            insertPos = j;
                            insertRoute = i;
                            minCost = cost;
                        }
                    }
                }
                // If satelliteId found then Break;
                if(found) break;
            }
            // If there is a possible insertion then, insert satelliteId or add its demand
            if(found){
                E1Route &route = routes[insertRoute];
                // Compute the quantity supplied to i by route r : if Sr > U[i] then Sr = Sr-U[i] and U[i]=0, else Sr = 0 and U[i]=U[i]-Sr (split occurs)
                if(dDemand < problem->getE1Capacity()-route.load){
                    route.load += dDemand;
                    route.satelliteGoods.insert(route.satelliteGoods.begin()+insertPos,dDemand);
                    dDemand = 0;
                }
                else {
                    route.satelliteGoods.insert(route.satelliteGoods.begin()+insertPos,problem->getE1Capacity()-route.load);
                    dDemand -= (problem->getE1Capacity()-route.load);
                    route.load = problem->getE1Capacity();
                }
            } else {
                if(minCost< 2*problem->getDistance(problem->getDepot(),s)){
                    E1Route &route = routes[insertRoute];
                    route.cost += minCost;
                    route.tour.insert(route.tour.begin()+insertPos,satelliteId);
                    // Compute the quantity supplied to i by route r : if Sr > U[i] then Sr = Sr-U[i] and U[i]=0, else Sr = 0 and U[i]=U[i]-Sr (split occurs)
                    if(dDemand < problem->getE1Capacity()-route.load){
                        route.load += dDemand;
                        route.satelliteGoods.insert(route.satelliteGoods.begin()+insertPos,dDemand);
                        dDemand = 0;
                    }
                    else {
                        route.satelliteGoods.insert(route.satelliteGoods.begin()+insertPos,problem->getE1Capacity()-route.load);
                        dDemand-= (problem->getE1Capacity()-route.load);
                        route.load = problem->getE1Capacity();
                    }
                    // Optimize route using Local Search
                    // Todo
                    // Update Total Cost
                    totalCost+=minCost;
                }
                // Else, initiate a new route
                else{
                    E1Route route;
                    route.cost =  2*problem->getDistance(problem->getDepot(),s);
                    route.tour.push_back(satelliteId);
                    // Compute the quantity supplied to i by route r : if Sr > U[i] then Sr = Sr-U[i] and U[i]=0, else Sr = 0 and U[i]=U[i]-Sr (split occurs)
                    if(dDemand< problem->getE1Capacity()){
                        route.load = dDemand;
                        route.satelliteGoods.push_back(dDemand);
                        dDemand = 0;
                    }
                    else {
                        route.satelliteGoods.push_back(problem->getE1Capacity());
                        dDemand -= (problem->getE1Capacity());
                        route.load = problem->getE1Capacity();
                    }
                    // Insert route into solution
                    routes.push_back(route);
                    // Update solution cost
                    totalCost+=route.cost;
                }
            }
        }
    }
    // return total cost */
    //return totalCost;
}

void SDVRPSolver::insert(Solution &solution, int satelliteId, int dDemand) {
    if (dDemand == 0) return;

    double totalCost = 0;
    vector<E1Route> routes = solution.getE1Routes();
    if (dDemand < 0) {
        while (dDemand != 0) {
            // Find route containing minimum demand for satelliteID
            // Remove dDemand or par of it from route
            // If there is no more demand for satelliteId then remove it from route
        }
    }
    else {
        double cost, minCost;
        int insertRoute, insertPos;
        Node p, q, s;
        s = problem->getSatellite(satelliteId);
        bool found;
        while (dDemand != 0) {
            minCost = Config::DOUBLE_INFINITY;
            found = false;
            insertPos = -1;
            insertRoute = -1;
            // Find a route containing satelliteId or the cheapest insertion for satellite id
            for (int i = 0; i < solution.getE1Routes().size(); ++i) {
                E1Route &route = solution.getE1Routes()[i];
                // If there is no more space in route, then ignore it
                if (route.load == problem->getE1Capacity()) continue;
                // Else
                for (int j = 0; j <= route.tour.size(); ++j) {
                    if (j < route.tour.size() && route.tour[j] == satelliteId) {
                        // Save positions then break
                        insertPos = j;
                        insertRoute = i;
                        minCost = 0;
                        found = true;
                        break;
                    }
                    else {
                        if (j == 0)
                            p = this->problem->getDepot();
                        else
                            p = this->problem->getSatellite(route.tour[j - 1]);

                        if (j == route.tour.size())
                            q = this->problem->getDepot();
                        else
                            q = this->problem->getSatellite(route.tour[j]);

                        cost = problem->getDistance(p, s) + problem->getDistance(s, q) - problem->getDistance(p, q);

                        if (cost < minCost) {
                            insertPos = j;
                            insertRoute = i;
                            minCost = cost;
                        }
                    }
                }
                // If satelliteId found then Break;
                if (found) break;
            }
            // If there is a possible insertion then, insert satelliteId or add its demand
            if (found) {
                E1Route &route = solution.getE1Routes()[insertRoute];
                // Compute the quantity supplied to i by route r : if Sr > U[i] then Sr = Sr-U[i] and U[i]=0, else Sr = 0 and U[i]=U[i]-Sr (split occurs)
                if (dDemand < problem->getE1Capacity() - route.load) {
                    route.load += dDemand;
                    route.satelliteGoods.insert(route.satelliteGoods.begin() + insertPos, dDemand);
                    dDemand = 0;
                }
                else {
                    route.satelliteGoods.insert(route.satelliteGoods.begin() + insertPos,
                                                problem->getE1Capacity() - route.load);
                    dDemand -= (problem->getE1Capacity() - route.load);
                    route.load = problem->getE1Capacity();
                }
            } else {
                if (minCost < 2 * problem->getDistance(problem->getDepot(), s)) {
                    E1Route &route = solution.getE1Routes()[insertRoute];
                    route.cost += minCost;
                    route.tour.insert(route.tour.begin() + insertPos, satelliteId);
                    // Compute the quantity supplied to i by route r : if Sr > U[i] then Sr = Sr-U[i] and U[i]=0, else Sr = 0 and U[i]=U[i]-Sr (split occurs)
                    if (dDemand < problem->getE1Capacity() - route.load) {
                        route.load += dDemand;
                        route.satelliteGoods.insert(route.satelliteGoods.begin() + insertPos, dDemand);
                        dDemand = 0;
                    }
                    else {
                        route.satelliteGoods.insert(route.satelliteGoods.begin() + insertPos,
                                                    problem->getE1Capacity() - route.load);
                        dDemand -= (problem->getE1Capacity() - route.load);
                        route.load = problem->getE1Capacity();
                    }
                    // Optimize route using Local Search
                    // Todo
                    // Update Total Cost
                    totalCost += minCost;
                }
                    // Else, initiate a new route
                else {
                    E1Route route;
                    route.cost = 2 * problem->getDistance(problem->getDepot(), s);
                    route.tour.push_back(satelliteId);
                    // Compute the quantity supplied to i by route r : if Sr > U[i] then Sr = Sr-U[i] and U[i]=0, else Sr = 0 and U[i]=U[i]-Sr (split occurs)
                    if (dDemand < problem->getE1Capacity()) {
                        route.load = dDemand;
                        route.satelliteGoods.push_back(dDemand);
                        dDemand = 0;
                    }
                    else {
                        route.satelliteGoods.push_back(problem->getE1Capacity());
                        dDemand -= (problem->getE1Capacity());
                        route.load = problem->getE1Capacity();
                    }
                    // Insert route into solution
                    solution.getE1Routes().push_back(route);
                    // Update solution cost
                    totalCost += route.cost;
                }
            }
        }
    }
}

