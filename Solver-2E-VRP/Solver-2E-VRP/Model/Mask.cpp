//
// Created by Youcef on 19/04/2016.
//

#include "Mask.h"

void Mask::setUp(double granularityThreshold, Solution &solution) {
/* For each customer c1 do
     *      For each customer c2 different from c1 do
     *          If cost(c1,c2) > granularityThreshold then {
     *              mask(c1,c2) = Prohibited
     *          }
     *          else mask(c1,c2) = Permited
     *      End for
     * End For
     *
     * For each arc a in solution do
     *      mask(a) = Permited
     * End for
     * */

    for (int i = 0; i < problem->getClients().size(); ++i) {
        Client c1 = problem->getClient(i);
        for (int j = i + 1; j < problem->getClients().size(); ++j) {
            Client c2 = problem->getClient(j);
            if (problem->getDistance(c1, c2) - granularityThreshold > 0.01) {
                mask[c1.getNodeId()][c2.getNodeId()] = Solution::PROHIBITED;
                mask[c2.getNodeId()][c1.getNodeId()] = Solution::PROHIBITED;
            }
            else {
                mask[c1.getNodeId()][c2.getNodeId()] = Solution::PERMITED;
                mask[c2.getNodeId()][c1.getNodeId()] = Solution::PERMITED;
            }
        }
    }

    for (int i = 0; i < solution.getE2Routes().size(); ++i) {
        E2Route &route = solution.getE2Routes()[i];
        for (int j = 0; j < route.tour.size() - 1; ++j) {
            mask[problem->getClient(route.tour[j]).getNodeId()][problem->getClient(
                    route.tour[j + 1]).getNodeId()] = Solution::PERMITED;
            mask[problem->getClient(route.tour[j + 1]).getNodeId()][problem->getClient(
                    route.tour[j]).getNodeId()] = Solution::PERMITED;
        }
    }
}

short &Mask::operator()(Node &i, Node &j) {
    return mask[i.getNodeId()][j.getNodeId()];
}
