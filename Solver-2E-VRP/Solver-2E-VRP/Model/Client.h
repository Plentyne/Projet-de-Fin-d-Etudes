#ifndef CLIENT_H
#define CLIENT_H

// Librairies standards
#include <cmath>

// Fichiers perso
#include "Node.h"

class Client {
	unsigned int id; // archive original client id
	double x;
	double y;
	unsigned int demande;

public:
	// initialization
	/** default constructor */ Client();
	/** constructor */ Client(const unsigned int & id, const double & coordX, const double & coordY, unsigned int & demande);
	/** copying constructor */ Client(const Client & c);

	// access
	/** set original client id */ void setID(const unsigned int & value);
	/** get original client id */ unsigned int getID() const;
	/** set X coordination value */ void setX(const double & value);
	/** get X coordination value */ double getX() const;
	/** set Y coordination value */ void setY(const double & value);
	/** get Y coordination value */ double getY() const;
	/** set demande value */ void setProfit(const unsigned int & value);
	/** get demande value */ unsigned int getProfit() const;

	// method
	/** distance to point */ double distanceTo(const double & x, const double & y) const;
	/** distance to other client */ double distanceTo(const Client & c) const;

	// operator
	Client & operator=(const Client & c);
	friend bool operator==(const Client & c1, const Client & c2);
};


inline Client::Client() {
	this->id = 0;
	this->x = 0.0;
	this->y = 0.0;
	this->demande = 0;
}
inline Client::Client(const unsigned int & id, const double & coordX, const double & coordY, unsigned int & demande)
: id(id), x(coordX), y(coordY), demande(demande) {

}
inline Client::Client(const Client & c) {
	this->id = c.id;
	this->x = c.x;
	this->y = c.y;
	this->demande = c.demande;
}

inline void Client::setID(const unsigned int & value) { this->id = value; }
inline unsigned int Client::getID() const { return this->id; }
inline void Client::setX(const double & value) { this->x = value; }
inline double Client::getX() const { return this->x; }
inline void Client::setY(const double & value) { this->y = value; }
inline double Client::getY() const { return this->y; }
inline void Client::setProfit(const unsigned int & value) { this->demande = value; }
inline unsigned int Client::getProfit() const { return this->demande; }

inline double Client::distanceTo(const double & x, const double & y) const  {
	return sqrt(pow(x - this->x, 2) + pow(x - this->y, 2));
}
inline double Client::distanceTo(const Client & c) const {
	return sqrt(pow(c.x - this->x, 2) + pow(c.y - this->y, 2));
}

inline Client & Client::operator=(const Client & c) {
	if (&c!=this) {
		this->id = c.id;
		this->x = c.x;
		this->y = c.y;
		this->demande = c.demande;
	}

	return *this;
}
inline bool operator==(const Client & c1, const Client & c2) {
	return (   (c1.x == c2.x)
			&& (c1.y == c2.y)
			&& (c1.demande == c2.demande)  );
}

#endif // CLIENT_H
