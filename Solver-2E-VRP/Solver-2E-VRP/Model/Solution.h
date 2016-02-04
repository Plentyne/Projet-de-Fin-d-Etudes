//
// Created by Youcef on 02/02/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_SOLUTION_H
#define PROJET_DE_FIN_D_ETUDES_SOLUTION_H

#include <vector>

#include "Problem.h"

using namespace std;

class Problem;

struct E1Route {
    Depot departure;
    vector<Satellite> sequence;
    int load;
    double cost;
};

struct E2Route {
    Satellite departure;
    vector<Client> sequence;
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
    vector<short> routedCustomers;

    double totalCost;

public:
    // Constructor
    Solution(const Problem *problem) : problem(problem), e1Routes{}, e2Routes{},
                                       satelliteDemands{problem->getSatellites().size()},
                                       deliveredQ{problem->getSatellites().size()},
                                       routedCustomers{problem->getClients().size()} { }

    // Data Access Methods
    const Problem *getProblem() const {
        return problem;
    }

    vector<E1Route> &getE1Routes() {
        return e1Routes;
    }

    vector<E2Route> &getE2Routes() {
        return e2Routes;
    }

    vector<int> &getSatelliteDemands() {
        return satelliteDemands;
    }

    vector<int> &getDeliveredQ() {
        return deliveredQ;
    }

    double getTotalCost() const {
        return totalCost;
    }

    void setTotalCost(double totalCost) {
        Solution::totalCost = totalCost;
    }


    // Methods
    void print();
};


#endif //PROJET_DE_FIN_D_ETUDES_SOLUTION_H
