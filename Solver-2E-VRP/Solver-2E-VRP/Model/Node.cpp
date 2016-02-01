#include "Node.h"

int Node::globalId = 0;

Node::Node(int x, int y) : x(x), y(y)
{
    this->id = Node::globalId++;
}

Node::~Node()
{
    //dtor
}
