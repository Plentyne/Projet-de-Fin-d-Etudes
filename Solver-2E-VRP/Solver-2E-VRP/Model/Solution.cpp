//
// Created by Youcef on 02/02/2016.
//

#include "Solution.h"
#include "../Config.h"
#include <fstream>

const bool Solution::OPEN = true;
const bool Solution::CLOSED = false;

const short Solution::PROHIBITED = 1;
const short Solution::PERMITED = 0;

Solution &Solution::operator=(const Solution &solution) {
    this->problem = solution.problem;
    this->totalCost = solution.totalCost;
    this->e1Routes.assign(solution.e1Routes.begin(), solution.e1Routes.end());
    this->e2Routes.assign(solution.e2Routes.begin(), solution.e2Routes.end());
    this->satelliteDemands.assign(solution.satelliteDemands.begin(), solution.satelliteDemands.end());
    this->deliveredQ.assign(solution.deliveredQ.begin(), solution.deliveredQ.end());
    this->unroutedCustomers.assign(solution.unroutedCustomers.begin(), solution.unroutedCustomers.end());
    this->satelliteState.assign(solution.satelliteState.begin(), solution.satelliteState.end());
    this->openSatellites = solution.openSatellites;
    this->satelliteAssignedRoutes.assign(solution.satelliteAssignedRoutes.begin(),
                                         solution.satelliteAssignedRoutes.end());
    this->mask = vector<vector<short>>(solution.mask.begin(), solution.mask.end());
    return *this;
}

Solution::Solution(const Solution &solution) {
    this->problem = solution.problem;
    this->totalCost = solution.totalCost;
    this->e1Routes = vector<E1Route>(solution.e1Routes.begin(), solution.e1Routes.end());
    this->e2Routes = vector<E2Route>(solution.e2Routes.begin(), solution.e2Routes.end());
    this->satelliteDemands = vector<int>(solution.satelliteDemands.begin(), solution.satelliteDemands.end());
    this->deliveredQ = vector<int>(solution.deliveredQ.begin(), solution.deliveredQ.end());
    this->unroutedCustomers = deque<int>(solution.unroutedCustomers.begin(), solution.unroutedCustomers.end());
    this->satelliteState = vector<bool>(solution.satelliteState.begin(), solution.satelliteState.end());
    this->satelliteAssignedRoutes = vector<int>(solution.satelliteAssignedRoutes.begin(),
                                                solution.satelliteAssignedRoutes.end());
    this->openSatellites = solution.openSatellites;

    this->mask = vector<vector<short>>(solution.mask.begin(), solution.mask.end());
}

void Solution::print() {
    cout << "Solution" << endl;
    cout << "Total Cost : " << this->getTotalCost() << endl;
    cout << "1st Level vehicles used : " << this->e1Routes.size() << endl;
    cout << "2nd Level vehicles used : " << this->e2Routes.size() << endl;
    cout << endl << "First level routes" << endl;
    // 1st Level Routes
    for (int i = 0; i < this->e1Routes.size(); ++i) {
        cout << "    Route " << i << " : 0";
        for (int satellite : this->e1Routes[i].tour) {
            cout << " S" << satellite + 1;
        }
        cout << " 0" << endl;
        cout << "        Cost : " << this->e1Routes[i].cost << endl;
        cout << "        Load : " << this->e1Routes[i].load << endl;
    }
    // 2nd Level Routes
    cout << endl;
    cout << "Second level routes" << endl;
    for (int i = 0; i < this->e2Routes.size(); ++i) {
        cout << "    Route " << i << " : S" << this->e2Routes[i].departureSatellite + 1;
        for (int client : this->e2Routes[i].tour) {
            cout << " " << client + 1;
        }
        cout << " S" << this->e2Routes[i].departureSatellite + 1 << endl;
        cout << "        Cost : " << this->e2Routes[i].cost << endl;
        cout << "        Load : " << this->e2Routes[i].load << endl;
    }
}

