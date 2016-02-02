#include "Satellite.h"

// Static attributes initialization

int Satellite::satelliteGlobalId = 0;

// Constructors
inline Satellite::Satellite() : Node()
{
    this->satelliteId = satelliteGlobalId++;
}

inline Satellite::Satellite(const double &x, const double &y)
        : Node(x, y) {
    this->satelliteId = satelliteGlobalId++;
}