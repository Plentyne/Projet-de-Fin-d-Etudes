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
    this->v = vector<double>(this->problem->getClients().size() + 1);
    //this->paths = vector<vector<predecessor>>(this->problem->getClients().size() + 1);
}

Sequence::Sequence(const Problem *problem) : problem(problem), evaluated(false), solutionCost(0) {
    this->tour.reserve(problem->getClients().size() + 1);
    this->tour.push_back(-1);
    for (int i = 0; i < problem->getClients().size(); ++i) {
        this->tour.push_back(i);
    }
    std::random_shuffle(tour.begin() + 1, tour.end());
    this->v = vector<double>(this->problem->getClients().size() + 1);
    //this->paths = vector<vector<predecessor>>(this->problem->getClients().size() + 1);
}

Sequence::Sequence(Sequence &sequence) {
    this->problem = sequence.problem;
    this->evaluated = sequence.evaluated;
    this->solutionCost = sequence.solutionCost;
    this->tour.assign(sequence.tour.begin(), sequence.tour.end());
    this->v.assign(sequence.v.begin(), sequence.v.end());
    this->paths.assign(sequence.paths.begin(), sequence.paths.end());
}

Sequence &Sequence::operator=(const Sequence &sequence) {
    this->problem = sequence.problem;
    this->evaluated = sequence.evaluated;
    this->solutionCost = sequence.solutionCost;
    this->tour.assign(sequence.tour.begin(), sequence.tour.end());
    this->v.assign(sequence.v.begin(), sequence.v.end());
    this->paths.assign(sequence.paths.begin(), sequence.paths.end());
    return *this;
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
    std::fill(v.begin(), v.end(), Config::DOUBLE_INFINITY);
    v[0] = 0;
    vector<double> v2(v.begin(), v.end());

    paths = vector<vector<predecessor>>(this->problem->getClients().size() + 1);
    paths[0].push_back(predecessor{0, 0, 0});
    vector<vector<predecessor>> paths2(paths.begin(), paths.end());

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
                            if (tmpCost - cost1 < -0.01) {
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
                            if (tmpCost - cost1 < -0.01) {
                                cost1 = tmpCost;
                                sat = l;
                            }
                        }
                        cost2 += problem->getDistance(problem->getClient(tour[j - 1]), problem->getClient(tour[j]));
                    }
                    if ((v[i - 1] + cost1 + cost2 - v2[j] < -0.01)) {
                        v2[j] = v[i - 1] + cost1 + cost2;
                        stable = false;
                        paths2[j].assign(paths[i - 1].begin(), paths[i - 1].end());
                        paths2[j].push_back(predecessor{j, sat, v2[j]});
                    }
                    j++;
                }
            } while (load <= problem->getE2Capacity() && j <= n);
        }
        v.assign(v2.begin(), v2.end());
        paths.assign(paths2.begin(), paths2.end());
    } while (k < problem->getK2() && !stable);

    this->evaluated = true;
    this->solutionCost = v[n];

    return this->solutionCost;
}

double Sequence::doQuickEvaluation() {
    std::fill(v.begin(), v.end(), Config::DOUBLE_INFINITY);
    v[0] = 0;
    vector<double> v2(v.begin(), v.end());

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
                            if (tmpCost - cost1 < -0.01) {
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
                            if (tmpCost - cost1 < -0.01) {
                                cost1 = tmpCost;
                                sat = l;
                            }
                        }
                        cost2 += problem->getDistance(problem->getClient(tour[j - 1]), problem->getClient(tour[j]));
                    }
                    if ((v[i - 1] + cost1 + cost2 - v2[j] < -0.01)) {
                        v2[j] = v[i - 1] + cost1 + cost2;
                        stable = false;
                    }
                    j++;
                }
            } while (load <= problem->getE2Capacity() && j <= n);
        }
        v.assign(v2.begin(), v2.end());
    } while (k < problem->getK2() && !stable);

    this->solutionCost = v[n];

    return this->solutionCost;
}

/****************************
 *
 * EXTRACT SOLUTION
 *
 ****************************/
bool Sequence::extractSolution(Solution &solution) {

    if (!evaluated) this->doEvaluate();

    if (this->solutionCost == Config::DOUBLE_INFINITY) return false;

    int j = paths.size() - 1;
    int i;

    solution = Solution(problem);
    solution.setTotalCost(solutionCost);

    E2Route route;

    vector<predecessor> path(paths[j].begin(), paths[j].end());
    std::reverse(path.begin(), path.end());

    for (int l = 0; l < path.size() - 1; ++l) {
        // Initialiser la route
        route.departureSatellite = path[l].sat;
        route.tour.clear();
        route.load = 0;
        // Construire la route
        i = path[l + 1].p;

        for (int k = i + 1; k <= path[l].p; ++k) {
            route.tour.push_back(tour[k]);
            route.load += problem->getClient(tour[k]).getDemand();
        }
        route.cost = path[l].cost - path[l + 1].cost;
        // Insert route into solution
        solution.getE2Routes().push_back(route);
        solution.getSatelliteDemands()[route.departureSatellite] += route.load;
    }
    // Résoudre le premier échelon
    SDVRPSolver sdvrpSolver(problem);
    sdvrpSolver.constructiveHeuristic(solution);

    return true;
}

