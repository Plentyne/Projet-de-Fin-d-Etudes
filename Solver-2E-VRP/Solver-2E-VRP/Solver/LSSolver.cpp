//
// Created by Youcef on 03/03/2016.
//

#include "LSSolver.h"
#include "../Config.h"
#include <algorithm>


// Todo make first improvement
// For now : does best improvement
bool LSSolver::apply2OptOnTour(E2Route &e2Route) {
    //cout << "number of calls : " << calls++ << endl;
    if (e2Route.tour.size() <= 2) return false;

    double improved, minImproved;
    double oldCost = e2Route.cost;
    int u, v, i, j;
    double dxu, dvy, dxv, duy;
    bool res = false;

    do {
        minImproved = 0;
        i = j = 0;

        for (u = 0; u < e2Route.tour.size(); u++) { // inner loop
            if (u == 0)
                dxu = this->problem->getDistance(this->problem->getSatellite(e2Route.departureSatellite),
                                                 this->problem->getClient(e2Route.tour[u]));
            else
                dxu = this->problem->getDistance(this->problem->getClient(e2Route.tour[u - 1]),
                                                 this->problem->getClient(e2Route.tour[u]));

            for (v = u + 1; v < e2Route.tour.size(); v++) {
                if (v == e2Route.tour.size() - 1)
                    dvy = this->problem->getDistance(this->problem->getClient(e2Route.tour[v]),
                                                     this->problem->getSatellite(e2Route.departureSatellite));
                else
                    dvy = this->problem->getDistance(this->problem->getClient(e2Route.tour[v]),
                                                     this->problem->getClient(e2Route.tour[v + 1]));

                if (u == 0)
                    dxv = this->problem->getDistance(this->problem->getSatellite(e2Route.departureSatellite),
                                                     this->problem->getClient(e2Route.tour[v]));
                else
                    dxv = this->problem->getDistance(this->problem->getClient(e2Route.tour[u - 1]),
                                                     this->problem->getClient(e2Route.tour[v]));

                if (v == e2Route.tour.size() - 1)
                    duy = this->problem->getDistance(this->problem->getClient(e2Route.tour[u]),
                                                     this->problem->getSatellite(e2Route.departureSatellite));
                else
                    duy = this->problem->getDistance(this->problem->getClient(e2Route.tour[u]),
                                                     this->problem->getClient(e2Route.tour[v + 1]));

                improved = dxv + duy - (dxu + dvy);

                if (improved - minImproved < -0.01) {
                    minImproved = improved;
                    i = u;
                    j = v;
                    res = true;
                }
            }
        }
        // inverse sequence order between u and v
        std::reverse(e2Route.tour.begin() + i, e2Route.tour.begin() + j + 1);
        e2Route.cost += minImproved;
    } while (minImproved < 0);

    return res;
}

