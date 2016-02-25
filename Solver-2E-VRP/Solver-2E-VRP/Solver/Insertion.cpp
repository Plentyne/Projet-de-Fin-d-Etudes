//
// Created by Youcef on 11/02/2016.
//
#include <vector>

#include "Insertion.h"

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

void Insertion::insertionHeuristic(Solution &solution, Problem *problem) {
    // Si solution vide, créer nouvelle solution
    if (solution.getProblem() == nullptr) {
        solution = Solution(problem);
    }
    // Initialiser la liste des clients non routés
    vector<int> unroutedClients(problem->getClients().size());
    vector<int> tmpClients(problem->getClients().size(), 0);
    for (E2Route route : solution.getE2Routes()) {
        for (int i : route.tour) tmpClients[i] = 1;
    }
    for (int i : tmpClients) if (tmpClients[i] == 1) unroutedClients.push_back(i);
    // Construire la liste des tours (regroupement de tournées) pour chaque satellite
    // (Cette partie peut être intégrer dans la boucle précédente pour n'utiliser qu'une seule boucle)
    vector<vector<int>> satelliteTours(problem->getSatellites().size());
    for (E2Route route : solution.getE2Routes()) {
        for (int i : route.tour) satelliteTours[route.departureSatellite].push_back(i);
    }
    // Tant qu'il y a des clients non routés Faire
    while (unroutedClients.size() > 0) {
        // Pour chaque satellite Faire
        // Si le nombre de tournées générées après insertion de c dans le tour de S respecte les contraintes
        // Estimer le coût d'insertion de c dans s
        // Si il est meilleure que celui de la meilleure insertion actuelle alors le garder
        // FSI
        // FPour
        // Si aucune insertion feasable, alors retirer des clients de la solution et rajouter c à la liste des clients non routés
        // Insérer c dans la position de la meilleure insertion
        // Adapter les tournées du premier échelon
    }
    // FTQ

}