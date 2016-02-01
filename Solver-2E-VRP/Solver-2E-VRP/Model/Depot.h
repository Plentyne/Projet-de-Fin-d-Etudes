#ifndef DEPOT_H_
#define DEPOT_H_

class Depot {
	double x;
	double y;
public:
	/** default constructor */ Depot();
	/** constructor */ Depot(const double & coordX, const double & coordY);
	/** constructor */ Depot(const Depot& d);

	//access methods
	/** set X coordination value */ void setX(const double & value);
	/** get X coordination value */ double getX() const;
	/** set Y coordination value */ void setY(const double & value);
	/** get Y coordination value */ double getY() const;

	// operators
	Depot & operator=(const Depot & d);

	// friends
	friend bool operator==(const Depot& d1, const Depot& d2);
};


inline Depot::Depot() {

}
inline Depot::Depot(const double & coordX, const double & coordY)
	  		 : x(coordX), y(coordY) {

}
inline Depot::Depot(const Depot& d) {
	this->x = d.x;
	this->y = d.y;
}

inline void Depot::setX(const double & value) { this->x = value; }
inline double Depot::getX() const { return this->x; }
inline void Depot::setY(const double & value) { this->y = value; }
inline double Depot::getY() const { return this->y; }
inline Depot & Depot::operator=(const Depot & d) {
	if (this!=&d) {
		this->x = d.x;
		this->y = d.y;
	}
	return *this;
}
inline bool operator==(const Depot& d1, const Depot& d2) {
	return (   (d1.x == d2.x)
			&& (d1.y == d2.y));
}

#endif /*DEPOT_H_*/
