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
    Depot *departure;
    vector<Satellite *> sequence;
    int load;
    double cost;
};

struct E2Route {
    Satellite *departure;
    vector<Client *> sequence;
    int load;
    double cost;
};
class Solution {
    // TODO Implémenter la classe Solution
private:
    Problem problem;
    vector<E1Route> e1Routes; // 1st Echelon Routes
    vector<E2Route> e2Routes; // 2nd Echelon Routes
    vector<int> satelliteDemands; // Satellite Demands (calculated after the 2nd Echelon routes are built)
    vector<int> deliveredQ; // Quantity Delivered to Each satellite

    int totalCost;

public:
    // Constructor
    Solution(const Problem &problem) : problem(problem), e1Routes{}, e2Routes{},
                                       satelliteDemands{problem.getSatellites().size()},
                                       deliveredQ{problem.getSatellites().size()} { }

    // Data Access Methods
    const Problem &getProblem() const {
        return problem;
    }

    const vector<E1Route> &getE1Routes() const {
        return e1Routes;
    }

    const vector<E2Route> &getE2Routes() const {
        return e2Routes;
    }

    const vector<int> &getSatelliteDemands() const {
        return satelliteDemands;
    }

    const vector<int> &getDeliveredQ() const {
        return deliveredQ;
    }

    int getTotalCost() const {
        return totalCost;
    }

    // Methods
    void print();
};


#endif //PROJET_DE_FIN_D_ETUDES_SOLUTION_H
