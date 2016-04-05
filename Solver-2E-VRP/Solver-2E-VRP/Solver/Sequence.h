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
    vector<vector<predecessor>> paths;

public:
    vector<int> tour;

    bool evaluated;
    double solutionCost;

    /* Construct from solution */
    Sequence(Solution &solution);

    /* Construct from a problem and randomly initialize*/
    Sequence(const Problem *problem);

    /* Copy Constructor : Construction from another Sequence */
    Sequence(Sequence &sequence);

    /* Compute split cost*/
    double doEvaluate();

    double doQuickEvaluation();

    /* Extract Solution from a Sequence*/
    bool extractSolution(Solution &solution);

    /*Local Search*/
    static bool apply2Opt(Sequence &sequence);

    void applyOrOpt();

    static bool applySingleShiftMove(Sequence &sequence);

    bool applySingleSwapMove();

    /* Operators */
    Sequence &operator=(const Sequence &sequence);

};


#endif //PROJET_DE_FIN_D_ETUDES_SPLIT_H
