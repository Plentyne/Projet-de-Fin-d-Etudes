// Standard libraries
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Header Files
#include "Problem.h"
#include "../Config.h"

// CONSTRUCTORS
Problem::Problem() : e1Capacity(0), e2Capacity(0), k1(0), k2(0), maxCf(0), depot{}, satellites{}, clients{} {

}

// PRIVATE METHODS
void Problem::buildDistanceMatrix() {
    // TODO Review
    // initialisation de la matrice des distances à +infini
    this->distances.resize(this->getDimension(), vector<double>(this->getDimension(), Config::DOUBLE_INFINITY));

    // Calcul des distances Client<->Client et Client<->Satellite
    for (int i = 0; i < this->clients.size(); i++) {
        // Distances Client<->Client
        for (int j = i + 1; j < this->clients.size(); ++j) {
            this->distances[this->clients[i].getNodeId()][this->clients[j].getNodeId()]
                    = this->distances[this->clients[j].getNodeId()][this->clients[i].getNodeId()]
                    = this->clients[i].distanceTo(this->clients[j]);
        }
        // Distance Client<->Satellite
        for (int j = 0; j < this->satellites.size(); ++j) {
            this->distances[this->clients[i].getNodeId()][this->satellites[j].getNodeId()]
                    = this->distances[this->satellites[j].getNodeId()][this->clients[i].getNodeId()]
                    = this->clients[i].distanceTo(this->satellites[j]);
        }
    }

    // Calcul des distance Satellite<->Satellite et Satellite<->Depot
    for (int i = 0; i < this->satellites.size(); i++) {
        // Distances Satellite<->Satellite
        for (int j = i + 1; j < this->satellites.size(); ++j) {
            this->distances[this->satellites[i].getNodeId()][this->satellites[j].getNodeId()]
                    = this->distances[this->satellites[j].getNodeId()][this->satellites[i].getNodeId()]
                    = this->satellites[i].distanceTo(this->satellites[j]);
        }
        // Distance Satellite<->Depot
        this->distances[this->satellites[i].getNodeId()][this->depot.getNodeId()]
                = this->distances[this->depot.getNodeId()][this->satellites[i].getNodeId()]
                = this->satellites[i].distanceTo(this->depot);
    }

}

// PUBLIC METHODS
void Problem::clear() {
    e1Capacity = e2Capacity = k1 = k2 = maxCf = 0;
    depot = {};
    satellites.clear();
    clients.clear();
}

// Data access methods
// Clients
void Problem::setClients(const vector<Client> &value) {
    this->clients = value;
}

const vector<Client> &Problem::getClients() const {
    return this->clients;
}

void Problem::setClient(const int &i, const Client &c) {
    // TODO implémenter SetClient si c'est utile
}

const Client Problem::getClient(const int i) const {
    assert(i >= 0 && i < this->clients.size());
    return this->clients[i];
}

int Problem::numberOfClients() {
    return this->clients.size();
}

// Satellites
void Problem::setSatellites(const vector<Satellite> &value) {
    this->satellites = value;
}

const vector<Satellite> &Problem::getSatellites() const {
    return this->satellites;
}

void Problem::setSatellite(const int &i, const Satellite &s) {
    // TODO implémenter si c'est utile
}

const Satellite Problem::getSatellite(const int &i) const {
    assert(i >= 0 && i < this->satellites.size());
    return this->satellites[i];
}

int Problem::numberOfSatellites() {
    return this->satellites.size();
}

// Depot
void Problem::setDepot(const Depot &d) {
    // TODO implémenter si c'est utile
}

const Depot Problem::getDepot() const {
    return this->depot;
}

// Problem Data
int Problem::getDimension() {
    return this->clients.size() + this->satellites.size() + 1;
}

void Problem::setK1(const int &value) {
    this->k1 = value;
}

void Problem::setK2(const int &value) {
    this->k2 = value;
}

int Problem::getK1() const {
    return this->k1;
}

int Problem::getK2() const {
    return this->k2;
}

void Problem::setE1Capacity(const int &value) {
    this->e1Capacity = value;
}

void Problem::setE2Capacity(const int &value) {
    this->e2Capacity = value;
}

int Problem::getE1Capacity() {
    return this->e1Capacity;
}

int Problem::getE2Capacity() {
    return this->e2Capacity;
}

int Problem::getMaxCf() {
    return this->maxCf;
}

void Problem::setMaxCf(const int &value) {
    this->maxCf = value;
}

// Distance matrix
double Problem::getDistance(const Node &i, const Node &j) {
    assert(i.getNodeId() < this->distances.size());
    assert(j.getNodeId() < this->distances[i.getNodeId()].size());

    return this->distances[i.getNodeId()][j.getNodeId()];
}

// Problem loading
void Problem::readBreunigFile(const string &fn) {
    // TODO Review
    // TODO Vérifier le formattage du fichier, la méthode telle qu'elle est suppose qu'il est correcte
    cout << fn << endl;
    ifstream fh;
    string line, token, value;
    stringstream sstream, ss;
    double x, y, demand;

    try {
        // Opening File
        fh.open(fn.c_str(), ifstream::in);
        if (!fh.good()) throw (string) "can not open file";

        // Reading 1st echelon vehicle data
        while (getline(fh, line) && line.at(0) == '!');
        sstream.clear();
        sstream.str(line);
        getline(sstream, token, ',');
        stringstream(token) >> this->k1;
        getline(sstream, token, ',');
        stringstream(token) >> this->e1Capacity;

        // Reading 2nd echelon vehicle data
        while (getline(fh, line) && line.at(0) == '!');
        sstream.clear();
        sstream.str(line);
        getline(sstream, token, ',');
        stringstream(token) >> this->maxCf;
        getline(sstream, token, ',');
        stringstream(token) >> this->k2;
        getline(sstream, token, ',');
        stringstream(token) >> this->e2Capacity;

        // Reading depot information
        while (getline(fh, line) && line.at(0) == '!');
        sstream.clear();
        sstream.str(line);
        sstream >> token;
        ss.clear();
        ss.str(token);
        getline(ss, value, ',');
        stringstream(value) >> x;
        getline(ss, value, ',');
        stringstream(value) >> y;
        this->depot = Depot(x, y);

        // Reading satellite information
        while (sstream >> token) {
            stringstream ss(token);
            getline(ss, value, ',');
            stringstream(value) >> x;
            getline(ss, value, ',');
            stringstream(value) >> y;
            this->satellites.push_back(Satellite(x, y));
        }

        // Reading client information
        while (getline(fh, line) && line.at(0) == '!');
        sstream.clear();
        sstream.str(line);
        while (sstream >> token) {
            stringstream ss(token);
            getline(ss, value, ',');
            stringstream(value) >> x;
            getline(ss, value, ',');
            stringstream(value) >> y;
            getline(ss, value, ',');
            stringstream(value) >> demand;
            this->clients.push_back(Client(x, y, demand));
        }

        fh.close();

    } catch (const string &emsg) {
        if (fh.is_open()) fh.close();
        cout << emsg << endl;
        exit(1);
    }
    // Construction de la matrice des distances
    this->buildDistanceMatrix();

}

bool Problem::verifySolution(const Solution &s) {
    // TODO implement VerifySolution
    return false;
}

void Problem::saveSolution(const Solution &s, const string &fn, const string &header) {
    // TODO implement saveSolution
}


