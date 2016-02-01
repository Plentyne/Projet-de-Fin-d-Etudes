#ifndef NODE_H
#define NODE_H


class Node
{
    public:
        /* Constructeurs */
		Node(double x, double y);

		/* Destructeurs */
        virtual ~Node();
		
		/* Getter & Setters */
        double getId() const { return id; }
        double getX() const { return x; }
        void setX(double val) { x = val; }
        double getY() const { return y; }
        void setY(double val) { y = val; }
		
		/* Class methods */
		

    private:
        static int globalId;
        int id;
        double x;
        double y;
};

#endif // NODE_H