void Solution::saveHumanReadable(const string &fn, const string &header, const bool clrFile) {

    ofstream fh;
    fh.open(fn.c_str(), (clrFile) ? fstream::trunc : (fstream::app | fstream::ate));

    fh << "!----------------------------------------------------------------" << endl;
    fh << header << endl;

    fh << "Total Cost : " << this->getTotalCost() << endl;
    fh << "1st Level vehicles used : " << this->e1Routes.size() << endl;
    fh << "2nd Level vehicles used : " << this->e2Routes.size() << endl;
    fh << endl << "First level routes" << endl;
    // 1st Level Routes
    for (int i = 0; i < this->e1Routes.size(); ++i) {
        fh << "    Route " << i << " : 0";
        for (int satellite : this->e1Routes[i].tour) {
            fh << " S" << satellite + 1;
        }
        fh << " 0" << endl;
        fh << "        Cost : " << this->e1Routes[i].cost << endl;
        fh << "        Load : " << this->e1Routes[i].load << endl;
    }
    // 2nd Level Routes
    fh << endl;
    fh << "Second level routes" << endl;
    for (int i = 0; i < this->e2Routes.size(); ++i) {
        fh << "    Route " << i << " : S" << this->e2Routes[i].departureSatellite + 1;
        for (int client : this->e2Routes[i].tour) {
            fh << " " << client + 1;
        }
        fh << " S" << this->e2Routes[i].departureSatellite + 1 << endl;
        fh << "        Cost : " << this->e2Routes[i].cost << endl;
        fh << "        Load : " << this->e2Routes[i].load << endl;
    }

    fh << "!----------------------------------------------------------------" << endl;
    fh.close();
}

void Solution::doQuickEvaluation() {
    if (this->unroutedCustomers.size()>0) {
        this->totalCost = Config::DOUBLE_INFINITY;
        return;
    }
    this->totalCost = 0;
    for (E2Route &e2route : this->e2Routes) {
        this->totalCost += e2route.cost;
    }
    for (E1Route &e1route : this->e1Routes) {
        this->totalCost += e1route.cost;
    }
}

void Solution::recomputeCost() {
    if (this->unroutedCustomers.size()>0) {
        this->totalCost = Config::DOUBLE_INFINITY;
        return;
    }
    for (E2Route e2route : this->e2Routes) {
        e2route.cost = this->problem->getDistance(this->problem->getSatellite(e2route.departureSatellite),
                                                  this->problem->getClient(e2route.tour[0]))
                       + this->problem->getDistance(this->problem->getSatellite(e2route.departureSatellite),
                                                    this->problem->getClient(
                                                            e2route.tour[e2route.tour.size() - 1]));;
        for (int k = 0; k < e2route.tour.size() - 1; ++k) {
            e2route.cost += this->problem->getDistance(this->problem->getClient(e2route.tour[k]),
                                                       this->problem->getClient(e2route.tour[k + 1]));
        }
    }
    this->doQuickEvaluation();
}

void Solution::setUpMask(double granularityThreshold) {
    /* For each customer c1 do
     *      For each customer c2 different from c1 do
     *          If cost(c1,c2) > granularityThreshold then {
     *              mask(c1,c2) = Prohibited
     *          }
     *          else mask(c1,c2) = Permited
     *      End for
     * End For
     *
     * For each arc a in solution do
     *      mask(a) = Permited
     * End for
     *
     * For each arc a in the Best Solution do
     *      mask(a) = Permitted
     * End For
     * */

    for (int i = 0; i < problem->getClients().size(); ++i) {
        Client c1 = problem->getClient(i);
        for (int j = i + 1; j < problem->getClients().size(); ++j) {
            Client c2 = problem->getClient(j);
            if (problem->getDistance(c1, c2) - granularityThreshold > 0.01) {
                mask[c1.getNodeId()][c2.getNodeId()] = Solution::PROHIBITED;
                mask[c2.getNodeId()][c1.getNodeId()] = Solution::PROHIBITED;
            }
            else {
                mask[c1.getNodeId()][c2.getNodeId()] = Solution::PERMITED;
                mask[c2.getNodeId()][c1.getNodeId()] = Solution::PERMITED;
            }
        }
    }

    for (int i = 0; i < this->e2Routes.size(); ++i) {
        E2Route &route = this->e2Routes[i];
        for (int j = 0; j < route.tour.size() - 1; ++j) {
            mask[problem->getClient(route.tour[j]).getNodeId()][problem->getClient(
                    route.tour[j + 1]).getNodeId()] = Solution::PERMITED;
            mask[problem->getClient(route.tour[j + 1]).getNodeId()][problem->getClient(
                    route.tour[j]).getNodeId()] = Solution::PERMITED;
        }
    }
}

void Solution::setUpMask(Solution &solution) {
    for (int i = 0; i < solution.getE2Routes().size(); ++i) {
        E2Route &route = solution.getE2Routes()[i];
        for (int j = 0; j < route.tour.size() - 1; ++j) {
            mask[problem->getClient(route.tour[j]).getNodeId()][problem->getClient(
                    route.tour[j + 1]).getNodeId()] = Solution::PERMITED;
            mask[problem->getClient(route.tour[j + 1]).getNodeId()][problem->getClient(
                    route.tour[j]).getNodeId()] = Solution::PERMITED;
        }
    }
}

double Solution::computeGranularityThreshold() {
    double thresh = 0;
    for(E2Route &route : this->e2Routes) thresh += route.cost;
    thresh = thresh / (problem->getClients().size()+this->e2Routes.size());
    return thresh * 2.5;
}