// Todo make first improvement
// For now : does best improvement
bool LSSolver::applyOrOpt(E2Route &e2Route, int seqLength) {

    if (seqLength < 1) return false;
    if (e2Route.tour.size() <= 2) return false;
    if (e2Route.tour.size() <= seqLength) return false;

    int u, v;
    int position;
    int seqBegin;
    double delta = 0;
    double bestDelta;
    double dRemoval, dInsertion;
    double oldCost = e2Route.cost;
    // Repeat
    do {

        bestDelta = 0;
        position = -1;

        // For Each vertex of the route
        for (u = 0; u <= e2Route.tour.size() - seqLength; ++u) {
            // Save the first and last elements (respectively i and j) of the sequence being tried
            v = u + seqLength - 1;
            Client i = this->problem->getClient(e2Route.tour[u]);
            Client j = this->problem->getClient(e2Route.tour[v]);
            vector<int> tmpTour(e2Route.tour);
            tmpTour.erase(tmpTour.begin() + u, tmpTour.begin() + u + seqLength);
            // Compute the change of cost caused by deleting sequence i-> ... ->j : dRemoval
            // Removed arcs are : (p=i-1,i),(j,q=j+1).  New arc is (i-1,j+1)
            // dRemoval = c(p,q) - ( c(p,i) + c(j,q) )
            Node p;
            if (u == 0)
                p = this->problem->getSatellite(e2Route.departureSatellite);
            else
                p = this->problem->getClient(e2Route.tour[u - 1]);

            Node q;
            if (v == e2Route.tour.size() - 1)
                q = this->problem->getSatellite(e2Route.departureSatellite);
            else
                q = this->problem->getClient(e2Route.tour[v + 1]);

            dRemoval = this->problem->getDistance(p, q) -
                       (this->problem->getDistance(p, i) + this->problem->getDistance(j, q));

            // For Each position k in tmpTour
            for (int w = 0; w <= tmpTour.size(); ++w) {

                if (w == u) continue;
                // Compute the cost of inserting i->..->j at position k : dInsertion
                // Removed arc is (k-1,k). New arcs are (k-1,i) and (j,k)
                // dInsertion = c(k-1,i)+c(j,k)-c(k-1,k)
                Node k, k_1;
                if (w == 0) {
                    k_1 = this->problem->getSatellite(e2Route.departureSatellite);
                    k = this->problem->getClient(tmpTour[w]);
                }
                else if (w == tmpTour.size()) {
                    k_1 = this->problem->getClient(tmpTour[w - 1]);
                    k = this->problem->getSatellite(e2Route.departureSatellite);
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
            vector<int> sequence(e2Route.tour.begin() + seqBegin, e2Route.tour.begin() + seqBegin + seqLength);
            e2Route.tour.erase(e2Route.tour.begin() + seqBegin, e2Route.tour.begin() + seqBegin + seqLength);
            e2Route.tour.insert(e2Route.tour.begin() + position, sequence.begin(), sequence.end());
            e2Route.cost += bestDelta;
        }

    } while (delta < 0); // While there is improvement

    return (e2Route.cost - oldCost < -0.01);
}

bool LSSolver::doChangeSatellite(Solution &solution) {
    double oldCost = solution.getTotalCost();
    for (E2Route &e2Route : solution.getE2Routes()) {
        if (e2Route.tour.size() == 1) continue;
        solution.setTotalCost(solution.getTotalCost() - e2Route.cost);
        double minCost = Config::DOUBLE_INFINITY;
        double tmpCost;
        int position = -1;
        int satellite = -1;
        for (int i = 0; i < this->problem->getSatellites().size(); ++i) {
            // Si le satellite ne peux plus avoir de tournées
            if (solution.satelliteAssignedRoutes[i] >= problem->getMaxCf()) continue;

            for (int j = 0; j < e2Route.tour.size(); ++j) {
                if (j == 0)
                    tmpCost = this->problem->getDistance(problem->getSatellite(i),
                                                         problem->getClient(e2Route.tour[0]))
                              + this->problem->getDistance(problem->getSatellite(i),
                                                           problem->getClient(e2Route.tour.back()))
                              - this->problem->getDistance(problem->getSatellite(e2Route.departureSatellite),
                                                           problem->getClient(e2Route.tour[0]))
                              - this->problem->getDistance(problem->getSatellite(e2Route.departureSatellite),
                                                           problem->getClient(e2Route.tour.back()));

                else
                    tmpCost = this->problem->getDistance(problem->getSatellite(i),
                                                         problem->getClient(e2Route.tour[j - 1]))
                              + this->problem->getDistance(problem->getSatellite(i),
                                                           problem->getClient(e2Route.tour[j]))
                              + this->problem->getDistance(problem->getClient(e2Route.tour[0]),
                                                           problem->getClient(e2Route.tour.back()))
                              - this->problem->getDistance(problem->getClient(e2Route.tour[j - 1]),
                                                           problem->getClient(e2Route.tour[j]))
                              - this->problem->getDistance(problem->getSatellite(e2Route.departureSatellite),
                                                           problem->getClient(e2Route.tour[0]))
                              - this->problem->getDistance(problem->getSatellite(e2Route.departureSatellite),
                                                           problem->getClient(e2Route.tour.back()));

                if (tmpCost - minCost < -0.01) {
                    minCost = tmpCost;
                    position = j;
                    satellite = i;
                }
            }
        }
        e2Route.cost += minCost;
        solution.getSatelliteDemands()[e2Route.departureSatellite] -= e2Route.load;
        solution.getSatelliteDemands()[satellite] += e2Route.load;
        solution.satelliteAssignedRoutes[e2Route.departureSatellite]--;
        solution.satelliteAssignedRoutes[satellite]++;
        e2Route.departureSatellite = satellite;
        std::rotate(e2Route.tour.begin(), e2Route.tour.begin() + position, e2Route.tour.end());
        // Insérer la tournée actuelle dans la solution
        // Calculer le nouveau côut de la route
        e2Route.cost = this->problem->getDistance(this->problem->getSatellite(e2Route.departureSatellite),
                                                  this->problem->getClient(e2Route.tour[0]))
                       + this->problem->getDistance(this->problem->getSatellite(e2Route.departureSatellite),
                                                    this->problem->getClient(
                                                            e2Route.tour[e2Route.tour.size() - 1]));;
        for (int k = 0; k < e2Route.tour.size() - 1; ++k) {
            e2Route.cost += this->problem->getDistance(this->problem->getClient(e2Route.tour[k]),
                                                       this->problem->getClient(e2Route.tour[k + 1]));
        }
        solution.setTotalCost(solution.getTotalCost() + e2Route.cost);
    }
    return solution.getTotalCost() - oldCost < -0.01;
}


// Todo make first improvement
// For now : does best improvement
bool LSSolver::applyRelocate(Solution &solution) {
    bool improvement;
    double dRemoval, dInsertion;
    vector<E2Route> &routes = solution.getE2Routes();
    double minRemoval;
    double minInsertion;
    int insertRoute;
    int insertPosition;
    double oldCost = solution.getTotalCost();
    int i = 0;
    // Repeat
    do {
        improvement = false;
        i++;
        // For each route r in solution.getE2Routes() do
        for (int r = 0; r < solution.getE2Routes().size(); ++r) {
            // For each client c in route do
            for (int c = 0; c < solution.getE2Routes()[r].tour.size(); ++c) {
                minRemoval = 0;
                minInsertion = 0;
                // Compute the cost of removing c from r : dRemoval
                Client client = this->problem->getClient(solution.getE2Routes()[r].tour[c]);
                Node p;
                if (c == 0)
                    p = this->problem->getSatellite(solution.getE2Routes()[r].departureSatellite);
                else
                    p = this->problem->getClient(solution.getE2Routes()[r].tour[c - 1]);

                Node q;
                if (c == solution.getE2Routes()[r].tour.size() - 1)
                    q = this->problem->getSatellite(solution.getE2Routes()[r].departureSatellite);
                else
                    q = this->problem->getClient(solution.getE2Routes()[r].tour[c + 1]);

                dRemoval = this->problem->getDistance(p, q) -
                           (this->problem->getDistance(p, client) + this->problem->getDistance(client, q));

                // For each route t in solution.getE2Routes() do
                for (int t = 0; t < solution.getE2Routes().size(); ++t) {
                    // If same route then ignore it
                    if (t == r) continue;
                    // If not enough space, then ignore the route
                    if (client.getDemand() > this->problem->getE2Capacity() - solution.getE2Routes()[t].load) continue;
                    // For each position i in r' do
                    for (int i = 0; i <= solution.getE2Routes()[t].tour.size(); ++i) {
                        // Ignore the same position of the same route
                        if (r == t && i == c) continue;
                        // Compute the cost for inserting c at position i : dInsertion
                        Node u;
                        if (i == 0)
                            u = this->problem->getSatellite(solution.getE2Routes()[t].departureSatellite);
                        else
                            u = this->problem->getClient(solution.getE2Routes()[t].tour[i - 1]);

                        Node v;
                        if (i == solution.getE2Routes()[t].tour.size())
                            v = this->problem->getSatellite(solution.getE2Routes()[t].departureSatellite);
                        else
                            v = this->problem->getClient(solution.getE2Routes()[t].tour[i]);

                        dInsertion = (this->problem->getDistance(u, client) + this->problem->getDistance(client, v)) -
                                     this->problem->getDistance(u, v);

                        if ((dRemoval + dInsertion) - (minRemoval + minInsertion) < -0.01) {

                            // Keep the best possible improvement for client c
                            minRemoval = dRemoval;
                            minInsertion = dInsertion;
                            insertRoute = t;
                            insertPosition = i;

                        }

                    }
                    // End For
                }
                // End For
                // Take the change if there is improvement
                if (minRemoval + minInsertion < -0.01) {
                    improvement = true;
                    // Remove from route r
                    solution.getE2Routes()[r].cost += minRemoval;
                    solution.getE2Routes()[r].load -= client.getDemand();
                    solution.getSatelliteDemands()[solution.getE2Routes()[r].departureSatellite] -= client.getDemand();
                    solution.getE2Routes()[r].tour.erase(solution.getE2Routes()[r].tour.begin() + c);
                    // If route r becomes empty then remove it
                    if (solution.getE2Routes()[r].tour.size() == 0) {
                        solution.getE2Routes().erase(solution.getE2Routes().begin() + r);
                    }
                    // Insert into route insertRoute
                    solution.getE2Routes()[insertRoute].cost += minInsertion;
                    solution.getE2Routes()[insertRoute].load += client.getDemand();
                    solution.getSatelliteDemands()[solution.getE2Routes()[insertRoute].departureSatellite] += client.getDemand();
                    if (insertPosition < solution.getE2Routes()[insertRoute].tour.size()) {
                        solution.getE2Routes()[insertRoute].tour.insert(
                                solution.getE2Routes()[insertRoute].tour.begin() + insertPosition,
                                client.getClientId());
                    }
                    else {
                        solution.getE2Routes()[insertRoute].tour.push_back(client.getClientId());
                    }
                    // Update Solution cost
                    solution.setTotalCost(solution.getTotalCost() + minInsertion + minRemoval);
                }
            }
            // End For
        }
        // End For
    } while (improvement);
    // Until there is no improvement
    return solution.getTotalCost() - oldCost < -0.01;
}

// Todo make first improvement
/* For now does Best Improvement*/
bool LSSolver::applySwap(Solution &solution) {
    bool improvement;
    //vector<E2Route> &solution.getE2Routes() = solution.getE2Routes();
    vector<bool> swaped = vector<bool>(this->problem->getClients().size());
    double xRemoval, xInsertion, yRemoval, yInsertion;
    double minXRemoval, minYRemoval;
    double minXInsertion, minYInsertion;
    int insertRoute, insertPosition;
    double oldCost = solution.getTotalCost();
    // Repeat
    do {
        improvement = false;
        fill(swaped.begin(), swaped.end(), false);
        // For each route r in solution.getE2Routes() do
        for (int r = 0; r < solution.getE2Routes().size(); ++r) {
            // For each client x in r do
            for (int x = 0; x < solution.getE2Routes()[r].tour.size(); ++x) {
                // Compute the cost for removing x from r : xRemoval
                minXRemoval = 0;
                minXInsertion = 0;
                minYRemoval = 0;
                minYInsertion = 0;
                // Compute the cost of removing c from r : dRemoval
                Client xClient = this->problem->getClient(solution.getE2Routes()[r].tour[x]);
                Node p;
                if (x == 0)
                    p = this->problem->getSatellite(solution.getE2Routes()[r].departureSatellite);
                else
                    p = this->problem->getClient(solution.getE2Routes()[r].tour[x - 1]);
                Node q;
                if (x == solution.getE2Routes()[r].tour.size() - 1)
                    q = this->problem->getSatellite(solution.getE2Routes()[r].departureSatellite);
                else
                    q = this->problem->getClient(solution.getE2Routes()[r].tour[x + 1]);

                xRemoval = -(this->problem->getDistance(p, xClient) + this->problem->getDistance(xClient, q));

                // For each route t different from r do
                for (int t = r + 1; t < solution.getE2Routes().size(); ++t) {
                    // For each client y in t do
                    for (int y = 0; y < solution.getE2Routes()[t].tour.size(); ++y) {
                        // Get client Y
                        Client yClient = this->problem->getClient(solution.getE2Routes()[t].tour[y]);
                        // if the client had already been swapped, then ignore it
                        //if(swaped[solution.getE2Routes()[r].tour[x]]) continue;
                        // If there isn't enough space in vehicles for swapping
                        if ((solution.getE2Routes()[r].load - xClient.getDemand() + yClient.getDemand() >
                             this->problem->getE2Capacity())
                            || (solution.getE2Routes()[t].load + xClient.getDemand() - yClient.getDemand() >
                                this->problem->getE2Capacity())) {
                            continue;
                        }

                        // Compute the cost for inserting y in x's location  : yInsertion
                        yInsertion = this->problem->getDistance(p, yClient) + this->problem->getDistance(yClient, q);
                        // Compute the cost for inserting x in y's location : xInsertion
                        Node u;
                        if (y == 0)
                            u = this->problem->getSatellite(solution.getE2Routes()[t].departureSatellite);
                        else
                            u = this->problem->getClient(solution.getE2Routes()[t].tour[y - 1]);

                        Node v;
                        if (y == solution.getE2Routes()[t].tour.size() - 1)
                            v = this->problem->getSatellite(solution.getE2Routes()[t].departureSatellite);
                        else
                            v = this->problem->getClient(solution.getE2Routes()[t].tour[y + 1]);

                        xInsertion = (this->problem->getDistance(u, xClient) + this->problem->getDistance(xClient, v));
                        // Compute the cost for removing y from t : yRemoval
                        yRemoval = -(this->problem->getDistance(u, yClient) + this->problem->getDistance(yClient, v));
                        // if there is improvement i.e. : xRemoval + xInsertion + yRemoval + yInsertion then
                        if (xRemoval + xInsertion + yRemoval + yInsertion <
                            minXRemoval + minXInsertion + minYRemoval + minYInsertion) {
                            // Save the coordinates of the clients to swap
                            minXRemoval = xRemoval;
                            minXInsertion = xInsertion;
                            minYRemoval = yRemoval;
                            minYInsertion = yInsertion;
                            insertRoute = t;
                            insertPosition = y;
                        }
                    }
                    // End For
                }
                // End For
                // If there is a possible swap then swap the two customers
                if (minXRemoval + minXInsertion + minYRemoval + minYInsertion < -0.01) {
                    improvement = true;
                    // Insert x into t
                    Client yClient = this->problem->getClient(solution.getE2Routes()[insertRoute].tour[insertPosition]);
                    solution.getE2Routes()[insertRoute].tour[insertPosition] = solution.getE2Routes()[r].tour[x];
                    solution.getE2Routes()[insertRoute].cost += (minXInsertion + minYRemoval);
                    //solution.getE2Routes()[insertRoute].load += (xClient.getDemand() - yClient.getDemand());
                    solution.getE2Routes()[insertRoute].load += xClient.getDemand();
                    solution.getE2Routes()[insertRoute].load -= yClient.getDemand();
                    solution.getSatelliteDemands()[solution.getE2Routes()[insertRoute].departureSatellite] += (
                            xClient.getDemand() - yClient.getDemand());
                    // Insert y into r
                    solution.getE2Routes()[r].tour[x] = yClient.getClientId();
                    solution.getE2Routes()[r].cost += (minXRemoval + minYInsertion);
                    //solution.getE2Routes()[r].load += (yClient.getDemand() - xClient.getDemand());
                    solution.getE2Routes()[r].load -= xClient.getDemand();
                    solution.getE2Routes()[r].load += yClient.getDemand();
                    solution.getSatelliteDemands()[solution.getE2Routes()[r].departureSatellite] += (
                            yClient.getDemand() - xClient.getDemand());
                    // Update solution cost
                    solution.setTotalCost(
                            solution.getTotalCost() + minXRemoval + minXInsertion + minYRemoval + minYInsertion);
                    swaped[solution.getE2Routes()[r].tour[x]] = true;
                }

            }
            // End For
        }
        // End For

    } while (improvement);
    // While there is improvement
    swaped.clear();
    return (solution.getTotalCost() - oldCost) < -0.01;
}
/* For now does First Improvement */
// Todo Optimize code
bool LSSolver::apply2optStar(Solution &solution) {
    int position1;
    int position2;
    double delta1 = 0;
    double delta2 = 0;
    int load1, load2;
    double cost1, cost2;
    short choice;
    double oldCost = solution.getTotalCost();
    double costChange = 0;
    bool improvement;

    // Repeat
    do {
        improvement = false;
        for (int u = 0; u < solution.getE2Routes().size() && !improvement; ++u) {
            for (int v = 0; v < solution.getE2Routes().size() && !improvement; ++v) {
                if (u == v ||
                    solution.getE2Routes()[u].departureSatellite != solution.getE2Routes()[v].departureSatellite)
                    continue;
                if (solution.getE2Routes()[u].tour.size() == 1 || solution.getE2Routes()[v].tour.size() == 1) continue;
                E2Route &r1 = solution.getE2Routes()[u];
                E2Route &r2 = solution.getE2Routes()[v];
                load1 = 0;

                for (int i = 0; i < r1.tour.size() - 1 && !improvement; ++i) {
                    Client p = problem->getClient(r1.tour[i]);
                    int i1 = i + 1;
                    Client p_1 = problem->getClient(r1.tour[i + 1]);
                    load1 += p.getDemand();
                    load2 = 0;
                    if (i == 0) cost1 = problem->getDistance(p, problem->getSatellite(r1.departureSatellite));
                    else cost1 += problem->getDistance(p, problem->getClient(r1.tour[i - 1]));

                    for (int j = 0; j < r2.tour.size() - 1 && !improvement; ++j) {
                        Client q = problem->getClient(r2.tour[j]);
                        int j1 = j + 1;
                        Client q_1 = problem->getClient(r2.tour[j + 1]);
                        load2 += q.getDemand();
                        if (j == 0) cost2 = problem->getDistance(q, problem->getSatellite(r2.departureSatellite));
                        else cost2 += problem->getDistance(q, problem->getClient(r2.tour[j - 1]));

                        delta1 = delta2 = Config::DOUBLE_INFINITY;

                        // try the first change : (p,q) , (p+1,q+1)
                        if ((load1 + load2 <= problem->getE2Capacity()) &&
                            (r1.load - load1 + r2.load - load2 <= problem->getE2Capacity())) {
                            delta1 = problem->getDistance(p, q) + problem->getDistance(p_1, q_1) -
                                     (problem->getDistance(p, p_1) + problem->getDistance(q, q_1));
                            if (delta1 < -0.01) {
                                improvement = true;
                                choice = 1;
                            }
                        }

                        // try the first change : (p,q+1) , (p+1,q)
                        if ((load1 + r2.load - load2 <= problem->getE2Capacity()) &&
                            (load2 + r1.load - load1 <= problem->getE2Capacity())) {
                            delta2 = problem->getDistance(p, q_1) + problem->getDistance(p_1, q) -
                                     (problem->getDistance(p, p_1) + problem->getDistance(q, q_1));
                            if (delta2 < -0.01 && delta2 < delta1) {
                                improvement = true;
                                choice = 2;
                            }
                        }
                        //update routes if there is improvement
                        position1 = i + 1;
                        position2 = j + 1;
                        if (improvement) {
                            costChange -= (r1.cost + r2.cost);
                            if (choice == 1) {
                                vector<int> tour1(r1.tour.begin(), r1.tour.begin() + position1);
                                tour1.insert(tour1.end(), r2.tour.rbegin() + r2.tour.size() - position2,
                                             r2.tour.rend());

                                vector<int> tour2(r1.tour.rbegin(), r1.tour.rbegin() + r1.tour.size() - position1);
                                tour2.insert(tour2.end(), r2.tour.begin() + position2, r2.tour.end());

                                r1.tour.assign(tour1.begin(), tour1.end());
                                r2.tour.assign(tour2.begin(), tour2.end());
                                // Update route cost
                                double tmpCost = r1.cost - cost1 + r2.cost - cost2 + problem->getDistance(p_1, q_1)
                                                 - problem->getDistance(p, p_1) - problem->getDistance(q, q_1);
                                r1.cost = cost1 + cost2 + problem->getDistance(p, q);
                                r2.cost = tmpCost;
                                // Update route load
                                int tmpLoad = r1.load - load1 + r2.load - load2;
                                r1.load = load1 + load2;
                                r2.load = tmpLoad;
                            } else {
                                vector<int> tour1(r1.tour.begin(), r1.tour.begin() + position1);
                                tour1.insert(tour1.end(), r2.tour.begin() + position2, r2.tour.end());

                                vector<int> tour2(r2.tour.begin(), r2.tour.begin() + position2);
                                tour2.insert(tour2.end(), r1.tour.begin() + position1, r1.tour.end());

                                r1.tour.assign(tour1.begin(), tour1.end());
                                r2.tour.assign(tour2.begin(), tour2.end());
                                // Update route cost
                                double tmpCost = cost2 + r1.cost - cost1 + problem->getDistance(q, p_1)
                                                 - problem->getDistance(p, p_1);
                                r1.cost = cost1 + r2.cost - cost2 + problem->getDistance(p, q_1)
                                          - problem->getDistance(q, q_1);
                                r2.cost = tmpCost;
                                // Update route load
                                int tmpLoad = load2 + r1.load - load1;
                                r1.load = load1 + r2.load - load2;
                                r2.load = tmpLoad;
                            }
                            costChange += (r1.cost + r2.cost);
                        }
                    }
                }

            }
        }
    } while (improvement); // While there is improvement

    return costChange < -0.01;
}
