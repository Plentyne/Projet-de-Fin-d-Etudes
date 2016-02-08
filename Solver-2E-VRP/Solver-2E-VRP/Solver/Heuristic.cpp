//
// Created by Youcef on 05/02/2016.
//

#include "Heuristic.h"
#include "../Model/Problem.h"
#include "../Model/Solution.h"
#include "../Config.h"

/* TODO Review simpleHeuristic
 *      - Il arrive que le résultat de l'heuristique change
 *      - Peut causer la fin anormale du programme : certainement en relation avcv les warnings pour l'instanciation des tableaux
 *      - Revoir l'accès aux vecteurs de E1Route et de Solution et l'unifier
 *      - Mettre à jour les valeurs du Tableau Goods de E1Route pour garder trace de ui transporte quoi
*/
int getClosestClient(E2Route e2Route, const Problem &p, vector<int> &clientAffectation) {
    double distance = Config::DOUBLE_INFINITY;
    int closestClient = -1;

    for (int i = 0; i < p.getClients().size(); i++) {
        if (e2Route.departure.getSatelliteId() != clientAffectation[i]) continue;

        if (e2Route.sequence.size() > 0) {
            if (p.getDistance( e2Route.sequence.back(), p.getClient(i)) < distance) {
                distance = p.getDistance( e2Route.sequence.back(), p.getClient(i));
                closestClient = i;
            }
        } else {
            if (p.getDistance( e2Route.departure, p.getClient(i)) < distance) {
                distance = p.getDistance( e2Route.departure, p.getClient(i));
                closestClient = i;
            }
        }
    }
    if (closestClient >= 0) clientAffectation[closestClient] = -1;
    return closestClient;
}

int getClosestSatellite(Client c, const Problem &p) {
    double distance = Config::DOUBLE_INFINITY;
    int closestSatellite;

    for (int i = 0; i < p.getSatellites().size(); i++) {
        if (p.getDistance(c, p.getClient(i)) < distance) {
            distance = p.getDistance(c, p.getClient(i));
            closestSatellite = i;
        }
    }

    return closestSatellite;
}

int getClosestSatellite(E1Route e1Route, const Solution &s) {
    double distance = Config::DOUBLE_INFINITY;
    int closestSatellite = -1;

    for (int i = 0; i < s.getProblem()->getSatellites().size(); i++) {
        if (s.getSatelliteDemands()[i] - s.getDeliveredQ()[i] <= 0) continue;

        if (e1Route.sequence.size() > 0) {
            if (s.getProblem()->getDistance( e1Route.sequence.back(), s.getProblem()->getSatellite(i)) < distance) {
                distance = s.getProblem()->getDistance( e1Route.sequence.back(), s.getProblem()->getSatellite(i));
                closestSatellite = i;
            }
        } else {
            if (s.getProblem()->getDistance( e1Route.departure, s.getProblem()->getSatellite(i)) < distance) {
                distance = s.getProblem()->getDistance( e1Route.departure, s.getProblem()->getSatellite(i));
                closestSatellite = i;
            }
        }
    }
    return closestSatellite;
}

