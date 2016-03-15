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

void SDVRPSolver::constructiveHeuristic(Solution &solution) {
    // List containig the clients
    vector<L_Entry> L; //= vector<L_Entry>(solution.getProblem()->getSatellites().size());
    // List containing the demands of the clients
    const vector<int> Q = solution.getSatelliteDemands();
    // List containing the unserved demand of each client
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
            // Optimize route using Local Search
            // Todo
            double dCost = apply2Opt(route);
            dCost += applyOrOpt(route, 1);
            dCost += applyOrOpt(route, 2);
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
        // If U[i] is fully supplied, go to the next customer in L
        if (U[i.getSatelliteId()] == 0) c++;
    }
    // End While

    // Update satellite demands
    for (int m = 0; m < solution.getSatelliteDemands().size(); ++m) {
        solution.getSatelliteDemands()[m] = U[m];
    }

}

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
                                                 this->problem->getClient(route.tour[u]));
            else
                dxu = this->problem->getDistance(this->problem->getClient(route.tour[u - 1]),
                                                 this->problem->getClient(route.tour[u]));

            for (v = u + 1; v < route.tour.size(); v++) {
                if (v == route.tour.size() - 1)
                    dvy = this->problem->getDistance(this->problem->getClient(route.tour[v]),
                                                     this->problem->getDepot());
                else
                    dvy = this->problem->getDistance(this->problem->getClient(route.tour[v]),
                                                     this->problem->getClient(route.tour[v + 1]));

                if (u == 0)
                    dxv = this->problem->getDistance(this->problem->getDepot(),
                                                     this->problem->getClient(route.tour[v]));
                else
                    dxv = this->problem->getDistance(this->problem->getClient(route.tour[u - 1]),
                                                     this->problem->getClient(route.tour[v]));

                if (v == route.tour.size() - 1)
                    duy = this->problem->getDistance(this->problem->getClient(route.tour[u]),
                                                     this->problem->getDepot());
                else
                    duy = this->problem->getDistance(this->problem->getClient(route.tour[u]),
                                                     this->problem->getClient(route.tour[v + 1]));

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
            i = this->problem->getClient(route.tour[u]);
            j = this->problem->getClient(route.tour[v]);
            tmpTour = route.tour;
            tmpTour.erase(tmpTour.begin() + u, tmpTour.begin() + u + seqLength);
            // Compute the change of cost caused by deleting sequence i-> ... ->j : dRemoval
            // Removed arcs are : (p=i-1,i),(j,q=j+1).  New arc is (i-1,j+1)
            // dRemoval = c(p,q) - ( c(p,i) + c(j,q) )
            if (u == 0)
                p = this->problem->getDepot();
            else
                p = this->problem->getClient(route.tour[u - 1]);

            if (v == route.tour.size() - 1)
                q = this->problem->getDepot();
            else
                q = this->problem->getClient(route.tour[v + 1]);

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
                    k = this->problem->getClient(tmpTour[w]);
                }
                else if (w == tmpTour.size()) {
                    k_1 = this->problem->getClient(tmpTour[w - 1]);
                    k = this->problem->getDepot();
                }
                else {
                    k_1 = this->problem->getClient(tmpTour[w - 1]);
                    k = this->problem->getClient(tmpTour[w]);
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
                            p = this->problem->getClient(route.tour[j - 1]);

                        if (j == route.tour.size())
                            q = this->problem->getDepot();
                        else
                            q = this->problem->getClient(route.tour[j]);

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
                            p = this->problem->getClient(route.tour[j - 1]);

                        if (j == route.tour.size())
                            q = this->problem->getDepot();
                        else
                            q = this->problem->getClient(route.tour[j]);

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
