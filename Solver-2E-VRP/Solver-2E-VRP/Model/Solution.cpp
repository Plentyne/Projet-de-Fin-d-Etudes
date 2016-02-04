//
// Created by Youcef on 02/02/2016.
//

#include "Solution.h"

// TODO Implémenter la classe Solution

void Solution::print() {
    cout << "Solution" << endl;
    cout << "Total Cost : " << this->getTotalCost() << endl;
    cout << "1st Level vehicles used : " << this->e1Routes.size() << endl;
    cout << "2nd Level vehicles used : " << this->e2Routes.size() << endl;
    cout << endl << "First level routes" << endl;
    // 1st Level Routes
    for (int i = 0; i < this->e1Routes.size(); ++i) {
        cout << "    Route " << i << " : 0";
        for (Satellite s : this->e1Routes[i].sequence) {
            cout << " S" << s.getSatelliteId() + 1;
        }
        cout << " 0" << endl;
        cout << "        Cost : " << this->e1Routes[i].cost << endl;
        cout << "        Load : " << this->e1Routes[i].load << endl;
    }
    // 2nd Level Routes
    cout << endl;
    cout << "Second level routes" << endl;
    for (int i = 0; i < this->e2Routes.size(); ++i) {
        cout << "    Route " << i << " : S" << this->e2Routes[i].departure.getSatelliteId() + 1;
        for (Client s : this->e2Routes[i].sequence) {
            cout << " " << s.getClientId() + 1;
        }
        cout << " S" << this->e2Routes[i].departure.getSatelliteId() + 1 << endl;
        cout << "        Cost : " << this->e2Routes[i].cost << endl;
        cout << "        Load : " << this->e2Routes[i].load << endl;
    }
}