void Heuristic::simpleHeuristic(const Problem p, Solution &solution) {
    cout << "Start Heuristic" << endl;
    // Instanciation de la solution
    solution = Solution(&p);
    // Construction des tournées du 1er échelon
    // Affectation des clients aux satellties
    vector<int> clientsAffectation{static_cast<int>(p.getClients().size())};
    for (int i = 0; i < p.getClients().size(); ++i) {
        clientsAffectation[i] = getClosestSatellite(p.getClient(i), p);
    }

    // Calcul des tournées
    int closestClient;
    E2Route e2Route;
    e2Route.sequence.clear();
    e2Route.load = 0;
    e2Route.cost = 0;
    for(Satellite satellite : p.getSatellites()) {
        e2Route.departure = satellite;
        solution.getSatelliteDemands()[e2Route.departure.getSatelliteId()] = 0;
        while ((closestClient = getClosestClient(e2Route, p, clientsAffectation)) >= 0) {
            if ((e2Route.load + p.getClient(closestClient).getDemand()) > p.getE2Capacity()) {
                e2Route.cost += p.getDistance( e2Route.departure, e2Route.sequence.back());

                solution.getE2Routes().push_back(e2Route);
                solution.getSatelliteDemands()[e2Route.departure.getSatelliteId()] += e2Route.load;
                solution.setTotalCost(solution.getTotalCost() + e2Route.cost);

                e2Route.sequence.clear();
                e2Route.load = 0;
                e2Route.departure = satellite;
                e2Route.cost = 0;
            }

            if (e2Route.sequence.size() > 0) {
                e2Route.cost += p.getDistance( e2Route.sequence.back(), p.getClient(closestClient));
            } else {
                e2Route.cost += p.getDistance( e2Route.departure, p.getClient(closestClient));
            }
            e2Route.sequence.push_back( p.getClient(closestClient));
            e2Route.load += p.getClient(closestClient).getDemand();

        }

        if (e2Route.sequence.size() > 0) {
            e2Route.cost += p.getDistance( e2Route.departure, e2Route.sequence.back());

            solution.getE2Routes().push_back(e2Route);
            solution.getSatelliteDemands()[e2Route.departure.getSatelliteId()] += e2Route.load;
            solution.setTotalCost(solution.getTotalCost() + e2Route.cost);

        }
        e2Route.sequence.clear();
        e2Route.load = 0;
        e2Route.departure = satellite;
        e2Route.cost = 0;
    }
    // Construction des tournées du 1er échelon
    E1Route e1Route;
    e1Route.departure = p.getDepot();
    e1Route.sequence.clear();
    e1Route.cost = 0;
    e1Route.load = 0;

    for (int j = 0; j < solution.getDeliveredQ().size(); ++j) {
        solution.getDeliveredQ()[j] = 0;
    }

    int closestSatellite;
    while ((closestSatellite = getClosestSatellite(e1Route, solution)) >= 0) {
        if ((e1Route.load + solution.getSatelliteDemands()[closestSatellite] -
             solution.getDeliveredQ()[closestSatellite]) > p.getE1Capacity()) {
            // Si la demande d'un satellite dépasse celle du véhicule
            // On prend une partie de sa demande et on la charge dans la tournée courantes
            if (solution.getSatelliteDemands()[closestSatellite] - solution.getDeliveredQ()[closestSatellite] >
                p.getE1Capacity()) {
                solution.getDeliveredQ()[closestSatellite] += (p.getE1Capacity() - e1Route.load);
                if (e1Route.sequence.size() > 0) {
                    e1Route.cost += p.getDistance( e1Route.sequence.back(), p.getSatellite(closestSatellite));
                } else {
                    e1Route.cost += p.getDistance( e1Route.departure, p.getSatellite(closestSatellite));
                }
                e1Route.sequence.push_back( p.getSatellite(closestSatellite));
                e1Route.load = p.getE1Capacity();
            }
            // Sinon

            e1Route.cost += p.getDistance( e1Route.departure,  e1Route.sequence.back());

            solution.getE1Routes().push_back(e1Route);
            solution.setTotalCost(solution.getTotalCost() + e1Route.cost);

            e1Route.sequence.clear();
            e1Route.load = 0;
            e1Route.cost = 0;
        }
        // Ajout du satellite dans la tournée actuelle
        if (e1Route.sequence.size() > 0) {
            e1Route.cost += p.getDistance( e1Route.sequence.back(), p.getSatellite(closestSatellite));
        } else {
            e1Route.cost += p.getDistance( e1Route.departure, p.getSatellite(closestSatellite));
        }
        e1Route.sequence.push_back( p.getSatellite(closestSatellite));
        e1Route.load +=
                solution.getSatelliteDemands()[closestSatellite] - solution.getDeliveredQ()[closestSatellite];
        solution.getDeliveredQ()[e1Route.sequence.back().getSatelliteId()] = solution.getSatelliteDemands()[closestSatellite];

    }
    // Ajout de la dernière route construite
    e1Route.cost += p.getDistance(e1Route.departure, e1Route.sequence.back());

    solution.getE1Routes().push_back(e1Route);
    solution.setTotalCost(solution.getTotalCost() + e1Route.cost);

    cout << "End Heuristic" << endl;
}