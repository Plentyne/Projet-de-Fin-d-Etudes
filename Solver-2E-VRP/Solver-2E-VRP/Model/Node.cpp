#include "Node.h"

// Static attributes initialisation
int Node::globalNodeId = 0;

// Constructors
Node::Node() : x(0), y(0) {
    this->nodeId = Node::globalNodeId++;
}

Node::Node(double x, double y) : x(x), y(y) {
    this->nodeId = Node::globalNodeId++;
}

// Destructors
Node::~Node() {
    //dtor
}

// Class Methods
double Node::distanceTo(Node n) {
    return sqrt(pow(n.x - this->x, 2) + pow(n.y - this->y, 2));
}

double Node::distance(Node a, Node b) {
    return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2));
}

