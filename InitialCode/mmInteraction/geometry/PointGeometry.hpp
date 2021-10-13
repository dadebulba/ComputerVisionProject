#ifndef POINT_HPP_
#define POINT_HPP_

#include "PointGeometry.h"
#include <cassert>
#include "../numeric/functions.hpp"


template<typename T>
inline bool PointGeometry<T>::isNull() const
{
    return xp == T(0) && yp == T(0);
}

template<typename T>
inline T PointGeometry<T>::getX() const
{
    return xp;
}

template<typename T>
inline T PointGeometry<T>::getY() const
{
    return yp;
}

template<typename T>
inline void PointGeometry<T>::setX(T xpos)
{
    xp = xpos;
}

template<typename T>
inline void PointGeometry<T>::setY(T ypos)
{
    yp = ypos;
}

template<typename T>
inline PointGeometry<T> &PointGeometry<T>::operator+=(const PointGeometry<T> &p)
{
    xp += p.xp;
    yp += p.yp;
    return *this;
}

template<typename T>
inline PointGeometry<T> &PointGeometry<T>::operator-=(const PointGeometry<T> &p)
{
    xp-=p.xp; yp-=p.yp; return *this;
}



template<typename T>
template<typename T2>
inline PointGeometry<T> &PointGeometry<T>::operator*=(T2 c)
{
    xp = static_cast<T>(xp * c);
    yp = static_cast<T>(yp * c);
    return *this;
}

template<>
template<>
inline PointGeometry<int> &PointGeometry<int>::operator*=(float c)
{
    xp = static_cast<int>(roundf(xp * c));
    yp = static_cast<int>(roundf(yp * c));
    return *this;
}

template<>
template<>
inline PointGeometry<long> &PointGeometry<long>::operator*=(float c)
{
    xp = static_cast<long>(roundf(xp * c));
    yp = static_cast<long>(roundf(yp * c));
    return *this;
}

template<>
template<>
inline PointGeometry<int> &PointGeometry<int>::operator*=(double c)
{
    xp = static_cast<int>(round(xp * c));
    yp = static_cast<int>(round(yp * c));
    return *this;
}

template<>
template<>
inline PointGeometry<long> &PointGeometry<long>::operator*=(double c)
{
    xp = static_cast<long>(round(xp * c));
    yp = static_cast<long>(round(yp * c));
    return *this;
}



template<typename T>
template<typename T2>
inline PointGeometry<T> &PointGeometry<T>::operator/=(T2 c)
{
    assert(!fuzzyEqual(c, 0));
    xp = static_cast<T>(xp / c);
    yp = static_cast<T>(yp / c);
    return *this;
}

template<>
template<>
inline PointGeometry<int> &PointGeometry<int>::operator/=(float c)
{
    assert(c != float(0));
    xp = static_cast<int>(round(xp / c));
    yp = static_cast<int>(round(yp / c));
    return *this;
}

template<>
template<>
inline PointGeometry<long> &PointGeometry<long>::operator/=(float c)
{
    assert(c != float(0));
    xp = static_cast<long>(round(xp / c));
    yp = static_cast<long>(round(yp / c));
    return *this;
}

template<>
template<>
inline PointGeometry<int> &PointGeometry<int>::operator/=(double c)
{
    assert(c != double(0));
    xp = static_cast<int>(round(xp / c));
    yp = static_cast<int>(round(yp / c));
    return *this;
}

template<>
template<>
inline PointGeometry<long> &PointGeometry<long>::operator/=(double c)
{
    assert(c != double(0));
    xp = static_cast<long>(round(xp / c));
    yp = static_cast<long>(round(yp / c));
    return *this;
}



template<typename T>
inline bool operator==(const PointGeometry<T> &p1, const PointGeometry<T> &p2)
{
    return p1.getX() == p2.getX() && p1.getY() == p2.getY();
}

