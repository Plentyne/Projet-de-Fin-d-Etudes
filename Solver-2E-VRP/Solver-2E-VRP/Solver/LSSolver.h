//
// Created by Youcef on 03/03/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_LSSOLVER_H
#define PROJET_DE_FIN_D_ETUDES_LSSOLVER_H

#include "../Model/Solution.h"

class LSSolver {
private:
    const Problem *problem;
    // static int calls;

public:
    LSSolver(const Problem *problem) : problem(problem) { };

    // Mouvements intra-route
    /* Applique 2-opt sur la tournée jusqu'à atteindre un optimum local (Strategie : Best Improvement)*/
    bool apply2OptOnTour(E2Route &e2Route);

    /* Applique Or-opt à la tournée*/
    bool applyOrOpt(E2Route &e2Route, int seqLength);
    /* 3-opt */
    /* GENI Insertion */

    // Mouvements Inter-routes
    /* Shift (parfois appelé Relocate)*/
    /* Swap  (parfois appelé Exchange)*/
    /* 2-opt* */
    /* Cyclic transfer operator*/
};


#endif //PROJET_DE_FIN_D_ETUDES_LSSOLVER_H
