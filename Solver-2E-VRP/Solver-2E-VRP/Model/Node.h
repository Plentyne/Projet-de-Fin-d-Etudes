#ifndef NODE_H
#define NODE_H

#include <cmath>
#include <typeinfo>


class Node {
private:
    static int globalNodeId;

protected:
    int nodeId;
    double x;
    double y;

public:
    // Constructeurs
    Node();
    Node(double x, double y);

    Node(const Node &n);


    // Destructeurs
    virtual ~Node();

    // Access methods
    double getNodeId() const { return nodeId; }
    double getX() const { return x; }
    void setX(double val) { x = val; }
    double getY() const { return y; }
    void setY(double val) { y = val; }

    // Class methods
    double distanceTo(Node n);

    // Static methods
    static double distance(Node a, Node b);

    // Operators
    friend bool operator== (const Node & a, const Node & b);
    friend bool operator!=(const Node & a, const Node & b);

};

// Operators implementation
inline bool operator==(const Node & a, const Node & b) {
    if (typeid(a) != typeid(b)) return false;
    return (a.x == b.x) && (a.y == b.y);
}

inline bool operator!=(const Node & a, const Node & b){
    return !(a==b);
};

#endif // NODE_H
