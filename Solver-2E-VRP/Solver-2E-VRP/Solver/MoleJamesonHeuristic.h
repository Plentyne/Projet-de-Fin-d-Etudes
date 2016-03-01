//
// Created by Youcef on 29/02/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_MOLEJAMESONHEURISTIC_H
#define PROJET_DE_FIN_D_ETUDES_MOLEJAMESONHEURISTIC_H

#include "../Model/Solution.h"


class MoleJamesonHeuristic {
public:
    MoleJamesonHeuristic(const Problem *problem, double mu, double lambda) : problem(problem), mu(mu),
                                                                             lambda(lambda) { };

    void solve(Solution &solution);

private:
    const Problem *problem;

    // Paramètres de la méthode
    double mu;
    double lambda;

    // Focntions
    double alpha(Node i, Node k, Node j);

    double beta(E2Route route, Node i, Node k, Node j);

    // TODO Remove
    int getClosestSatellite(E1Route e1Route, const Solution &s);


};


#endif //PROJET_DE_FIN_D_ETUDES_MOLEJAMESONHEURISTIC_H
