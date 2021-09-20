#ifndef POINTGEOMETRY_H_
#define POINTGEOMETRY_H_

#include <iostream>

template<typename T>
class PointGeometry
{
public:
	typedef T ValueType;
	
	PointGeometry()
    : xp(0), yp(0) { }
    
	PointGeometry(T xpos, T ypos)
    : xp(xpos), yp(ypos) { }

    bool isNull() const;

    T getX() const;
    T getY() const;
    void setX(T x);
    void setY(T y);

    PointGeometry<T> &operator+=(const PointGeometry<T> &p);
    PointGeometry<T> &operator-=(const PointGeometry<T> &p);
    template<typename T2>
    PointGeometry<T> &operator*=(T2 c);
    template<typename T2>
    PointGeometry<T> &operator/=(T2 c);

	template<typename T2>
	operator PointGeometry<T2>() const;

private:
    T xp;
    T yp;
};


// some usefule operators
template<typename T>
bool operator==(const PointGeometry<T> &, const PointGeometry<T> &);

template<typename T>
bool operator!=(const PointGeometry<T> &, const PointGeometry<T> &);

template<typename T>
const PointGeometry<T> operator+(const PointGeometry<T> &, const PointGeometry<T> &);

template<typename T>
const PointGeometry<T> operator-(const PointGeometry<T> &, const PointGeometry<T> &);

template<typename T>
const PointGeometry<T> operator-(const PointGeometry<T> &);

template<typename T2, typename T3>
const PointGeometry<T2> operator*(T3, const PointGeometry<T2> &);

template<typename T2, typename T3>
const PointGeometry<T2> operator*(const PointGeometry<T2> &, T3);

template<typename T2, typename T3>
const PointGeometry<T2> operator/(const PointGeometry<T2> &, T3);


// stream operators
template<typename T>
std::ostream& operator<<(std::ostream& o, const PointGeometry<T>& b);

template<typename T>
std::istream& operator>>(std::istream& i, PointGeometry<T>& b);

#endif /*POINTGEOMETRY_H_*/