template<>
inline bool operator==(const PointGeometry<float> &p1, const PointGeometry<float> &p2)
{
    return fuzzyEqual(p1.getX(), p2.getX()) && fuzzyEqual(p1.getY(), p2.getY());
}

template<>
inline bool operator==(const PointGeometry<double> &p1, const PointGeometry<double> &p2)
{
    return fuzzyEqual(p1.getX(), p2.getX()) && fuzzyEqual(p1.getY(), p2.getY());
}



template<typename T>
inline bool operator!=(const PointGeometry<T> &p1, const PointGeometry<T> &p2)
{
    return p1.getX() != p2.getX() || p1.getY() != p2.getY();
}

template<>
inline bool operator!=(const PointGeometry<float> &p1, const PointGeometry<float> &p2)
{
    return !fuzzyEqual(p1.getX(), p2.getX()) || !fuzzyEqual(p1.getY(), p2.getY());
}

template<>
inline bool operator!=(const PointGeometry<double> &p1, const PointGeometry<double> &p2)
{
    return !fuzzyEqual(p1.getX(), p2.getX()) || !fuzzyEqual(p1.getY(), p2.getY());
}



template<typename T>
inline const PointGeometry<T> operator+(const PointGeometry<T> &p1, const PointGeometry<T> &p2)
{
    return PointGeometry<T>(p1.getX()+p2.getX(), p1.getY()+p2.getY());
}

template<typename T>
inline const PointGeometry<T> operator-(const PointGeometry<T> &p1, const PointGeometry<T> &p2)
{
    return PointGeometry<T>(p1.getX()-p2.getX(), p1.getY()-p2.getY());
}

template<typename T, typename T2>
inline const PointGeometry<T> operator*(const PointGeometry<T> &p, T2 c)
{
	PointGeometry<T> point = p;
    point *= c;
    return point;
}

template<typename T, typename T2>
inline const PointGeometry<T> operator*(T2 c, const PointGeometry<T> &p)
{
	PointGeometry<T> point = p;
    point *= c;
    return point;
}

template<typename T>
inline const PointGeometry<T> operator-(const PointGeometry<T> &p)
{
    return PointGeometry<T>(-p.getX(), -p.getY());
}

template<typename T, typename T2>
inline const PointGeometry<T> operator/(const PointGeometry<T> &p, T2 c)
{
	PointGeometry<T> point = p;
	point /= c;
	return point;
}



template<typename T>
template<typename T2>
inline PointGeometry<T>::operator PointGeometry<T2>() const
{
	return PointGeometry<T2>(static_cast<T2>(getX()), static_cast<T2>(getY()));
}

template<>
template<>
inline PointGeometry<float>::operator PointGeometry<int>() const
{
	return PointGeometry<int>(static_cast<int>(roundf(getX())), static_cast<int>(roundf(getY())));
}

template<>
template<>
inline PointGeometry<float>::operator PointGeometry<long>() const
{
	return PointGeometry<long>(static_cast<long>(roundf(getX())), static_cast<long>(roundf(getY())));
}

template<>
template<>
inline PointGeometry<double>::operator PointGeometry<int>() const
{
	return PointGeometry<int>(static_cast<int>(round(getX())), static_cast<int>(round(getY())));
}

template<>
template<>
inline PointGeometry<double>::operator PointGeometry<long>() const
{
	return PointGeometry<long>(static_cast<long>(round(getX())), static_cast<long>(round(getY())));
}



template<typename T>
inline std::ostream& operator<<(std::ostream& o, const PointGeometry<T>& p)
{
	o << p.getX() << ", " << p.getY();
	return o;
}

template<typename T>
std::istream& operator>>(std::istream& i, PointGeometry<T>& p)
{
	T x, y;
	i.setf(std::ios_base::skipws);
	i >> x;
	i.ignore(256, ',');
	i >> y;
	p = PointGeometry<T>(x, y);
	return i;
}


#endif
