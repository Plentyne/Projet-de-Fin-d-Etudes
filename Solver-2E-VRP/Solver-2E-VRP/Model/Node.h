#ifndef NODE_H
#define NODE_H


class Node
{
    public:
        Node(int x, int y);
        virtual ~Node();
        int Getid() { return id; }
        int Getx() { return x; }
        void Setx(int val) { x = val; }
        int Gety() { return y; }
        void Sety(int val) { y = val; }

    private:
        static int globalId;
        int id;
        int x;
        int y;
};

#endif // NODE_H
