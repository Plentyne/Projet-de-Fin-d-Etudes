//
// Created by Youcef on 11/02/2016.
//
#include <vector>
#include <algorithm>
#include "Insertion.h"
#include "../Config.h"
#include "../Utility.h"

struct reinsertEntry {
    int clientId;
    int demand;
};

bool mySortFunction(reinsertEntry i, reinsertEntry j) { return (i.demand < j.demand); }

double Insertion::distanceCost(Solution &solution, int client, int route, int position) {
    Node p, q, c;
    c = problem->getClient(client);
    if (position == 0) {
        p = problem->getSatellite(solution.getE2Routes()[route].departureSatellite);
        q = problem->getClient(solution.getE2Routes()[route].tour[0]);
    }
    else if (position == solution.getE2Routes()[route].tour.size()) {
        p = problem->getClient(solution.getE2Routes()[route].tour[position - 1]);
        q = problem->getSatellite(solution.getE2Routes()[route].departureSatellite);
    }
    else {
        p = problem->getClient(solution.getE2Routes()[route].tour[position - 1]);
        q = problem->getClient(solution.getE2Routes()[route].tour[position]);
    }
    return problem->getDistance(p, c) + problem->getDistance(c, q) - problem->getDistance(p, q);
}

double Insertion::biasedDistanceCost(Solution &solution, int client, int route, int position) {
    double noise = Utility::randomDouble(0.8, 1.2);
    Node p, q, c;
    c = problem->getClient(client);
    if (position == 0) {
        p = problem->getSatellite(solution.getE2Routes()[route].departureSatellite);
        q = problem->getClient(solution.getE2Routes()[route].tour[0]);
    }
    else if (position == solution.getE2Routes()[route].tour.size()) {
        p = problem->getClient(solution.getE2Routes()[route].tour[position - 1]);
        q = problem->getSatellite(solution.getE2Routes()[route].departureSatellite);
    }
    else {
        p = problem->getClient(solution.getE2Routes()[route].tour[position - 1]);
        q = problem->getClient(solution.getE2Routes()[route].tour[position]);
    }
    return (problem->getDistance(p, c) + problem->getDistance(c, q) - problem->getDistance(p, q)) * noise;
}

void Insertion::insertIntoRoute(Solution &solution, int client, int route, int position) {
    double cost = distanceCost(solution, client, route, position);
    solution.getE2Routes()[route].cost += cost;
    solution.getE2Routes()[route].load += problem->getClient(client).getDemand();
    solution.getE2Routes()[route].tour.insert(
            solution.getE2Routes()[route].tour.begin() + position, client);
    // Maj infos solution
    solution.setTotalCost(solution.getTotalCost() + cost);
    solution.getSatelliteDemands()[solution.getE2Routes()[route].departureSatellite] += problem->getClient(
            client).getDemand();
    // Sauvegarder l'insertion
    insertEntry ent;
    ent.clientId = client;
    ent.tour = route;
    ent.position = position;
    ent.cost = cost;
    insertStack.push_back(ent);
    // Insérer la demande dans le satellite Todo
    //e1Solver.insert(solution,solution.getE2Routes()[route].departureSatellite,tmpClient.getDemand());
}

void Insertion::insertIntoNewRoute(Solution &solution, int client, int satellite) {
    E2Route newRoute;
    double cost = 2 * problem->getDistance(problem->getSatellite(satellite), problem->getClient(client));
    newRoute.cost = cost;
    newRoute.load = problem->getClient(client).getDemand();
    newRoute.departureSatellite = satellite;
    newRoute.tour.push_back(client);
    // insérer la nouvelle tournée
    solution.getE2Routes().push_back(newRoute);
    solution.setTotalCost(solution.getTotalCost() + cost);
    solution.getSatelliteDemands()[newRoute.departureSatellite] += problem->getClient(client).getDemand();
    // Sauvegarder l'insertion
    insertEntry ent;
    ent.clientId = client;
    ent.tour = solution.getE2Routes().size() - 1;
    ent.cost = cost;
    ent.position = 0;
    insertStack.push_back(ent);
}

