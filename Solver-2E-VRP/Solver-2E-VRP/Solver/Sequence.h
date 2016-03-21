//
// Created by Youcef on 18/03/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_SPLIT_H
#define PROJET_DE_FIN_D_ETUDES_SPLIT_H

#include "../Model/Solution.h"

class Sequence {
private:
    const Problem *problem;
    vector<int> p;
    vector<double> v;
    vector<int> N;
    vector<int> satellites;
    vector<int> tour;

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
