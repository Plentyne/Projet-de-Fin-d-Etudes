//
// Created by Youcef on 02/02/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_SOLUTION_H
#define PROJET_DE_FIN_D_ETUDES_SOLUTION_H

#include <vector>
#include <deque>
#include <bits/stl_deque.h>

#include "Problem.h"

using namespace std;

class Problem;

struct E1Route {
    vector<int> tour;
    vector<int> satelliteGoods;
    int load;
    double cost;
};

struct E2Route {
    int departureSatellite;
    vector<int> tour;
    int load;
    double cost;
};
class Solution {
    // TODO Implémenter la classe Solution
private:
    // Problem to which the object is a solution
    const Problem *problem;

    // Solution routes
    vector<E1Route> e1Routes; // 1st Echelon Routes
    vector<E2Route> e2Routes; // 2nd Echelon Routes

    // Data for solution validity and other utilities
    vector<int> satelliteDemands; // Satellite Demands (calculated after the 2nd Echelon routes are built)
    vector<int> deliveredQ; // Quantity Delivered to Each satellite

    double totalCost;

public:

    static const bool OPEN;
    static const bool CLOSED;

    deque<int> unroutedCustomers;
    vector<bool> satelliteState; // satelliteState[i]== true means that satellite i is available, else it's closed
    vector<int> satelliteAssignedRoutes; // Number of routes assigned to each satellite
    int openSatellites;
    // Constructor
    Solution() : problem(nullptr), e1Routes(), e2Routes() { }

    Solution(const Problem *problem) : totalCost(0), problem(problem), e1Routes{}, e2Routes{} {
        this->satelliteDemands = vector<int>(static_cast<int>(problem->getSatellites().size()), 0);
        this->deliveredQ = vector<int>(static_cast<int>(problem->getSatellites().size()), 0);
        this->satelliteAssignedRoutes = vector<int>(static_cast<int>(problem->getSatellites().size()), 0);
        for (int i = 0; i < problem->getClients().size(); ++i) {
            this->unroutedCustomers.push_back(problem->getClient(i).getClientId());
        }
        this->satelliteState = vector<bool>(static_cast<int>(problem->getSatellites().size()), OPEN);
        openSatellites = this->satelliteState.size();
    }

    void doQuickEvaluation();

    void recomputeCost();

    // Operators
    Solution &operator=(const Solution &solution);

    Solution(const Solution &solution);
    // Data Access Methods
    const Problem *getProblem() const {
        return problem;
    }

    vector<E1Route> &getE1Routes() {
        return e1Routes;
    }

    const vector<E1Route> &getE1Routes() const {
        return e1Routes;
    }

    vector<E2Route> &getE2Routes() {
        return e2Routes;
    }

    const vector<E2Route> &getE2Routes() const {
        return e2Routes;
    }

    vector<int> &getSatelliteDemands() {
        return satelliteDemands;
    }

    const vector<int> &getSatelliteDemands() const {
        return satelliteDemands;
    }

    vector<int> &getDeliveredQ() {
        return deliveredQ;
    }

    const vector<int> &getDeliveredQ() const {
        return deliveredQ;
    }

    double getTotalCost() const {
        return totalCost;
    }

    void setTotalCost(double totalCost) {
        Solution::totalCost = totalCost;
    }


    // Methods
    /** print solution to the console */ void print();

    /** save the solution to file */ void saveHumanReadable(const string &fn, const string &header = "",
                                                            const bool clrFile = true);
};


#endif //PROJET_DE_FIN_D_ETUDES_SOLUTION_H
