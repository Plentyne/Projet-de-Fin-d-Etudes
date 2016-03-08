//
// Created by Youcef on 05/02/2016.
//
#include <algorithm>

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
        if (e2Route.departureSatellite != clientAffectation[i]) continue;

        if (e2Route.tour.size() > 0) {
            if (p.getDistance(p.getClient(e2Route.tour.back()), p.getClient(i)) < distance) {
                distance = p.getDistance(p.getClient(e2Route.tour.back()), p.getClient(i));
                closestClient = i;
            }
        } else {
            if (p.getDistance(p.getSatellite(e2Route.departureSatellite), p.getClient(i)) < distance) {
                distance = p.getDistance(p.getSatellite(e2Route.departureSatellite), p.getClient(i));
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

void Heuristic::simpleHeuristic(const Problem p, Solution &solution) {
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
    e2Route.tour.clear();
    e2Route.load = 0;
    e2Route.cost = 0;
    for(Satellite satellite : p.getSatellites()) {
        e2Route.departureSatellite = satellite.getSatelliteId();
        solution.getSatelliteDemands()[e2Route.departureSatellite] = 0;
        while ((closestClient = getClosestClient(e2Route, p, clientsAffectation)) >= 0) {
            if ((e2Route.load + p.getClient(closestClient).getDemand()) > p.getE2Capacity()) {
                e2Route.cost += p.getDistance(satellite, p.getClient(e2Route.tour.back()));

                solution.getE2Routes().push_back(e2Route);
                solution.getSatelliteDemands()[e2Route.departureSatellite] += e2Route.load;
                solution.setTotalCost(solution.getTotalCost() + e2Route.cost);

                e2Route.tour.clear();
                e2Route.load = 0;
                e2Route.departureSatellite = satellite.getSatelliteId();
                e2Route.cost = 0;
            }

            if (e2Route.tour.size() > 0) {
                e2Route.cost += p.getDistance(p.getClient(e2Route.tour.back()), p.getClient(closestClient));
            } else {
                e2Route.cost += p.getDistance(satellite, p.getClient(closestClient));
            }
            e2Route.tour.push_back(p.getClient(closestClient).getClientId());
            e2Route.load += p.getClient(closestClient).getDemand();

        }

        if (e2Route.tour.size() > 0) {
            e2Route.cost += p.getDistance(p.getSatellite(e2Route.departureSatellite), p.getClient(e2Route.tour.back()));

            solution.getE2Routes().push_back(e2Route);
            solution.getSatelliteDemands()[e2Route.departureSatellite] += e2Route.load;
            solution.setTotalCost(solution.getTotalCost() + e2Route.cost);

        }
        e2Route.tour.clear();
        e2Route.load = 0;
        e2Route.departureSatellite = satellite.getSatelliteId();
        e2Route.cost = 0;
    }
    // Construction des tournées du 1er échelon
    E1Route e1Route;
    e1Route.tour.clear();
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
                if (e1Route.tour.size() > 0) {
                    e1Route.cost += p.getDistance(p.getSatellite(e1Route.tour.back()),
                                                  p.getSatellite(closestSatellite));
                } else {
                    e1Route.cost += p.getDistance(p.getDepot(), p.getSatellite(closestSatellite));
                }
                e1Route.tour.push_back(p.getSatellite(closestSatellite).getSatelliteId());
                e1Route.load = p.getE1Capacity();
            }
            // Sinon

            e1Route.cost += p.getDistance(p.getDepot(), p.getSatellite(e1Route.tour.back()));

            solution.getE1Routes().push_back(e1Route);
            solution.setTotalCost(solution.getTotalCost() + e1Route.cost);

            e1Route.tour.clear();
            e1Route.load = 0;
            e1Route.cost = 0;
        }
        // Ajout du satellite dans la tournée actuelle
        if (e1Route.tour.size() > 0) {
            e1Route.cost += p.getDistance(p.getSatellite(e1Route.tour.back()), p.getSatellite(closestSatellite));
        } else {
            e1Route.cost += p.getDistance(p.getDepot(), p.getSatellite(closestSatellite));
        }
        e1Route.tour.push_back(p.getSatellite(closestSatellite).getSatelliteId());
        e1Route.load +=
                solution.getSatelliteDemands()[closestSatellite] - solution.getDeliveredQ()[closestSatellite];
        solution.getDeliveredQ()[e1Route.tour.back()] = solution.getSatelliteDemands()[closestSatellite];

    }
    // Ajout de la dernière route construite
    e1Route.cost += p.getDistance(p.getDepot(), p.getSatellite(e1Route.tour.back()));

    solution.getE1Routes().push_back(e1Route);
    solution.setTotalCost(solution.getTotalCost() + e1Route.cost);
}

// TODO rendre la méthode plus générale
bool demandComparison(Client &c1, Client &c2) { return c1.getDemand() < c2.getDemand(); }
/*
void moveClients(Solution &solution){
    vector<Client> clients(solution.getProblem()->getClients());
    std::sort(clients.begin(),clients.end(),demandComparison);
}*/
/*
void Heuristic::feasabilitySearch(Solution &solution){
    short error;
    while ( (error = solution.getProblem()->isValidSolution(solution)) != Problem::VALID_SOLUTION){
        switch (error) {
            case Problem::K1_VIOLATION :
                break;
            case Problem::K2_VIOLATION :
                break;
            default:
                cout << "Erreur non encore traite" << endl;
                break;
            }
    }

}*/