#ifndef NODE_H
#define NODE_H


class Node
{
    public:
        Node(int x, int y);
        virtual ~Node();
        int getId() { return id; }
        int getX() { return x; }
        void setX(int val) { x = val; }
        int getY() { return y; }
        void setY(int val) { y = val; }



    private:
        static int globalId;
        int id;
        int x;
        int y;
};

#endif // NODE_H
