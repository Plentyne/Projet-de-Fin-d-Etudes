#include "Problem.h"
#include <cassert>

/*
void Problem::readFile(const string & fn) {

	cout << fn ;
	ifstream fh;
	string line, value, valuex, valuey, valuep;
	unsigned int i, n, m, p;
	double x, y, tmax;

	this->clear();

	try {
		fh.open(fn.c_str(), ifstream::in);


		cout << endl ;
		if (!fh.good()) throw (string)"can not open file";

		if (getline(fh, line) && Config::getKeyValue(line, (string)"n", value)) {
			stringstream(value)>>n;
		} else throw (string)"file format error";

		if (getline(fh, line) && Config::getKeyValue(line, (string)"m", value)) {
			stringstream(value)>>m;
		} else throw (string)"file format error";

		if (getline(fh, line) && Config::getKeyValue(line, (string)"tmax", value)) {
			stringstream(value)>>tmax;
			cout << ";"<< tmax<< endl;
		} else throw (string)"file format error";

		this->_number_vehicle = m;
		for (i=0; i<n; i++) {
			if (getline(fh, line)) {
				if (   Config::getFieldValue(line, 0, valuex)
					&& Config::getFieldValue(line, 1, valuey)
					&& Config::getFieldValue(line, 2, valuep)) {
					stringstream(valuex)>>x;
					stringstream(valuey)>>y;
					stringstream(valuep)>>p;
					if (p==0) {
						if (this->_depot.size()==0) {
							this->_depot.push_back(Depot(x, y));
						} else {
							this->_depot.push_back(Depot(x, y));
						}
					} else {
						this->_clients.push_back(Client(this->_clients.size(), x, y, p));
					}
				} else throw (string)"unknown data type";
			} else throw (string)"file format error";
		}

		if (this->_depot.size()==2) this->_vehicle = Vehicle(&this->_depot[0], &this->_depot[1], tmax);
		else throw (string)"incorrect TOP instance";

		fh.close();

	} catch (const string & emsg) {
		if (fh.is_open()) fh.close();
		exitOnError("Problem::readTOPFile()", emsg);
	}

	this->buildDistance();
}


void Problem::buildDistance() {


}

*/

// CONSTRUCTORS
Problem::Problem() : e1Capacity(0), e2Capacity(0), k1(0), k2(0), maxCf(0), depot{}, satellites{}, clients{} {

}

// PRIVATE METHODS
void Problem::buildDistanceMatrix() {
    // TODO implémenter la construction de la matrice des distances
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
void Problem::readFile(const string &fn) {
    // TODO implémenter la lecture depuis un fichier
}
