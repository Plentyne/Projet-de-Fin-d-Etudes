#include "Satellite.h"

// Static attributes initialization

int Satellite::satelliteGlobalId = 0;

// Constructors
Satellite::Satellite() : Node()
{
    this->satelliteId = satelliteGlobalId++;
}

Satellite::Satellite(const double &x, const double &y)
        : Node(x, y) {
    this->satelliteId = satelliteGlobalId++;
}