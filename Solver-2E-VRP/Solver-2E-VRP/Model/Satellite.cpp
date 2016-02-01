#include "Satellite.h"

// Static attributes initialization

int Satellite::satelliteGlobalId = 0;

// Constructors
inline Satellite::Satellite() : Node(), maxCf(0)
{
    this->satelliteId = satelliteGlobalId++;
}

inline Satellite::Satellite(const double &x, const double &y, const int &maxCf)
        : Node(x,y) , maxCf(maxCf) {
    this->satelliteId = satelliteGlobalId++;
}