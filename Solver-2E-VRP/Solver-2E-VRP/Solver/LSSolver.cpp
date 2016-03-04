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
    } while (minImproved < 0);

    // Calculer le nouveau côut de la route
    e2Route.cost = this->problem->getDistance(this->problem->getSatellite(e2Route.departureSatellite),
                                              this->problem->getClient(e2Route.tour[0]))
                   + this->problem->getDistance(this->problem->getSatellite(e2Route.departureSatellite),
                                                this->problem->getClient(e2Route.tour.back()));
    for (int k = 0; k < e2Route.tour.size() - 1; ++k) {
        e2Route.cost += this->problem->getDistance(this->problem->getClient(e2Route.tour[k]),
                                                   this->problem->getClient(e2Route.tour[k + 1]));
    }

    return (e2Route.cost < oldCost);
}

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