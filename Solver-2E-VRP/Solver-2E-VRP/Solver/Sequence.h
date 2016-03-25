//
// Created by Youcef on 18/03/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_SPLIT_H
#define PROJET_DE_FIN_D_ETUDES_SPLIT_H

#include "../Model/Solution.h"

struct predecessor {
    int p;
    int sat;
    double cost;
};

class Sequence {
private:
    const Problem *problem;
    vector<double> v;
    vector<int> tour;
    vector<vector<predecessor>> paths;

    bool evaluated;
    double solutionCost;


public:
    /* Construct from solution */

    Sequence(Solution &solution);

/* Construct from a problem and randomly initialize*/
    Sequence(Problem *problem);

/* Compute split cost*/
    double doEvaluate();

    /* Extract Solution from a Sequence*/
    void extractSolution(Solution &solution);

};


#endif //PROJET_DE_FIN_D_ETUDES_SPLIT_H
