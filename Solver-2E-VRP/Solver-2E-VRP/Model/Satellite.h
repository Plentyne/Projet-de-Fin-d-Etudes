#ifndef SATELLITE_H
#define SATELLITE_H

// Standard Libraries

// Fichiers Perso
#include "Node.h"

class Satellite : public Node{
private:
	static int satelliteGlobalId;
	int satelliteId;


public:
	static void clear();
	// initialization
	/** default constructor */ Satellite();

    /** constructor */ Satellite(const double &x, const double &y);

	// access
	virtual ~Satellite() { }

/** get satellite id */ int getSatelliteId() const { return this->satelliteId; };

	// method


	// operator
	friend bool operator==(const Satellite & s1, const Satellite & s2);
	friend bool operator!=(const Satellite & s1, const Satellite & s2);
};


// Operators
inline bool operator==(const Satellite &s1, const Satellite &s2) {
    return (static_cast<Node>(s1) == static_cast<Node>(s2));
}

inline bool operator!=(const Satellite &s1, const Satellite &s2) {
	return !(s1==s2);
}

#endif // SATELLITE_H
