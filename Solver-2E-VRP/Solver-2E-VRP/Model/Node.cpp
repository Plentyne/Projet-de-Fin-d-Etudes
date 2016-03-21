#include "Node.h"

// Static attributes initialisation
int Node::globalNodeId = 0;

// Constructors
Node::Node() : x(0), y(0) {
    //this->nodeId = globalNodeId++;
}

Node::Node(double x, double y) : x(x), y(y) {
    this->nodeId = globalNodeId++;
}

Node::Node(const Node &n) : nodeId(n.nodeId), x(n.x), y(n.y) { };

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

void Node::clear() {
    Node::globalNodeId = 0;
}
