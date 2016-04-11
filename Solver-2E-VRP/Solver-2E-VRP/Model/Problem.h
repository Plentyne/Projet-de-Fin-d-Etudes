#ifndef PROBLEM_H
#define PROBLEM_H

// Standard Libraries
#include <cmath>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

// Header Files
#include "Node.h"
#include "Client.h"
#include "Satellite.h"
#include "Depot.h"


class Solution;

// Class Declaration
class Problem {
private:
    // problem data
    vector<Client> clients;
    vector<Satellite> satellites;
    Depot depot;
    int e1Capacity;
    int e2Capacity;
    int k1;
    int k2;
    int maxCf;

    // pre-processed data
    vector<vector<double> > distances;

    // private methods
    /** build distance matrix, inaccessible clients will be removed */ void buildDistanceMatrix();

public:
    /** default constructor */ Problem();

    /** clear all data */ void clear();

    // DATA ACCESS METHODS
    // Clients
    /** put client set  */ void setClients(const vector<Client> &value);
    /** get client set  */ const vector<Client> &getClients() const;
    /** set i-th client */ void setClient(const int &i, const Client &c);
    /** get i-th client */ const Client getClient(const int i) const;
    /** get the number of clients */ int numberOfClients();

    // Satellites
    /** put Satellite set */  void setSatellites(const vector<Satellite> &value);
    /** get Satellite set */  const vector<Satellite> &getSatellites() const;
    /** set i-th satellite */ void setSatellite(const int &i, const Satellite &s);
    /** get i-th satellite */ const Satellite getSatellite(const int &i) const;
    /** get the number of satellites */ int numberOfSatellites();

    // Depot
    /** set depot node */ void setDepot(const Depot &d);
    /** get depot node */ const Depot getDepot() const;


    // Problem Data
    /** get problem dimension */ int getDimension() const;
    /** set number of vehicles of 1st Echelon*/ void setK1(const int &value);
    /** set number of vehicles of 2nd Echelon*/ void setK2(const int &value);
    /** get number of vehicles of 1st Echelon*/ int getK1() const;
    /** get number of vehicles of 2nd Echelon*/ int getK2() const;
    /** set the capacity of 1st level vehicles (Trucks) */ void setE1Capacity(const int &value);
    /** set the capacity of 2nd level vehicles (City Freighters) */ void setE2Capacity(const int &value);
    /** get the capacity of 1st level vehicles (Trucks) */ int getE1Capacity();
    /** get the capacity of 2nd level vehicles (City Freighters) */ int getE2Capacity();

    /** get the capacity of 1st level vehicles (Trucks) */ const int getE1Capacity() const;

    /** get the capacity of 2nd level vehicles (City Freighters) */ const int getE2Capacity() const;
    /** set X coordination value */ void setMaxCf(const int &value);

    /** get X coordination value */ int getMaxCf() const;

    // Distance Matrix
    /** get the distance between node i and node j*/ const double getDistance(const Node &i, const Node &j) const;
    /** get the distance between node i and node j*/ double getDistance(const Node &i, const Node &j);


    // Solution Methods
    /** Check if a solution matches the problem constraints */ short isValidSolution(const Solution &s) const;
    // Todo Make solution parametre const

    // I/O Methods
    /** load problem data from file using Breunig Unified Format */ void readBreunigFile(const string &fn);

    // Constants
    static const short K1_VIOLATION;
    static const short K2_VIOLATION;
    static const short E1CAP_VIOLATION;
    static const short E2CAP_VIOLATION;
    static const short MAXCF_VIOLATION;
    static const short UNSERVED_CLIENT_VIOLATION;
    static const short UNSERVED_SATELLITE_VIOLATION;
    static const short VALID_SOLUTION;

};

#endif // PROBLEM_H
