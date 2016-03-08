//
// Created by Youcef on 03/03/2016.
//

#include "LSSolver.h"
#include <algorithm>


//int LSSolver::calls =0;

// For now : does best improvement
bool LSSolver::apply2OptOnTour(E2Route &e2Route) {
    //cout << "number of calls : " << calls++ << endl;
    if (e2Route.tour.size() <= 2) return 0;

    double improved, minImproved;
    double oldCost = e2Route.cost;
    int u, v, i, j;
    double dxu, dvy, dxv, duy;

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

                if (improved < minImproved) {
                    minImproved = improved;
                    i = u;
                    j = v;
                }
            }
        }
        // inverse sequence order between u and v
        std::reverse(e2Route.tour.begin() + i, e2Route.tour.begin() + j + 1);
        e2Route.cost += minImproved;
    } while (minImproved < 0);

    return (e2Route.cost < oldCost);
}

// For now : does best improvement
bool LSSolver::applyOrOpt(E2Route &e2Route, int seqLength) {

    if (seqLength < 1) return false;
    if (e2Route.tour.size() <= seqLength) return false;

    vector<int> tmpTour;

    int u, v;
    int position;
    int seqBegin;
    double delta = 0;
    double bestDelta;
    double dRemoval, dInsertion;
    double oldCost = e2Route.cost;
    Node p, q, i, j, k, k_1;
    // Repeat
    do {

        bestDelta = 0;
        position = -1;

        // For Each vertex of the route
        for (u = 0; u <= e2Route.tour.size() - seqLength; ++u) {
            // Save the first and last elements (respectively i and j) of the sequence being tried
            v = u + seqLength - 1;
            i = this->problem->getClient(e2Route.tour[u]);
            j = this->problem->getClient(e2Route.tour[v]);
            tmpTour = e2Route.tour;
            tmpTour.erase(tmpTour.begin() + u, tmpTour.begin() + u + seqLength);
            // Compute the change of cost caused by deleting sequence i-> ... ->j : dRemoval
            // Removed arcs are : (p=i-1,i),(j,q=j+1).  New arc is (i-1,j+1)
            // dRemoval = c(p,q) - ( c(p,i) + c(j,q) )
            if (u == 0)
                p = this->problem->getSatellite(e2Route.departureSatellite);
            else
                p = this->problem->getClient(e2Route.tour[u - 1]);

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

    return (e2Route.cost < oldCost);
}

// For now : does best improvement
bool LSSolver::applyRelocate(Solution &solution) {
    bool improvement;
    double dRemoval, dInsertion;
    vector<E2Route> &routes = solution.getE2Routes();
    Node p, q, u, v;
    Client client;
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
                client = this->problem->getClient(solution.getE2Routes()[r].tour[c]);
                if (c == 0)
                    p = this->problem->getSatellite(solution.getE2Routes()[r].departureSatellite);
                else
                    p = this->problem->getClient(solution.getE2Routes()[r].tour[c - 1]);

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
                        if (i == 0)
                            u = this->problem->getSatellite(solution.getE2Routes()[t].departureSatellite);
                        else
                            u = this->problem->getClient(solution.getE2Routes()[t].tour[i - 1]);

                        if (i == solution.getE2Routes()[t].tour.size())
                            v = this->problem->getSatellite(solution.getE2Routes()[t].departureSatellite);
                        else
                            v = this->problem->getClient(solution.getE2Routes()[t].tour[i]);

                        dInsertion = (this->problem->getDistance(u, client) + this->problem->getDistance(client, v)) -
                                     this->problem->getDistance(u, v);

                        if ((dRemoval + dInsertion) - (minRemoval + minInsertion) < -0.0001) {

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
                if (minRemoval + minInsertion < -0.0001) {
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
    return solution.getTotalCost() < oldCost;
}

/* For now does Best Improvement*/
bool LSSolver::applySwap(Solution &solution) {
    bool improvement;
    //vector<E2Route> &solution.getE2Routes() = solution.getE2Routes();
    vector<bool> swaped = vector<bool>(this->problem->getClients().size());
    Node p, q, u, v;
    double xRemoval, xInsertion, yRemoval, yInsertion;
    Client xClient, yClient;
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
                xClient = this->problem->getClient(solution.getE2Routes()[r].tour[x]);
                if (x == 0)
                    p = this->problem->getSatellite(solution.getE2Routes()[r].departureSatellite);
                else
                    p = this->problem->getClient(solution.getE2Routes()[r].tour[x - 1]);

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
                        yClient = this->problem->getClient(solution.getE2Routes()[t].tour[y]);
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
                        if (y == 0)
                            u = this->problem->getSatellite(solution.getE2Routes()[t].departureSatellite);
                        else
                            u = this->problem->getClient(solution.getE2Routes()[t].tour[y - 1]);

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
                if (minXRemoval + minXInsertion + minYRemoval + minYInsertion < -0.0001) {
                    improvement = true;
                    // Insert x into t
                    yClient = this->problem->getClient(solution.getE2Routes()[insertRoute].tour[insertPosition]);
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
    return (solution.getTotalCost() < oldCost);
}
