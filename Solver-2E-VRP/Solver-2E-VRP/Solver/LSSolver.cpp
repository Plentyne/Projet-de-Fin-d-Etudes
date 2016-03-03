//
// Created by Youcef on 03/03/2016.
//

#include "LSSolver.h"
#include <algorithm>


//int LSSolver::calls =0;

// For now : does best improvement
double LSSolver::apply2OptOnTour(E2Route &e2Route) {
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
        //e2Route.cost += minImproved;
        //cout << "Here minImproved = " << minImproved << endl;
        //cout << "Here i = " << i << endl;
        //cout << "Here j =  " << j << endl;
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

    return e2Route.cost - oldCost;
}