#ifndef SATELLITE_H
#define SATELLITE_H

#include <cmath>


class Satellite {
	unsigned int id; // archive original client id
	double x;
	double y;

public:
	// initialization
	/** default constructor */ Satellite();
	/** constructor */ Satellite(const unsigned int & id, const double & coordX, const double & coordY);
	/** copying constructor */ Satellite(const Satellite & c);

	// access
	/** set original client id */ void setID(const unsigned int & value);
	/** get original client id */ unsigned int getID() const;
	/** set X coordination value */ void setX(const double & value);
	/** get X coordination value */ double getX() const;
	/** set Y coordination value */ void setY(const double & value);
	/** get Y coordination value */ double getY() const;


	// method
	/** distance to point */ double distanceTo(const double & x, const double & y) const;
	/** distance to other client */ double distanceTo(const Satellite & c) const;

	// operator
	Satellite & operator=(const Satellite & c);
	friend bool operator==(const Satellite & c1, const Satellite & c2);
};


inline Satellite::Satellite() {
	this->id = 0;
	this->x = 0.0;
	this->y = 0.0;
}
inline Satellite::Satellite(const unsigned int & id, const double & coordX, const double & coordY)
: id(id), x(coordX), y(coordY) {

}
inline Satellite::Satellite(const Satellite & c) {
	this->id = c.id;
	this->x = c.x;
	this->y = c.y;
}

inline void Satellite::setID(const unsigned int & value) { this->id = value; }
inline unsigned int Satellite::getID() const { return this->id; }
inline void Satellite::setX(const double & value) { this->x = value; }
inline double Satellite::getX() const { return this->x; }
inline void Satellite::setY(const double & value) { this->y = value; }
inline double Satellite::getY() const { return this->y; }

inline double Satellite::distanceTo(const double & x, const double & y) const  {
	return sqrt(pow(x - this->x, 2) + pow(x - this->y, 2));
}
inline double Satellite::distanceTo(const Satellite & c) const {
	return sqrt(pow(c.x - this->x, 2) + pow(c.y - this->y, 2));
}

inline Satellite & Satellite::operator=(const Satellite & c) {
	if (&c!=this) {
		this->id = c.id;
		this->x = c.x;
		this->y = c.y;
	}

	return *this;
}
inline bool operator==(const Satellite & c1, const Satellite & c2) {
	return (   (c1.x == c2.x)
			&& (c1.y == c2.y));
}

#endif // SATELLITE_H
