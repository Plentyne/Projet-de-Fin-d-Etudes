#include "Client.h"

// Static attributes
int Client::globalClientId = 0;

// Constructors
Client::Client() : Node() ,demand(0){
    this->clientId = globalClientId++;
}

Client::Client(const double & x, const double & y, const int & demand)
        : Node(x,y), demand(demand) {
    this->clientId = globalClientId++;
}

