//
// Created by Youcef on 18/03/2016.
//

#include "Sequence.h"
#include "../Config.h"
#include "SDVRPSolver.h"

#include <algorithm>

/******************************
 *
 *       CONSTRUCTORS
 *
 ******************************/
Sequence::Sequence(Solution &solution) : problem(solution.getProblem()), evaluated(false), solutionCost(0) {
    this->tour.reserve(problem->getClients().size() + 1);
    this->tour.push_back(-1);
    for (E2Route &route : solution.getE2Routes()) {
        tour.insert(tour.end(), route.tour.begin(), route.tour.end());
    }
    this->p = vector<int>(problem->getClients().size() + 1);
    this->satellites = vector<int>(problem->getClients().size() + 1);
    this->N = vector<int>(problem->getClients().size() + 1);
}

Sequence::Sequence(Problem *problem) : problem(problem), evaluated(false), solutionCost(0) {
    this->tour.reserve(problem->numberOfClients() + 1);
    this->tour.push_back(-1);
    for (int i = 0; i < problem->getClients().size(); ++i) {
        this->tour.push_back(i);
    }
    std::random_shuffle(tour.begin() + 1, tour.end());
    this->p = vector<int>(problem->getClients().size() + 1);
    this->satellites = vector<int>(problem->getClients().size() + 1);
    this->N = vector<int>(problem->getClients().size() + 1);
}

/****************************
 *
 * EVALUATION FUNCTION
 *
 ****************************/
double Sequence::doEvaluate() {
    // If the sequence has been evaluated, it's no use to do it again
    if (this->evaluated)
        return this->solutionCost;

    // Evaluate the sequence
    // Initialisation
    v = vector<double>(this->problem->getClients().size() + 1, Config::DOUBLE_INFINITY);
    v[0] = 0;
    N[0] = 0;
    vector<double> v2(v.begin(), v.end());
    vector<int> p2(p.begin(), p.end());
    vector<int> satellites2(satellites.begin(), satellites.end());

    // External loop for the number of vehicles
    int k = 0;
    int i, j;
    int n = v.size() - 1;
    bool stable;
    int load;
    double cost1, cost2;
    int sat;

    do {
        k++;
        stable = true;

        for (i = 1; i <= n && v[i - 1] < Config::DOUBLE_INFINITY; ++i) {
            load = 0;
            j = i;

            do {
                load += problem->getClient(tour[j]).getDemand();
                if (load <= problem->getE2Capacity()) {
                    if (j == i) {
                        cost1 = 2 * problem->getDistance(problem->getClient(tour[i]), problem->getSatellite(0));
                        if (cost1 == 0) cost1 = Config::DOUBLE_INFINITY;
                        sat = 0;
                        for (int l = 1; l < problem->getSatellites().size(); ++l) {
                            double tmpCost =
                                    2 * problem->getDistance(problem->getClient(tour[i]), problem->getSatellite(l));
                            if (tmpCost == 0) tmpCost = Config::DOUBLE_INFINITY;
                            if (tmpCost - cost1 < -0.0001) {
                                cost1 = tmpCost;
                                sat = l;
                            }
                        }
                        cost2 = 0;
                    }
                    else {
                        cost1 = problem->getDistance(problem->getClient(tour[i]), problem->getSatellite(0))
                                + problem->getDistance(problem->getClient(tour[j]), problem->getSatellite(0));
                        sat = 0;
                        for (int l = 1; l < problem->getSatellites().size(); ++l) {
                            double tmpCost = problem->getDistance(problem->getClient(tour[i]), problem->getSatellite(l))
                                             + problem->getDistance(problem->getClient(tour[j]),
                                                                    problem->getSatellite(l));
                            if (tmpCost - cost1 < -0.0001) {
                                cost1 = tmpCost;
                                sat = l;
                            }
                        }
                        cost2 += problem->getDistance(problem->getClient(tour[j - 1]), problem->getClient(tour[j]));
                    }
                    if ((v[i - 1] + cost1 + cost2 - v2[j] < -0.0001)) {
                        v2[j] = v[i - 1] + cost1 + cost2;
                        stable = false;
                        p2[j] = i - 1;
                        satellites2[j] = sat;
                    }
                    j++;
                }
            } while (load <= problem->getE2Capacity() && j <= n);
        }
        v.assign(v2.begin(), v2.end());
        p.assign(p2.begin(), p2.end());
        satellites.assign(satellites2.begin(), satellites2.end());
    } while (k < problem->getK2() && !stable);

    this->evaluated = true;
    this->solutionCost = v[n];

    return this->solutionCost;
}

/****************************
 *
 * EXTRACT SOLUTION
 *
 ****************************/
void Sequence::extractSolution(Solution &solution) {
    int j = p.size() - 1;
    int i;

    if (!evaluated) this->doEvaluate();

    solution = Solution(problem);
    solution.setTotalCost(solutionCost);

    E2Route route;

    do {
        // Initialiser la route
        route.departureSatellite = satellites[j];
        route.tour.clear();
        route.load = 0;
        // Construire la route
        i = p[j];
        for (int k = i + 1; k <= j; ++k) {
            route.tour.push_back(tour[k]);
            route.load += problem->getClient(tour[k]).getDemand();
        }
        route.cost = v[j] - v[i];
        // Insert route into solution
        solution.getE2Routes().push_back(route);
        solution.getSatelliteDemands()[route.departureSatellite] += route.load;
        // Passer à la tournée suivante
        j = i;
    } while (i > 0);
    // Résoudre le premier échelon
    SDVRPSolver sdvrpSolver(problem);
    sdvrpSolver.constructiveHeuristic(solution);
}
