//
// Created by Youcef on 02/02/2016.
//

#include "Solution.h"

// TODO Implémenter la classe Solution

void Solution::print() {
    cout << "Solution" << endl;
    cout << "Total Cost : " << this->getTotalCost();
    cout << "1st Level vehicles used : " << this->e1Routes.size();
    cout << "2nd Level vehicles used : " << this->e2Routes.size();
    cout << "First level routes" << endl;
    // 1st Level Routes
    for (int i = 0; i < this->e1Routes.size(); ++i) {
        cout << endl << "    Route " << i << " : 0";
        for (Satellite *s : this->e1Routes[i].sequence) {
            cout << " S" << s->getSatelliteId();
        }
        cout << endl;
        cout << "        Cost : " << this->e1Routes[i].cost << endl;
        cout << "        Load : " << this->e1Routes[i].load << endl;
    }
    // 2nd Level Routes
    cout << "Second level routes" << endl;
    for (int i = 0; i < this->e2Routes.size(); ++i) {
        cout << endl << "    Route " << i << " : 0";
        for (Client *s : this->e2Routes[i].sequence) {
            cout << " " << s->getClientId();
        }
        cout << endl;
        cout << "        Cost : " << this->e1Routes[i].cost << endl;
        cout << "        Load : " << this->e1Routes[i].load << endl;
    }
}