bool Sequence::apply2Opt(Sequence &sequence) {
    if (sequence.tour.size() <= 2) return false;

    Sequence testSeq(sequence);
    double improved;
    int u, v;
    double dxu, dvy, dxv, duy;
    bool improvement = false;
    bool improvementFound;

    Solution bestSolution, tmpSol;
    sequence.extractSolution(bestSolution);

    do {

        improvementFound = false;
        for (u = 1; u < testSeq.tour.size() && !improvementFound; u++) { // inner loop
            if (u == 1)
                dxu = testSeq.problem->getDistance(testSeq.problem->getClient(testSeq.tour.back()),
                                                   testSeq.problem->getClient(testSeq.tour[u]));
            else
                dxu = testSeq.problem->getDistance(testSeq.problem->getClient(testSeq.tour[u - 1]),
                                                   testSeq.problem->getClient(testSeq.tour[u]));

            for (v = u + 1; v < testSeq.tour.size() && !improvementFound; v++) {
                if (v == testSeq.tour.size() - 1)
                    dvy = testSeq.problem->getDistance(testSeq.problem->getClient(testSeq.tour[v]),
                                                       testSeq.problem->getClient(testSeq.tour[1]));
                else
                    dvy = testSeq.problem->getDistance(testSeq.problem->getClient(testSeq.tour[v]),
                                                       testSeq.problem->getClient(testSeq.tour[v + 1]));

                if (u == 1)
                    dxv = testSeq.problem->getDistance(testSeq.problem->getClient(testSeq.tour.back()),
                                                       testSeq.problem->getClient(testSeq.tour[v]));
                else
                    dxv = testSeq.problem->getDistance(testSeq.problem->getClient(testSeq.tour[u - 1]),
                                                       testSeq.problem->getClient(testSeq.tour[v]));

                if (v == testSeq.tour.size() - 1)
                    duy = testSeq.problem->getDistance(testSeq.problem->getClient(testSeq.tour[u]),
                                                       testSeq.problem->getClient(testSeq.tour[1]));
                else
                    duy = testSeq.problem->getDistance(testSeq.problem->getClient(testSeq.tour[u]),
                                                       testSeq.problem->getClient(testSeq.tour[v + 1]));

                improved = dxv + duy - (dxu + dvy);

                if (improved < -0.01) {
                    improvementFound = true;
                    // inverse sequence order between u and v
                    std::reverse(testSeq.tour.begin() + u, testSeq.tour.begin() + v + 1);
                    testSeq.evaluated = false;
                    testSeq.extractSolution(tmpSol);
                    double ttt = testSeq.doQuickEvaluation();
                    double lll = sequence.doQuickEvaluation();
                    if (tmpSol.getTotalCost() - bestSolution.getTotalCost() < -0.01) {
                        improvement = true;
                        sequence = testSeq;
                        bestSolution = tmpSol;
                    }
                }
            }
        }

    } while (improvementFound);

    return improvement;
}

void Sequence::applyOrOpt() {

}

bool Sequence::applySingleShiftMove(Sequence &sequence) {
    int i, j, n = (int) sequence.tour.size();
    if (n < 2) return false;


    for (i = 1; i < n; i++) { // take for all customer i of the sequence
        // First, try to move it to the left
        Sequence leftTest(sequence);
        for (j = i - 1; j >= 1; j--) {
            swap(leftTest.tour[j], leftTest.tour[j + 1]); // take the change
            leftTest.doQuickEvaluation();

            // improvement found ?
            if (leftTest.solutionCost - sequence.solutionCost < -0.01) {
                sequence = leftTest;
                sequence.evaluated = false;
                return true;
            }
            // the next test, we keep moving the client i, so we do not need a reset here
        }

        // Try to move it to the right
        Sequence rightTest(sequence);
        for (j = i + 1; j < n; j++) {
            swap(rightTest.tour[j], rightTest.tour[j - 1]); // take the change
            rightTest.doQuickEvaluation();

            // improvement found ?
            if (rightTest.solutionCost - rightTest.solutionCost < -0.01) {
                sequence = rightTest;
                sequence.evaluated = false;
                return true;
            }
            // the next test, we keep moving the client i, so we do not need a reset here
        }
    }

    return false;
}

bool Sequence::applySingleSwapMove() {
    return false;
}


