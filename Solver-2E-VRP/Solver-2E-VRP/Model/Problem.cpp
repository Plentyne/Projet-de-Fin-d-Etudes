#include "Problem.h"


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

