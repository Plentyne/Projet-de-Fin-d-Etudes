#ifndef CLIENT_H
#define CLIENT_H

// Librairies standards
#include <cmath>

// Fichiers perso
#include "Node.h"

class Client : public Node {
private:
    static int globalClientId;
    int clientId;
    int demand;

public:
    // initialization
    /** default constructor */ Client();
    /** constructor */ Client(const double &x, const double &y, const int & demand);
    /** copying constructor */ Client(const Client & c);

    // access
    /** get client id*/ int getClientId() const { return clientId; };
    /** set demand value */ void setDemand(const int & value) { this->demand = value; };
    /** get demand value */ int getDemand()  const { return this->demand; };

    // methods

    // operator
    Client & operator=(const Client & c);
    friend bool operator==(const Client & c1, const Client & c2);
    friend bool operator!=(const Client & c1, const Client & c2);
};

inline Client & Client::operator=(const Client & c) {
    if (&c!=this) {
        this->nodeId = c.nodeId;
        this->clientId = c.clientId;
        this->x = c.x;
        this->y = c.y;
        this->demand = c.demand;
    }

    return *this;
}
inline bool operator==(const Client & c1, const Client & c2) {
    return ( (static_cast<Node>(c1) == static_cast<Node>(c2))
              && (c1.demand == c2.demand)  );
}

inline bool operator!=(const Client & c1, const Client & c2) {
    return !( c1==c2  );
}

#endif // CLIENT_H
