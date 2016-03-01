//
// Created by Youcef on 29/02/2016.
//

#include <deque>
#include "MoleJamesonHeuristic.h"
#include "../Config.h"
#include "../Utility.h"


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
    return (this->problem->getDistance(i, k) + this->problem->getDistance(k, j) -
            lambda * this->problem->getDistance(i, j));
}

double MoleJamesonHeuristic::beta(E2Route route, Node i, Node k, Node j) {
    return (mu * this->problem->getDistance(this->problem->getSatellite(route.departureSatellite), k) - alpha(i, k, j));
}

void MoleJamesonHeuristic::solve(Solution &solution) {
    // Si solution vide, créer nouvelle solution
    if (solution.getProblem() == nullptr) {
        solution = Solution(this->problem);
    }

    this->problem = solution.getProblem();

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
        Client tttt = this->problem->getClient(k);
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

            if (alpha(ik, tttt, jk) < alphas[k].alpha) {
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
            // Todo
            // Insérer la tournée actuelle dans la solution
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
            e2route.cost += alphas[lastInserted].alpha;
            e2route.load += this->problem->getClient(lastInserted).getDemand();
            if (alphas[lastInserted].position == e2route.tour.size()) e2route.tour.push_back(lastInserted);
            else e2route.tour.insert(e2route.tour.begin() + alphas[lastInserted].position, lastInserted);
            // Enlever le client de la liste des clients non routés
            unroutedClients.erase(unroutedClients.begin() + nodeToInsert);
            // Optimiser la tournée actuelle avec 3-opt
            // Todo
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

                int lllll = this->problem->getClient(k).getDemand();
                int ggggg = this->problem->getE2Capacity() - e2route.load;

                bool test = lllll > ggggg;
                bool test2 = (this->problem->getClient(k).getDemand() > this->problem->getE2Capacity() - e2route.load);

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
                    else jk = this->problem->getClient(e2route.tour[alphas[lastInserted].position + 1]);
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
    // Insérer la dernière tournée construi dans la solution
    solution.getE2Routes().push_back(e2route);
    solution.getSatelliteDemands()[e2route.departureSatellite] += e2route.load;
    solution.setTotalCost(solution.getTotalCost() + e2route.cost);
    // Réparer si le nombre de tournées du 2nd echelon est trop grand
    //
    // Construire les tournées du premier échelon
}