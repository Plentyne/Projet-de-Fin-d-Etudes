//
// Created by Youcef on 29/02/2016.
//

#include <deque>
#include "MoleJamesonHeuristic.h"
#include "../Config.h"
#include "../Utility.h"
#include "LSSolver.h"


// TODO Clean le code
int MoleJamesonHeuristic::getClosestSatellite(E1Route e1Route, const Solution &s) {
    double distance = Config::DOUBLE_INFINITY;
    int closestSatellite = -1;

    for (int i = 0; i < s.getProblem()->getSatellites().size(); i++) {
        if (s.getSatelliteDemands()[i] - s.getDeliveredQ()[i] <= 0) continue;

        if (e1Route.tour.size() > 0) {
            if (s.getProblem()->getDistance(s.getProblem()->getClient(e1Route.tour.back()),
                                            s.getProblem()->getSatellite(i)) < distance) {
                distance = s.getProblem()->getDistance(s.getProblem()->getClient(e1Route.tour.back()),
                                                       s.getProblem()->getSatellite(i));
                closestSatellite = i;
            }
        } else {
            if (s.getProblem()->getDistance(s.getProblem()->getDepot(), s.getProblem()->getSatellite(i)) < distance) {
                distance = s.getProblem()->getDistance(s.getProblem()->getDepot(), s.getProblem()->getSatellite(i));
                closestSatellite = i;
            }
        }
    }
    return closestSatellite;
}

/* Todo Rajouter une recherche locale pour améliorer la solution obtenue
 * Décider de l'endroit où placer la recherche locale :
 *      - Avant l'insertion d'une tournée
 *      - Après la construction de toutes les tournées
 */

/* Adaptation de l'heuristic de Mole and Jameson
 * Noms des fonctions repris de toth et vigo : alpha pour déterminer la meilleure position pour l'insertion et beta pour qui insérer
 *  Proposée par C.Prins
 *  On Deux versions :
 *      - L'une ne prenant en compte que le saving (l'ordre d'insertion est aléatoire)
 *      - L'autre considère le strain pour choisir quel client insérer
 */

struct alphaEnt {
    double alpha;
    int i;
    int j;
    int position;
};

double MoleJamesonHeuristic::alpha(Node i, Node k, Node j) {
    double ik = this->problem->getDistance(i, k);
    double kj = this->problem->getDistance(k, j);
    double ij = this->problem->getDistance(i, j);
    return (ik + kj - (lambda * ij));
}

double MoleJamesonHeuristic::beta(E2Route route, Node i, Node k, Node j) {
    return (mu * this->problem->getDistance(this->problem->getSatellite(route.departureSatellite), k) - alpha(i, k, j));
}