void Insertion::cancelInsertions(Solution &solution, int client) {
    insertEntry entry;
    reinsertEntry rentry;
    deque<reinsertEntry> tmp;

    insertStack.clear();
    solution = Solution(this->problem);
    std::random_shuffle(solution.unroutedCustomers.begin(), solution.unroutedCustomers.end());
    return;
    // Annulation des insertions
    /*int t = insertStack.size();
    int d = lround(0.1 * static_cast<double>(problem->getClients().size()));
    if (lastCanceled == client) {
        cancelations = min(t, cancelations + d);
    }
    else {
        lastCanceled = client;
        cancelations = min(t, d * 2);
    }
    // Si il faut tout annuler, alors recommencer à zéro
    if (cancelations == t) {
        insertStack.clear();
        solution = Solution(this->problem);
        std::random_shuffle(solution.unroutedCustomers.begin(), solution.unroutedCustomers.end());
        return;
    }

    // Sinon
    int i = cancelations;
    while (i > 0) {
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

        --i;
    }
    // Insérer le client dans la liste des réinsertions
    rentry.clientId = client;
    rentry.demand = problem->getClient(client).getDemand();
    tmp.push_back(rentry);
    // Réordonner les insertions
    //sort(tmp.begin(), tmp.end(), mySortFunction);
    random_shuffle(tmp.begin(), tmp.end());
    for (reinsertEntry ent : tmp) solution.unroutedCustomers.push_back(ent.clientId);*/
    // Recalculer le 1er échelon TODO
    /*for (E1Route route : solution.getE1Routes()) solution.setTotalCost(solution.getTotalCost() - route.cost);
    solution.getE1Routes().clear();
    e1Solver.constructiveHeuristic(solution);*/
}

void Insertion::GreedyInsertionHeuristic(Solution &solution) {
    // Si solution vide, créer nouvelle solution
    if (solution.getProblem() == nullptr) {
        solution = Solution(problem);
    }

    // Clear insertion stack
    this->insertStack.clear();

    // Déclaration des variables de la boucle
    Client tmpClient;
    double cost;
    double minCost;
    double minE1Cost;
    int insertRoute;
    int insertPosition;
    int insertSatellite;
    bool feasible;

    // Sauvegarder la solution en entrée
    Solution save(solution);

    std::random_shuffle(solution.unroutedCustomers.begin(), solution.unroutedCustomers.end());
    // Tant qu'il y a des clients non routés Faire
    while (solution.unroutedCustomers.size() > 0) {
        // Initialisation des variables de boucle
        minE1Cost = minCost = Config::DOUBLE_INFINITY;
        insertRoute = -1;
        insertPosition = -1;
        insertSatellite = -1;
        feasible = false;
        // Retirer un client de la file
        /*tmpClient = problem->getClient(solution.unroutedCustomers.front());
        solution.unroutedCustomers.pop_front();*/
        int tmp = Utility::randomInt(0, solution.unroutedCustomers.size());
        tmpClient = problem->getClient(solution.unroutedCustomers[tmp]);
        solution.unroutedCustomers.erase(solution.unroutedCustomers.begin() + tmp);

        E2Route e2Route;
        // Calculer le coût d'insertion dans les tournées déjà existantes
        for (int i = 0; i < solution.getE2Routes().size(); i++) {
            // S'il n'y a pas assez d'espace pour l'insertion
            if (tmpClient.getDemand() <= (problem->getE2Capacity() - solution.getE2Routes()[i].load)) {
                e2Route = solution.getE2Routes()[i];
                // Imprimer coût sur le 1er échelon TODO
                //double e1cost =  e1Solver.insertionCost(solution, e2Route.departureSatellite, tmpClient.getDemand());
                double e1cost = 0;
                // Estimer le coût d'insertion de c
                for (int j = 0; j <= e2Route.tour.size(); j++) {
                    cost = this->distanceCost(solution, tmpClient.getClientId(), i, j);
                    // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
                    if (cost + e1cost < minCost + minE1Cost) {
                        minCost = cost;
                        minE1Cost = e1cost;
                        insertRoute = i;
                        insertPosition = j;
                        feasible = true;
                    }// FSI
                }
            }

        }// FPour
        // Calculer le coût d'insertion dans une nouvelle tournée si c'est possible
        if (solution.getE2Routes().size() < problem->getK2()) {
            for (int i = 0; i < problem->getSatellites().size(); i++) {
                // Si satellite fermé alors l'ignorer
                if (solution.satelliteState[i] == Solution::CLOSED) continue;
                // Estimer le coût d'insertion de c
                cost = 2 * problem->getDistance(solution.getProblem()->getSatellite(i), tmpClient);
                //double e1cost =  e1Solver.insertionCost(solution, i, tmpClient.getDemand());
                double e1cost = 0;
                // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
                if (cost + e1cost < minCost + minE1Cost) {
                    feasible = true;
                    minE1Cost = e1cost;
                    minCost = cost;
                    insertSatellite = i;
                }
                // FSI
            }
        }
        // Si insertion faisable
        if (feasible) {
            // Insertion dans une tournée existante
            if (insertSatellite == -1) {
                insertIntoRoute(solution, tmpClient.getClientId(), insertRoute, insertPosition);
                // Insérer la demande dans le satellite Todo
                //e1Solver.insert(solution,solution.getE2Routes()[insertRoute].departureSatellite,tmpClient.getDemand());
            }
                // Insertion dans une nouvelle tournée
            else {
                insertIntoNewRoute(solution, tmpClient.getClientId(), insertSatellite);
                // Insérer la demande dans le satellite Todo
                //e1Solver.insert(solution,insertSatellite,tmpClient.getDemand());
            }
        }
        else { //Si aucune insertion feasable, alors retirer des clients de la solution et rajouter c à la liste des clients non routés TODO
                // Annuler des insertions et recommencer
            solution = save;
            //cancelInsertions(solution, tmpClient.getClientId());
        }
    }
    // FTQ
    // Amélioration de la tournée Todo Enlever après
    double imp;
    do {
        imp = solution.getTotalCost();
        lsSolver.apply2optStar(solution);
        lsSolver.applySwap(solution);
        lsSolver.applyRelocate(solution);
        for (E2Route &e2route : solution.getE2Routes()) {
            solution.setTotalCost(solution.getTotalCost() - e2route.cost);
            lsSolver.apply2OptOnTour(e2route);
            solution.setTotalCost(solution.getTotalCost() + e2route.cost);
        }
        imp = solution.getTotalCost() - imp;
    } while (imp < -0.0001);

    //--------------------------------------------

    // Changer le satellite des tournées
    for (E2Route &e2route : solution.getE2Routes()) {
        solution.setTotalCost(solution.getTotalCost() - e2route.cost);
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
        solution.getSatelliteDemands()[e2route.departureSatellite] -= e2route.load;
        solution.getSatelliteDemands()[satellite] += e2route.load;

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
        solution.setTotalCost(solution.getTotalCost() + e2route.cost);
    }
    //--------------------------------------------
    // Amélioration de la tournée Todo Enlever après

    do {
        imp = solution.getTotalCost();
        lsSolver.apply2optStar(solution);
        lsSolver.applyRelocate(solution);
        lsSolver.applySwap(solution);
        for (E2Route &e2route : solution.getE2Routes()) {
            solution.setTotalCost(solution.getTotalCost() - e2route.cost);
            //lsSolver.applyOrOpt(e2route, 1);
            //lsSolver.applyOrOpt(e2route, 2);
            //lsSolver.applyOrOpt(e2route, 3);
            lsSolver.apply2OptOnTour(e2route);
            solution.setTotalCost(solution.getTotalCost() + e2route.cost);
        }
        imp = solution.getTotalCost() - imp;
    } while (imp < -0.0001);

    //--------------------------------------------
    // Construction des tournées du 1er échelon
    solution.getE1Routes().clear();
    /*for (int k = 0; k < solution.getSatelliteDemands().size(); ++k) {
        solution.getSatelliteDemands()[k] =0;
    }
    for (E2Route &route : solution.getE2Routes()) {
        solution.getSatelliteDemands()[route.departureSatellite] += route.load;
    }*/
    e1Solver.constructiveHeuristic(solution);

}

