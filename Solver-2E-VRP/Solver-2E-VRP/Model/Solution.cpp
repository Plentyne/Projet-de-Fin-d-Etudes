//
// Created by Youcef on 02/02/2016.
//

#include "Solution.h"
#include <fstream>

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

void Solution::saveHumanReadable(const string &fn, const string &header, const bool clrFile) {

    ofstream fh;
    fh.open(fn.c_str(), (clrFile) ? fstream::trunc : (fstream::app | fstream::ate));

    cout << "Saving solution to : " + fn << endl;

    fh << "!----------------------------------------------------------------" << endl;
    fh << header << endl;

    fh << "Total Cost : " << this->getTotalCost() << endl;
    fh << "1st Level vehicles used : " << this->e1Routes.size() << endl;
    fh << "2nd Level vehicles used : " << this->e2Routes.size() << endl;
    fh << endl << "First level routes" << endl;
    // 1st Level Routes
    for (int i = 0; i < this->e1Routes.size(); ++i) {
        fh << "    Route " << i << " : 0";
        for (Satellite s : this->e1Routes[i].sequence) {
            fh << " S" << s.getSatelliteId() + 1;
        }
        fh << " 0" << endl;
        fh << "        Cost : " << this->e1Routes[i].cost << endl;
        fh << "        Load : " << this->e1Routes[i].load << endl;
    }
    // 2nd Level Routes
    fh << endl;
    fh << "Second level routes" << endl;
    for (int i = 0; i < this->e2Routes.size(); ++i) {
        fh << "    Route " << i << " : S" << this->e2Routes[i].departure.getSatelliteId() + 1;
        for (Client s : this->e2Routes[i].sequence) {
            fh << " " << s.getClientId() + 1;
        }
        fh << " S" << this->e2Routes[i].departure.getSatelliteId() + 1 << endl;
        fh << "        Cost : " << this->e2Routes[i].cost << endl;
        fh << "        Load : " << this->e2Routes[i].load << endl;
    }

    fh << "!----------------------------------------------------------------" << endl;
    fh.close();
}