void MoleJamesonHeuristic::solve(Solution &solution) {
    // Si solution vide, créer nouvelle solution
    if (solution.getProblem() == nullptr) {
        solution = Solution(this->problem);
    }

    bool improve = true;
    // Initialiser la liste des clients non routés Todo améliorer cette partie
    deque<int> unroutedClients{};
    vector<int> tmpClients(this->problem->getClients().size(), 0);
    for (E2Route route : solution.getE2Routes()) {
        for (int i : route.tour) tmpClients[i] = 1;
    }
    for (int i = 0; i < tmpClients.size(); i++) if (tmpClients[i] == 0) unroutedClients.push_back(i);
    // initialiser une tournée (s,k,s) avs s un satellite aléatoire et k un client pris aléatoirement
    E2Route e2route;
    int tmpSat = Utility::randomInt(0, this->problem->getSatellites().size() - 1);
    int tmp = Utility::randomInt(0, unroutedClients.size() - 1);
    int tmpCli = unroutedClients[tmp];
    unroutedClients.erase(unroutedClients.begin() + tmp);
    e2route.departureSatellite = tmpSat;
    e2route.tour.push_back(tmpCli);
    e2route.load = this->problem->getClient(tmpCli).getDemand();
    e2route.cost =
            2 * this->problem->getDistance(this->problem->getSatellite(tmpSat), this->problem->getClient(tmpCli));

    // Calculer les coûts d'insertion alpha et beta des clients non routés
    vector<alphaEnt> alphas{this->problem->getClients().size()};
    vector<double> betas = vector<double>(this->problem->getClients().size());
    Node ik;
    Node jk;

    double maxBeta = -Config::DOUBLE_INFINITY;
    double nodeToInsert = -1;

    for (int c = 0; c < unroutedClients.size(); ++c) {
        int k = unroutedClients[c];
        alphas[k].alpha = Config::DOUBLE_INFINITY;
        alphas[k].i = -1;
        alphas[k].j = -1;
        alphas[k].position = -1;
        betas[k] = -Config::DOUBLE_INFINITY;

        if (this->problem->getClient(k).getDemand() > this->problem->getE2Capacity() - e2route.load) continue;

        for (int u = 0; u <= e2route.tour.size(); ++u) {
            if (u == 0) ik = this->problem->getSatellite(e2route.departureSatellite);
            else ik = this->problem->getClient(e2route.tour[u - 1]);

            if (u == e2route.tour.size()) jk = this->problem->getSatellite(e2route.departureSatellite);
            else jk = this->problem->getClient(e2route.tour[u]);

            if (alpha(ik, this->problem->getClient(k), jk) < alphas[k].alpha) {
                alphas[k].alpha = alpha(ik, this->problem->getClient(k), jk);
                alphas[k].i = ik.getNodeId();
                alphas[k].j = jk.getNodeId();
                alphas[k].position = u;
                betas[k] = beta(e2route, ik, this->problem->getClient(k), jk);
                if (betas[k] > maxBeta) {
                    maxBeta = betas[k];
                    nodeToInsert = c;
                }
            }
        }
    }

    int lastInserted;
    // Tant qu'il y a des clients non servis faire
    while (unroutedClients.size() > 0) {
        lastInserted = -1;

        // Si aucune insertion faisable alors
        if (nodeToInsert == -1) {

            // changer le satellite de la tournée actuelle par un meilleur
            double minCost = Config::DOUBLE_INFINITY;
            double tmpCost;
            int position = -1;
            int satellite = -1;
            for (int i = 0; i < this->problem->getSatellites().size(); ++i) {

                for (int j = 0; j < e2route.tour.size(); ++j) {
                    if (j == 0)
                        tmpCost = this->problem->getDistance(problem->getSatellite(i),
                                                             problem->getClient(e2route.tour[0]))
                                  + this->problem->getDistance(problem->getSatellite(i),
                                                               problem->getClient(e2route.tour.back()))
                                  - this->problem->getDistance(problem->getSatellite(e2route.departureSatellite),
                                                               problem->getClient(e2route.tour[0]))
                                  - this->problem->getDistance(problem->getSatellite(e2route.departureSatellite),
                                                               problem->getClient(e2route.tour.back()));

                    else
                        tmpCost = this->problem->getDistance(problem->getSatellite(i),
                                                             problem->getClient(e2route.tour[j - 1]))
                                  + this->problem->getDistance(problem->getSatellite(i),
                                                               problem->getClient(e2route.tour[j]))
                                  - this->problem->getDistance(problem->getSatellite(e2route.departureSatellite),
                                                               problem->getClient(e2route.tour[j - 1]))
                                  - this->problem->getDistance(problem->getSatellite(e2route.departureSatellite),
                                                               problem->getClient(e2route.tour[j]));

                    if (tmpCost < minCost) {
                        minCost = tmpCost;
                        position = j;
                        satellite = i;
                    }
                }
            }
            e2route.cost += minCost;
            e2route.departureSatellite = satellite;
            std::rotate(e2route.tour.begin(), e2route.tour.begin() + position, e2route.tour.end());
            // Insérer la tournée actuelle dans la solution
            // Calculer le nouveau côut de la route
            e2route.cost = this->problem->getDistance(this->problem->getSatellite(e2route.departureSatellite),
                                                      this->problem->getClient(e2route.tour[0]))
                           + this->problem->getDistance(this->problem->getSatellite(e2route.departureSatellite),
                                                        this->problem->getClient(
                                                                e2route.tour[e2route.tour.size() - 1]));;
            for (int k = 0; k < e2route.tour.size() - 1; ++k) {
                e2route.cost += this->problem->getDistance(this->problem->getClient(e2route.tour[k]),
                                                           this->problem->getClient(e2route.tour[k + 1]));
            }
            // Amélioration de la tournée Todo Enlever après
            //LSSolver ls(this->problem);
            //ls.apply2OptOnTour(e2route);
            //ls.applyOrOpt(e2route,2);
            //ls.applyOrOpt(e2route,3);
            //--------------------------------------------

            solution.getE2Routes().push_back(e2route);
            solution.getSatelliteDemands()[e2route.departureSatellite] += e2route.load;
            solution.setTotalCost(solution.getTotalCost() + e2route.cost);
            // initiliasier une nouvelle tournée (s',k',s')
            tmpSat = Utility::randomInt(0, this->problem->getSatellites().size() - 1);
            tmp = Utility::randomInt(0, unroutedClients.size() - 1);
            tmpCli = unroutedClients[tmp];
            unroutedClients.erase(unroutedClients.begin() + tmp);
            e2route.departureSatellite = tmpSat;
            e2route.tour.clear();
            e2route.tour.push_back(tmpCli);
            e2route.load = this->problem->getClient(tmpCli).getDemand();
            e2route.cost = 2 * this->problem->getDistance(this->problem->getSatellite(tmpSat),
                                                          this->problem->getClient(tmpCli));
        }
        else { // Sinon
            // insérer dans la tournée le client k* tq beta(ik*,k*, jk*) = max { beta(ik,k,jk) }
            lastInserted = unroutedClients[nodeToInsert];
            //e2route.cost += alphas[lastInserted].alpha;
            e2route.load += this->problem->getClient(lastInserted).getDemand();
            if (alphas[lastInserted].position == e2route.tour.size()) e2route.tour.push_back(lastInserted);
            else e2route.tour.insert(e2route.tour.begin() + alphas[lastInserted].position, lastInserted);
            // Enlever le client de la liste des clients non routés
            unroutedClients.erase(unroutedClients.begin() + nodeToInsert);

        }
        // FSI
        // Maj les valeurs alpha*(ik, k, jk) = min{ alpha(q,k,p)} pour chaque client k non encore routé et pouvant être inséré dans la tournée actuelle
        maxBeta = -Config::DOUBLE_INFINITY;
        nodeToInsert = -1;
        // Cas d'une nouvelle tournée : il faut tt recalculer
        if (lastInserted == -1) {
            for (int c = 0; c < unroutedClients.size(); ++c) {
                int k = unroutedClients[c];

                alphas[k].alpha = Config::DOUBLE_INFINITY;
                alphas[k].i = -1;
                alphas[k].j = -1;
                alphas[k].position = -1;
                betas[k] = -Config::DOUBLE_INFINITY;

                if (this->problem->getClient(k).getDemand() > this->problem->getE2Capacity() - e2route.load) continue;

                for (int u = 0; u <= e2route.tour.size(); ++u) {
                    if (u == 0) ik = this->problem->getSatellite(e2route.departureSatellite);
                    else ik = this->problem->getClient(e2route.tour[u - 1]);

                    if (u == e2route.tour.size()) jk = this->problem->getSatellite(e2route.departureSatellite);
                    else jk = this->problem->getClient(e2route.tour[u]);

                    if (alpha(ik, this->problem->getClient(k), jk) < alphas[k].alpha) {
                        alphas[k].alpha = alpha(ik, this->problem->getClient(k), jk);
                        alphas[k].i = ik.getNodeId();
                        alphas[k].j = jk.getNodeId();
                        alphas[k].position = u;
                        betas[k] = beta(e2route, ik, this->problem->getClient(k), jk);
                        if (betas[k] > maxBeta) {
                            maxBeta = betas[k];
                            nodeToInsert = c;
                        }
                    }
                }
            }
        } // Sinon : ne tester que les valeurs affectées par la dernière insertion
        else {
            for (int c = 0; c < unroutedClients.size(); ++c) {
                int k = unroutedClients[c];


                if (this->problem->getClient(k).getDemand() > this->problem->getE2Capacity() - e2route.load) continue;
                // On pose : L le dernier noeud inséré entre iL et jL

                // Si le noeud devait être inséré autre part que entre iL et jL
                if ((alphas[lastInserted].i != alphas[k].i) || (alphas[lastInserted].j != alphas[k].j)) {
                    // Décaler le noued s'il devait être inséré après jL
                    if (alphas[k].position > alphas[lastInserted].position) alphas[k].position++;
                    // Récupérer iL et jL
                    if (alphas[lastInserted].position == 0)
                        ik = this->problem->getSatellite(e2route.departureSatellite);
                    else ik = this->problem->getClient(e2route.tour[alphas[lastInserted].position - 1]);

                    if (alphas[lastInserted].position == e2route.tour.size())
                        jk = this->problem->getSatellite(e2route.departureSatellite);
                    else jk = this->problem->getClient(e2route.tour[alphas[lastInserted].position]);
                    // Tester la valeur alpha(iL,k,L)
                    if (alpha(ik, this->problem->getClient(k), this->problem->getClient(lastInserted)) <
                        alphas[k].alpha) {
                        alphas[k].alpha = alpha(ik, this->problem->getClient(k),
                                                this->problem->getClient(lastInserted));
                        alphas[k].i = ik.getNodeId();
                        alphas[k].j = this->problem->getClient(lastInserted).getNodeId();
                        alphas[k].position = alphas[lastInserted].position;
                        betas[k] = beta(e2route, ik, this->problem->getClient(k),
                                        this->problem->getClient(lastInserted));
                    }
                    // Tester la valeur alpha(L,k,jL)
                    if (alpha(this->problem->getClient(lastInserted), this->problem->getClient(k), jk) <
                        alphas[k].alpha) {
                        alphas[k].alpha = alpha(this->problem->getClient(lastInserted), this->problem->getClient(k),
                                                jk);
                        alphas[k].i = this->problem->getClient(lastInserted).getNodeId();
                        alphas[k].j = jk.getNodeId();
                        alphas[k].position = alphas[lastInserted].position + 1;
                        betas[k] = beta(e2route, this->problem->getClient(lastInserted), this->problem->getClient(k),
                                        jk);
                    }

                    // Tester la valeur de beta pour voir s'il doit être inséré
                    if (betas[k] > maxBeta) {
                        maxBeta = betas[k];
                        nodeToInsert = c;
                    }
                }
                else { // Si le noeud devait être inséré entre iL et jL alors le réinitialiser et tout recalculer
                    alphas[k].alpha = Config::DOUBLE_INFINITY;
                    alphas[k].i = -1;
                    alphas[k].j = -1;
                    alphas[k].position = -1;
                    betas[k] = -Config::DOUBLE_INFINITY;

                    for (int u = 0; u <= e2route.tour.size(); ++u) {
                        if (u == 0) ik = this->problem->getSatellite(e2route.departureSatellite);
                        else ik = this->problem->getClient(e2route.tour[u - 1]);

                        if (u == e2route.tour.size()) jk = this->problem->getSatellite(e2route.departureSatellite);
                        else jk = this->problem->getClient(e2route.tour[u]);

                        if (alpha(ik, this->problem->getClient(k), jk) < alphas[k].alpha) {
                            alphas[k].alpha = alpha(ik, this->problem->getClient(k), jk);
                            alphas[k].i = ik.getNodeId();
                            alphas[k].j = jk.getNodeId();
                            alphas[k].position = u;
                            betas[k] = beta(e2route, ik, this->problem->getClient(k), jk);
                            if (betas[k] > maxBeta) {
                                maxBeta = betas[k];
                                nodeToInsert = c;
                            }
                        }
                    }
                }
                // FSI
            }

        }
    }
    // Ftq
    //
    // Insérer la dernière tournée construite dans la solution
    e2route.cost = this->problem->getDistance(this->problem->getSatellite(e2route.departureSatellite),
                                              this->problem->getClient(e2route.tour[0]))
                   + this->problem->getDistance(this->problem->getSatellite(e2route.departureSatellite),
                                                this->problem->getClient(
                                                        e2route.tour[e2route.tour.size() - 1]));;
    for (int k = 0; k < e2route.tour.size() - 1; ++k) {
        e2route.cost += this->problem->getDistance(this->problem->getClient(e2route.tour[k]),
                                                   this->problem->getClient(e2route.tour[k + 1]));
    }
    solution.getE2Routes().push_back(e2route);
    solution.getSatelliteDemands()[e2route.departureSatellite] += e2route.load;
    solution.setTotalCost(solution.getTotalCost() + e2route.cost);

    // Réparer si le nombre de tournées du 2nd echelon est trop grand
    // TODO Feasability Search

    // Amélioration de la tournée Todo Enlever après
    LSSolver ls(this->problem);
    //ls.applySwap(solution);
    int llllll;
    //ls.applyRelocate(solution);
    //ls.applySwap(solution);
    //ls.applyRelocate(solution);
    int imp;
    do {
        imp = solution.getTotalCost();

        ls.applySwap(solution);
        ls.applyRelocate(solution);
        for (E2Route &e2route : solution.getE2Routes()) {
            solution.setTotalCost(solution.getTotalCost() - e2route.cost);

            ls.applyOrOpt(e2route, 1);
            ls.applyOrOpt(e2route, 2);
            ls.applyOrOpt(e2route, 3);
            ls.apply2OptOnTour(e2route);
            solution.setTotalCost(solution.getTotalCost() + e2route.cost);
        }
        //ls.apply2OptOnTour(e2route);
        //ls.applyOrOpt(e2route,2);
        //ls.applyOrOpt(e2route,3);
        imp = solution.getTotalCost() - imp;
    } while (imp < -0.0001);

    //--------------------------------------------

    // Construire les tournées du premier échelon
    // Todo changer par l'heuristiques pour le sdvrp
    E1Route e1Route;
    e1Route.tour.clear();
    e1Route.cost = 0;
    e1Route.load = 0;

    int closestSatellite;
    while ((closestSatellite = MoleJamesonHeuristic::getClosestSatellite(e1Route, solution)) >= 0) {
        if ((e1Route.load + solution.getSatelliteDemands()[closestSatellite] -
             solution.getDeliveredQ()[closestSatellite]) > problem->getE1Capacity()) {
            // Si la demande d'un satellite dépasse celle du véhicule
            // On prend une partie de sa demande et on la charge dans la tournée courantes
            if (solution.getSatelliteDemands()[closestSatellite] - solution.getDeliveredQ()[closestSatellite] >
                problem->getE1Capacity()) {
                solution.getDeliveredQ()[closestSatellite] += (problem->getE1Capacity() - e1Route.load);
                if (e1Route.tour.size() > 0) {
                    e1Route.cost += problem->getDistance(problem->getSatellite(e1Route.tour.back()),
                                                         problem->getSatellite(closestSatellite));
                } else {
                    e1Route.cost += problem->getDistance(problem->getDepot(), problem->getSatellite(closestSatellite));
                }
                e1Route.tour.push_back(problem->getSatellite(closestSatellite).getSatelliteId());
                e1Route.load = problem->getE1Capacity();
            }
            // Sinon

            e1Route.cost += problem->getDistance(problem->getDepot(), problem->getSatellite(e1Route.tour.back()));

            solution.getE1Routes().push_back(e1Route);
            solution.setTotalCost(solution.getTotalCost() + e1Route.cost);

            e1Route.tour.clear();
            e1Route.load = 0;
            e1Route.cost = 0;
        } else {
            // Ajout du satellite dans la tournée actuelle
            if (e1Route.tour.size() > 0) {
                e1Route.cost += problem->getDistance(problem->getSatellite(e1Route.tour.back()),
                                                     problem->getSatellite(closestSatellite));
            } else {
                e1Route.cost += problem->getDistance(problem->getDepot(), problem->getSatellite(closestSatellite));
            }
            e1Route.tour.push_back(problem->getSatellite(closestSatellite).getSatelliteId());
            e1Route.load +=
                    solution.getSatelliteDemands()[closestSatellite] - solution.getDeliveredQ()[closestSatellite];
            solution.getDeliveredQ()[e1Route.tour.back()] = solution.getSatelliteDemands()[closestSatellite];
        }

    }
    // Ajout de la dernière route construite
    e1Route.cost += problem->getDistance(problem->getDepot(), problem->getSatellite(e1Route.tour.back()));
    solution.getE1Routes().push_back(e1Route);
    solution.setTotalCost(solution.getTotalCost() + e1Route.cost);

}