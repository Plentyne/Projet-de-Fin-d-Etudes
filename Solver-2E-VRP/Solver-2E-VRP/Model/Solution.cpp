//
// Created by Youcef on 02/02/2016.
//

#include "Solution.h"
#include <fstream>

const bool Solution::OPEN = true;
const bool Solution::CLOSED = false;

Solution &Solution::operator=(const Solution &solution) {
    this->problem = solution.problem;
    this->totalCost = solution.totalCost;
    this->e1Routes = vector<E1Route>(solution.e1Routes.begin(), solution.e1Routes.end());
    this->e2Routes = vector<E2Route>(solution.e2Routes.begin(), solution.e2Routes.end());
    this->satelliteDemands = vector<int>(solution.satelliteDemands.begin(), solution.satelliteDemands.end());
    this->deliveredQ = vector<int>(solution.deliveredQ.begin(), solution.deliveredQ.end());
    this->unroutedCustomers = deque<int>(solution.unroutedCustomers.begin(), solution.unroutedCustomers.end());
    this->satelliteState = vector<bool>(solution.satelliteState.begin(), solution.satelliteState.end());
    this->openSatellites = solution.openSatellites;
    this->satelliteAssignedRoutes = vector<int>(solution.satelliteAssignedRoutes.begin(),
                                                solution.satelliteAssignedRoutes.end());
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
    this->totalCost = 0;
    for (E2Route &e2route : this->e2Routes) {
        this->totalCost += e2route.cost;
    }
    for (E1Route &e1route : this->e1Routes) {
        this->totalCost += e1route.cost;
    }
}

void Solution::recomputeCost() {
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
