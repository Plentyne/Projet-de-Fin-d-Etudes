#include "Node.h"

int Node::globalId = 0;

Node::Node(double x, double y) : x(x), y(y)
{
    this->id = Node::globalId++;
}

Node::~Node()
{
    //dtor
}
