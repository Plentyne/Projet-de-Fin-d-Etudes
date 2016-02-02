#ifndef DEPOT_H_
#define DEPOT_H_

#include "Node.h"

class Depot : public Node {

public:
	/** default constructor */ Depot();

    /** constructor */ Depot(const double &x, const double &y);

	//access methods

	// operators
	Depot & operator=(const Depot & d);

	// friends
	friend bool operator==(const Depot& d1, const Depot& d2);
};


// Operators
inline Depot & Depot::operator=(const Depot & d) {
	if (this!=&d) {
		this->nodeId = d.nodeId;
		this->x = d.x;
		this->y = d.y;
	}
	return *this;
}

inline bool operator==(const Depot& d1, const Depot& d2) {
	return (   (d1.x == d2.x)
			&& (d1.y == d2.y));
}

#endif /*DEPOT_H_*/
