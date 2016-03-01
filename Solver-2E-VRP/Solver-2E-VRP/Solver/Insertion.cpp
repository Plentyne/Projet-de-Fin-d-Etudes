//
// Created by Youcef on 11/02/2016.
//
#include <vector>
#include <deque>
#include <algorithm>
#include "Insertion.h"
#include "../Config.h"

// TODO remplacer la construction des tournées du premire échelon par l'heuristique pour le split

int Insertion::getClosestSatellite(E1Route e1Route, const Solution &s) {
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


/* Algorithme pour l'heuristique par insertion
 * Debut
 *   Pour chaque client c faire
 *      trouver la meilleure insertion de c dans les tournées existantes
 *      Si elle est possible, alors y insérer c
 *      Sinon
 *          TQ insertion de c impossible Faire
 *              annuler autant d'insertions que nécéssaires pour libérer assez d'espace pour c
 *              Réinsérer c et les clients retirés de la solution.
 *          FTQ
 *      Fsinon
 *   FinPour
 * Fin
 */

struct insertEntry {
    int clientId;
    int tour;
    int position;
    double cost;
};

struct reinsertEntry {
    int clientId;
    int demand;
};

bool insertExistingRoute(int clientId, Solution &solution) {
    // Déclaration des variables de la fonction
    const Problem *problem = solution.getProblem();
    Client tmpClient;
    double cost;
    double minCost;
    int insertRoute;
    int insertPosition;
    deque<insertEntry> insertStack{};
    // Tant qu'il y a des clients non routés Faire

    // Initialisation des variables de boucle
    minCost = Config::DOUBLE_INFINITY;
    insertRoute = -1;
    insertPosition = -1;
    // Retirer un client de la file
    tmpClient = problem->getClient(clientId);

    E2Route e2Route;
    for (int i = 0; i < solution.getE2Routes().size(); i++) {
        // S'il n'y a pas assez d'espace pour l'insertion
        if (tmpClient.getDemand() <= (problem->getE2Capacity() - solution.getE2Routes()[i].load)) {

            int last = solution.getE2Routes()[i].tour.size();
            e2Route = solution.getE2Routes()[i];
            // Estimer le coût d'insertion de c
            for (int j = 0; j <= e2Route.tour.size(); j++) {
                if (j == 0) {
                    cost = problem->getDistance(problem->getSatellite(e2Route.departureSatellite), tmpClient)
                           + problem->getDistance(problem->getClient(e2Route.tour[0]), tmpClient)
                           - problem->getDistance(problem->getSatellite(e2Route.departureSatellite),
                                                  problem->getClient(e2Route.tour[0]));
                } else if (j == last) {
                    cost = problem->getDistance(problem->getSatellite(e2Route.departureSatellite), tmpClient)
                           + problem->getDistance(problem->getClient(e2Route.tour[last - 1]), tmpClient)
                           - problem->getDistance(problem->getSatellite(e2Route.departureSatellite),
                                                  problem->getClient(e2Route.tour[last - 1]));
                } else {
                    cost = problem->getDistance(problem->getClient(e2Route.tour[j - 1]), tmpClient)
                           + problem->getDistance(problem->getClient(e2Route.tour[j]), tmpClient)
                           - problem->getDistance(problem->getClient(e2Route.tour[j - 1]),
                                                  problem->getClient(e2Route.tour[j]));
                }
                // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
                if (cost < minCost) {
                    minCost = cost;
                    insertRoute = i;
                    insertPosition = j;
                }
                // FSI
            }
        }

    }
    // FPour
    // Si insertion faisable
    if (insertRoute >= 0) {
        solution.getE2Routes()[insertRoute].cost += minCost;
        solution.getE2Routes()[insertRoute].load += tmpClient.getDemand();
        solution.getE2Routes()[insertRoute].tour.insert(
                solution.getE2Routes()[insertRoute].tour.begin() + insertPosition, tmpClient.getClientId());
        // Maj infos solution
        solution.setTotalCost(solution.getTotalCost() + minCost);
        solution.getSatelliteDemands()[solution.getE2Routes()[insertRoute].departureSatellite] += tmpClient.getDemand();
        // Sauvegarder l'insertion
        insertEntry ent;
        ent.clientId = tmpClient.getClientId();
        ent.tour = insertRoute;
        ent.position = insertPosition;
        ent.cost = minCost;
        insertStack.push_back(ent);
        return true;
    }
    return false;
}

bool mySortFunction(reinsertEntry i, reinsertEntry j) { return (i.demand < j.demand); }

/*
void Insertion::GreedyInsertionHeuristic(Solution &solution, const Problem *problem) {
    // Si solution vide, créer nouvelle solution
    if (solution.getProblem() == nullptr) {
        solution = Solution(problem);
    }
    problem = solution.getProblem();
    // Initialiser la liste des clients non routés
    deque<int> unroutedClients{};
    vector<int> tmpClients(problem->getClients().size(), 0);
    for (E2Route route : solution.getE2Routes()) {
        for (int i : route.tour) tmpClients[i] = 1;
    }
    for (int i = 0; i < tmpClients.size(); i++) if (tmpClients[i] == 0) unroutedClients.push_back(i);
    // Déclaration des variables de la boucle
    Client tmpClient;
    double cost;
    double minCost;
    int insertRoute;
    int insertPosition;
    deque<insertEntry> insertStack{};
    // Tant qu'il y a des clients non routés Faire
    while (unroutedClients.size() > 0) {
        // Initialisation des variables de boucle
        minCost = Config::DOUBLE_INFINITY;
        insertRoute = -1;
        insertPosition = -1;
        // Retirer un client de la file
        int c = unroutedClients.front();
        tmpClient = problem->getClient(unroutedClients.front());
        unroutedClients.pop_front();

        if (insertExistingRoute(c,solution)){
            ;
        }
        else { // Si insertion infaisable
            // S'il est possible de créer de nouvelles routes
            if (solution.getE2Routes().size() < problem->getK2()) {
                // Pour chaque satellite Faire
                int insertSatellite = -1;
                for (int i = 0; i < problem->getSatellites().size(); i++) {
                    // Estimer le coût d'insertion de c
                    cost = 2 * problem->getDistance(solution.getProblem()->getSatellite(i), tmpClient);
                    // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
                    if (cost < minCost) {
                        minCost = cost;
                        insertSatellite = i;
                    }
                    // FSI
                }
                // FPour
                // Créer la nouvelle tournée
                E2Route newRoute;
                newRoute.cost = minCost;
                newRoute.load = tmpClient.getDemand();
                newRoute.departureSatellite = insertSatellite;
                newRoute.tour.push_back(tmpClient.getClientId());
                // insérer la nouvelle tournée
                solution.getE2Routes().push_back(newRoute);
                solution.setTotalCost(solution.getTotalCost() + minCost);
                solution.getSatelliteDemands()[newRoute.departureSatellite] += tmpClient.getDemand();
                // Sauvegarder l'insertion
                insertEntry ent;
                ent.clientId = tmpClient.getClientId();
                ent.tour = solution.getE2Routes().size() - 1;
                ent.cost = minCost;
                ent.position = 0;
                insertStack.push_back(ent);
            }
                // Sinon
                // Si aucune insertion feasable, alors retirer des clients de la solution et rajouter c à la liste des clients non routés TODO
            else {
                // Annuler des insertions et recommencer
                int freedSpace = 0;
                insertEntry entry;
                reinsertEntry rentry;
                vector<reinsertEntry> tmp;
                // Anbulation des insertions
                while (freedSpace < tmpClient.getDemand()) {
                    // Dernière insertion
                    entry = insertStack.back();
                    insertStack.pop_back();
                    // Supression de l'insertion
                    solution.getE2Routes()[entry.tour].load -= problem->getClient(entry.clientId).getDemand();
                    solution.getE2Routes()[entry.tour].cost -= entry.cost;
                    solution.getE2Routes()[entry.tour].tour.erase(
                            solution.getE2Routes()[entry.tour].tour.begin() + entry.position);

                    solution.setTotalCost(solution.getTotalCost() - entry.cost);
                    solution.getSatelliteDemands()[solution.getE2Routes()[entry.tour].departureSatellite] -= problem->getClient(
                            entry.clientId).getDemand();

                    if (solution.getE2Routes()[entry.tour].tour.size() == 0) {
                        solution.getE2Routes().erase(solution.getE2Routes().begin() + entry.tour);
                    }
                    // Réinsérer le client dans la file des insertions
                    rentry.clientId = entry.clientId;
                    rentry.demand = problem->getClient(entry.clientId).getDemand();
                    tmp.push_back(rentry);
                    // Espace libéré
                    freedSpace += problem->getClient(entry.clientId).getDemand();
                }
                // Réordonner les insertions
                sort(tmp.begin(), tmp.end(), mySortFunction);
                for (reinsertEntry ent : tmp) unroutedClients.push_front(ent.clientId);
            }
        }
    }
    // FTQ

    // Construire les tournées du premier échelon
    // Construction des tournées du 1er échelon
    E1Route e1Route;
    e1Route.tour.clear();
    e1Route.cost = 0;
    e1Route.load = 0;

    for (int j = 0; j < solution.getDeliveredQ().size(); ++j) {
        solution.getDeliveredQ()[j] = 0;
    }

    int closestSatellite;
    while ((closestSatellite = Insertion::getClosestSatellite(e1Route, solution)) >= 0) {
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
        }
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
    // Ajout de la dernière route construite
    e1Route.cost += problem->getDistance(problem->getDepot(), problem->getSatellite(e1Route.tour.back()));

    solution.getE1Routes().push_back(e1Route);
    solution.setTotalCost(solution.getTotalCost() + e1Route.cost);

}*/

void Insertion::GreedyInsertionHeuristic(Solution &solution, const Problem *problem) {
    // Si solution vide, créer nouvelle solution
    if (solution.getProblem() == nullptr) {
        solution = Solution(problem);
    }

    problem = solution.getProblem();
    // Initialiser la liste des clients non routés
    deque<int> unroutedClients{};
    vector<int> tmpClients(problem->getClients().size(), 0);
    for (E2Route route : solution.getE2Routes()) {
        for (int i : route.tour) tmpClients[i] = 1;
    }
    for (int i = 0; i < tmpClients.size(); i++) if (tmpClients[i] == 0) unroutedClients.push_back(i);
    // Déclaration des variables de la boucle
    Client tmpClient;
    double cost;
    double minCost;
    int insertRoute;
    int insertPosition;
    deque<insertEntry> insertStack{};
    // Tant qu'il y a des clients non routés Faire
    while (unroutedClients.size() > 0) {
        // Initialisation des variables de boucle
        minCost = Config::DOUBLE_INFINITY;
        insertRoute = -1;
        insertPosition = -1;
        // Retirer un client de la file
        tmpClient = problem->getClient(unroutedClients.front());
        unroutedClients.pop_front();

        E2Route e2Route;
        for (int i = 0; i < solution.getE2Routes().size(); i++) {
            // S'il n'y a pas assez d'espace pour l'insertion
            if (tmpClient.getDemand() <= (problem->getE2Capacity() - solution.getE2Routes()[i].load)) {

                int last = solution.getE2Routes()[i].tour.size();
                e2Route = solution.getE2Routes()[i];
                // Estimer le coût d'insertion de c
                for (int j = 0; j <= e2Route.tour.size(); j++) {
                    if (j == 0) {
                        cost = problem->getDistance(problem->getSatellite(e2Route.departureSatellite), tmpClient)
                               + problem->getDistance(problem->getClient(e2Route.tour[0]), tmpClient)
                               - problem->getDistance(problem->getSatellite(e2Route.departureSatellite),
                                                      problem->getClient(e2Route.tour[0]));
                    } else if (j == last) {
                        cost = problem->getDistance(problem->getSatellite(e2Route.departureSatellite), tmpClient)
                               + problem->getDistance(problem->getClient(e2Route.tour[last - 1]), tmpClient)
                               - problem->getDistance(problem->getSatellite(e2Route.departureSatellite),
                                                      problem->getClient(e2Route.tour[last - 1]));
                    } else {
                        cost = problem->getDistance(problem->getClient(e2Route.tour[j - 1]), tmpClient)
                               + problem->getDistance(problem->getClient(e2Route.tour[j]), tmpClient)
                               - problem->getDistance(problem->getClient(e2Route.tour[j - 1]),
                                                      problem->getClient(e2Route.tour[j]));
                    }
                    // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
                    if (cost < minCost) {
                        minCost = cost;
                        insertRoute = i;
                        insertPosition = j;
                    }
                    // FSI
                }
            }

        }
        // FPour
        // Si insertion faisable
        if (insertRoute >= 0) {
            solution.getE2Routes()[insertRoute].cost += minCost;
            solution.getE2Routes()[insertRoute].load += tmpClient.getDemand();
            solution.getE2Routes()[insertRoute].tour.insert(
                    solution.getE2Routes()[insertRoute].tour.begin() + insertPosition, tmpClient.getClientId());
            // Maj infos solution
            solution.setTotalCost(solution.getTotalCost() + minCost);
            solution.getSatelliteDemands()[solution.getE2Routes()[insertRoute].departureSatellite] += tmpClient.getDemand();
            // Sauvegarder l'insertion
            insertEntry ent;
            ent.clientId = tmpClient.getClientId();
            ent.tour = insertRoute;
            ent.position = insertPosition;
            ent.cost = minCost;
            insertStack.push_back(ent);
        }
        else { // Si insertion infaisable
            // S'il est possible de créer de nouvelles routes
            if (solution.getE2Routes().size() < problem->getK2()) {
                // Pour chaque satellite Faire
                int insertSatellite = -1;
                for (int i = 0; i < problem->getSatellites().size(); i++) {
                    // Estimer le coût d'insertion de c
                    cost = 2 * problem->getDistance(solution.getProblem()->getSatellite(i), tmpClient);
                    // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
                    if (cost < minCost) {
                        minCost = cost;
                        insertSatellite = i;
                    }
                    // FSI
                }
                // FPour
                // Créer la nouvelle tournée
                E2Route newRoute;
                newRoute.cost = minCost;
                newRoute.load = tmpClient.getDemand();
                newRoute.departureSatellite = insertSatellite;
                newRoute.tour.push_back(tmpClient.getClientId());
                // insérer la nouvelle tournée
                solution.getE2Routes().push_back(newRoute);
                solution.setTotalCost(solution.getTotalCost() + minCost);
                solution.getSatelliteDemands()[newRoute.departureSatellite] += tmpClient.getDemand();
                // Sauvegarder l'insertion
                insertEntry ent;
                ent.clientId = tmpClient.getClientId();
                ent.tour = solution.getE2Routes().size() - 1;
                ent.cost = minCost;
                ent.position = 0;
                insertStack.push_back(ent);
            }
                // Sinon
                // Si aucune insertion feasable, alors retirer des clients de la solution et rajouter c à la liste des clients non routés TODO
            else {
                // Annuler des insertions et recommencer
                int freedSpace = 0;
                insertEntry entry;
                reinsertEntry rentry;
                vector<reinsertEntry> tmp;
                // Anbulation des insertions
                while (freedSpace < tmpClient.getDemand()) {
                    // Dernière insertion
                    entry = insertStack.back();
                    insertStack.pop_back();
                    // Supression de l'insertion
                    solution.getE2Routes()[entry.tour].load -= problem->getClient(entry.clientId).getDemand();
                    solution.getE2Routes()[entry.tour].cost -= entry.cost;
                    solution.getE2Routes()[entry.tour].tour.erase(
                            solution.getE2Routes()[entry.tour].tour.begin() + entry.position);

                    solution.setTotalCost(solution.getTotalCost() - entry.cost);
                    solution.getSatelliteDemands()[solution.getE2Routes()[entry.tour].departureSatellite] -= problem->getClient(
                            entry.clientId).getDemand();

                    if (solution.getE2Routes()[entry.tour].tour.size() == 0) {
                        solution.getE2Routes().erase(solution.getE2Routes().begin() + entry.tour);
                    }
                    // Réinsérer le client dans la file des insertions
                    rentry.clientId = entry.clientId;
                    rentry.demand = problem->getClient(entry.clientId).getDemand();
                    tmp.push_back(rentry);
                    // Espace libéré
                    freedSpace += problem->getClient(entry.clientId).getDemand();
                }
                // Réordonner les insertions
                sort(tmp.begin(), tmp.end(), mySortFunction);
                for (reinsertEntry ent : tmp) unroutedClients.push_front(ent.clientId);
            }
        }
    }
    // FTQ

    // Construire les tournées du premier échelon
    // Construction des tournées du 1er échelon
    E1Route e1Route;
    e1Route.tour.clear();
    e1Route.cost = 0;
    e1Route.load = 0;

    int closestSatellite;
    while ((closestSatellite = Insertion::getClosestSatellite(e1Route, solution)) >= 0) {
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
        }
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
    // Ajout de la dernière route construite
    e1Route.cost += problem->getDistance(problem->getDepot(), problem->getSatellite(e1Route.tour.back()));

    solution.getE1Routes().push_back(e1Route);
    solution.setTotalCost(solution.getTotalCost() + e1Route.cost);

}
