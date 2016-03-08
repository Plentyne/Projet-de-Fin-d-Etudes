//
// Created by Youcef on 11/02/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_INSERTION_H
#define PROJET_DE_FIN_D_ETUDES_INSERTION_H

#include "../Model/Solution.h"

class Insertion {
public:
    static void GreedyInsertionHeuristic(Solution &solution, const Problem *problem);

    static int getClosestSatellite(E1Route e1Route, const Solution &s);
};


#endif //PROJET_DE_FIN_D_ETUDES_INSERTION_H
