#ifndef SATELLITE_H
#define SATELLITE_H

// Standard Libraries
#include <cmath>
#include <ddeml.h>
#include <winddi.h>

// Fichiers Perso
#include "Node.h"

class Satellite : public Node{
private:
	static int satelliteGlobalId;
	int satelliteId;
	int maxCf;


public:
	// initialization
	/** default constructor */ Satellite();
	/** constructor */ Satellite(const double &x, const double &y, const int &maxCf);

	// access
	/** get satellite id */ int getSatelliteId() const { return this->satelliteId; };
	/** set X coordination value */ void setMaxCf(const int & value) { this->maxCf = value; };
	/** get X coordination value */ int getMaxCf() const { return this->maxCf; };

	// method


	// operator
	friend bool operator==(const Satellite & s1, const Satellite & s2);
	friend bool operator!=(const Satellite & s1, const Satellite & s2);
};


// Operators
inline bool operator==(const Satellite &s1, const Satellite &s2) {
	return (   (static_cast<Node>(s1) == static_cast<Node>(s2))
			   && (s1.maxCf == s2.maxCf));
}

inline bool operator!=(const Satellite &s1, const Satellite &s2) {
	return !(s1==s2);
}

#endif // SATELLITE_H