void Insertion::GreedyInsertionNoiseHeuristic(Solution &solution) {
    // Si solution vide, créer nouvelle solution
    if (solution.getProblem() == nullptr) {
        solution = Solution(problem);
    }

    // Clear insertion stack
    this->insertStack.clear();

    // Déclaration des variables de la boucle
    Client tmpClient;
    double cost;
    double minCost;
    double minE1Cost;
    int insertRoute;
    int insertPosition;
    int insertSatellite;
    bool feasible;

    std::random_shuffle(solution.unroutedCustomers.begin(), solution.unroutedCustomers.end());
    // Tant qu'il y a des clients non routés Faire
    while (solution.unroutedCustomers.size() > 0) {
        // Initialisation des variables de boucle
        minE1Cost = minCost = Config::DOUBLE_INFINITY;
        insertRoute = -1;
        insertPosition = -1;
        insertSatellite = -1;
        feasible = false;
        // Retirer un client de la file
        /*tmpClient = problem->getClient(solution.unroutedCustomers.front());
        solution.unroutedCustomers.pop_front();*/
        int tmp = Utility::randomInt(0, solution.unroutedCustomers.size());
        tmpClient = problem->getClient(solution.unroutedCustomers[tmp]);
        solution.unroutedCustomers.erase(solution.unroutedCustomers.begin() + tmp);

        E2Route e2Route;
        // Calculer le coût d'insertion dans les tournées déjà existantes
        for (int i = 0; i < solution.getE2Routes().size(); i++) {
            // S'il n'y a pas assez d'espace pour l'insertion
            if (tmpClient.getDemand() <= (problem->getE2Capacity() - solution.getE2Routes()[i].load)) {
                e2Route = solution.getE2Routes()[i];
                // Imprimer coût sur le 1er échelon TODO
                //double e1cost =  e1Solver.insertionCost(solution, e2Route.departureSatellite, tmpClient.getDemand());
                double e1cost = 0;
                // Estimer le coût d'insertion de c
                for (int j = 0; j <= e2Route.tour.size(); j++) {
                    cost = this->biasedDistanceCost(solution, tmpClient.getClientId(), i, j);
                    // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
                    if (cost + e1cost < minCost + minE1Cost) {
                        minCost = cost;
                        minE1Cost = e1cost;
                        insertRoute = i;
                        insertPosition = j;
                        feasible = true;
                    }// FSI
                }
            }

        }// FPour
        // Calculer le coût d'insertion dans une nouvelle tournée si c'est possible
        if (solution.getE2Routes().size() < problem->getK2()) {
            for (int i = 0; i < problem->getSatellites().size(); i++) {
                // Si satellite fermé alors l'ignorer
                if (solution.satelliteState[i] == Solution::CLOSED) continue;
                // Estimer le coût d'insertion de c
                cost = 2 * problem->getDistance(solution.getProblem()->getSatellite(i), tmpClient);
                //double e1cost =  e1Solver.insertionCost(solution, i, tmpClient.getDemand());
                double e1cost = 0;
                // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
                if (cost + e1cost < minCost + minE1Cost) {
                    feasible = true;
                    minE1Cost = e1cost;
                    minCost = cost;
                    insertSatellite = i;
                }
                // FSI
            }
        }
        // Si insertion faisable
        if (feasible) {
            // Insertion dans une tournée existante
            if (insertSatellite == -1) {
                insertIntoRoute(solution, tmpClient.getClientId(), insertRoute, insertPosition);
                // Insérer la demande dans le satellite Todo
                //e1Solver.insert(solution,solution.getE2Routes()[insertRoute].departureSatellite,tmpClient.getDemand());
            }
                // Insertion dans une nouvelle tournée
            else {
                insertIntoNewRoute(solution, tmpClient.getClientId(), insertSatellite);
                // Insérer la demande dans le satellite Todo
                //e1Solver.insert(solution,insertSatellite,tmpClient.getDemand());
            }
        }
        else { //Si aucune insertion feasable, alors retirer des clients de la solution et rajouter c à la liste des clients non routés TODO
            // Annuler des insertions et recommencer
            cancelInsertions(solution, tmpClient.getClientId());
        }
    }
    // FTQ
    // Amélioration de la tournée Todo Enlever après
    double imp;
    do {
        imp = solution.getTotalCost();
        lsSolver.apply2optStar(solution);
        lsSolver.applySwap(solution);
        lsSolver.applyRelocate(solution);
        for (E2Route &e2route : solution.getE2Routes()) {
            solution.setTotalCost(solution.getTotalCost() - e2route.cost);
            lsSolver.apply2OptOnTour(e2route);
            solution.setTotalCost(solution.getTotalCost() + e2route.cost);
        }
        imp = solution.getTotalCost() - imp;
    } while (imp < -0.0001);

    //--------------------------------------------

    // Changer le satellite des tournées
    for (E2Route &e2route : solution.getE2Routes()) {
        solution.setTotalCost(solution.getTotalCost() - e2route.cost);
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
        solution.getSatelliteDemands()[e2route.departureSatellite] -= e2route.load;
        solution.getSatelliteDemands()[satellite] += e2route.load;

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
        solution.setTotalCost(solution.getTotalCost() + e2route.cost);
    }
    //--------------------------------------------
    // Amélioration de la tournée Todo Enlever après

    do {
        imp = solution.getTotalCost();
        lsSolver.apply2optStar(solution);
        lsSolver.applyRelocate(solution);
        lsSolver.applySwap(solution);
        for (E2Route &e2route : solution.getE2Routes()) {
            solution.setTotalCost(solution.getTotalCost() - e2route.cost);
            //lsSolver.applyOrOpt(e2route, 1);
            //lsSolver.applyOrOpt(e2route, 2);
            //lsSolver.applyOrOpt(e2route, 3);
            lsSolver.apply2OptOnTour(e2route);
            solution.setTotalCost(solution.getTotalCost() + e2route.cost);
        }
        imp = solution.getTotalCost() - imp;
    } while (imp < -0.0001);

    //--------------------------------------------
    // Construction des tournées du 1er échelon
    solution.getE1Routes().clear();
    /*for (int k = 0; k < solution.getSatelliteDemands().size(); ++k) {
        solution.getSatelliteDemands()[k] =0;
    }
    for (E2Route &route : solution.getE2Routes()) {
        solution.getSatelliteDemands()[route.departureSatellite] += route.load;
    }*/
    e1Solver.constructiveHeuristic(solution);
}
