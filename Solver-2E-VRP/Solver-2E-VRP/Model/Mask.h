//
// Created by Youcef on 19/04/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_MASK_H
#define PROJET_DE_FIN_D_ETUDES_MASK_H

#include <vector>
#include "Solution.h"

using namespace std;

class Mask {
private:
    Problem *problem;

    vector<vector<short>> mask;

public:
    /* Arc states */
    static const short PROHIBITED;
    static const short PERMITED;

    /* Functions */
    void setUp(double granularityThreshold, Solution &solution);

    /* Access operators*/
    short& operator()(Node& i, Node& j);
};


#endif //PROJET_DE_FIN_D_ETUDES_MASK_H
