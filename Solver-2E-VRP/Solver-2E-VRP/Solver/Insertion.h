//
// Created by Youcef on 11/02/2016.
//

#ifndef PROJET_DE_FIN_D_ETUDES_INSERTION_H
#define PROJET_DE_FIN_D_ETUDES_INSERTION_H

#include "../Model/Solution.h"
#include "SDVRPSolver.h"
#include "LSSolver.h"
#include <deque>

struct insertEntry {
    int clientId;
    int tour;
    int position;
    double cost;
};

class Insertion {
private:
    const Problem *problem;
    SDVRPSolver e1Solver;
    LSSolver lsSolver;
    deque<insertEntry> insertStack{};

    int maxRestart = 10;

    int lastCanceled;
    int cancelations;
    bool mask = false;

    double distanceCost(Solution &solution, int client, int route, int position);

    double biasedDistanceCost(Solution &solution, int client, int route, int position);

    void insertIntoRoute(Solution &solution, int client, int route, int position);

    void insertIntoNewRoute(Solution &solution, int client, int satellite);

    void cancelInsertions(Solution &solution, int client);

    bool apply2OptOnEachTour(Solution &solution);

public:
    Insertion(Problem *problem) : problem(problem) {
        e1Solver = SDVRPSolver(this->problem);
        lsSolver = LSSolver(this->problem);
    }

    bool GreedyInsertionHeuristic(Solution &solution);

    void GreedyInsertionNoiseHeuristic(Solution &solution);

    bool BestInsertionHeuristic(Solution &solution);

    bool solveBestInsertion(Solution &solution);
};


#endif //PROJET_DE_FIN_D_ETUDES_INSERTION_H
