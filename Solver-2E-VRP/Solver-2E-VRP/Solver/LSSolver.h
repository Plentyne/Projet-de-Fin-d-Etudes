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

    double apply2OptOnTour(E2Route &e2Route);
};


#endif //PROJET_DE_FIN_D_ETUDES_LSSOLVER_H
