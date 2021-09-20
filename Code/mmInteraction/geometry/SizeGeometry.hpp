#ifndef SIZE_HPP_
#define SIZE_HPP_

#include "SizeGeometry.h"
#include <cassert>
#include <stdexcept>
#include "../numeric/functions.hpp"

template<typename T>
inline bool SizeGeometry<T>::isEmpty() const
{ return !isValid() || wd == T(0) || ht == T(0); }

template<typename T>
inline bool SizeGeometry<T>::isValid() const
{ return wd >= T(0) && ht >= T(0); }

template<typename T>
inline T SizeGeometry<T>::getWidth() const
{ return wd; }

template<typename T>
inline T SizeGeometry<T>::getHeight() const
{ return ht; }

template<typename T>
inline void SizeGeometry<T>::setWidth(T w)
{ wd = w; }

template<typename T>
inline void SizeGeometry<T>::setHeight(T h)
{ ht = h; }

template<typename T>
inline T SizeGeometry<T>::getArea() const
{
	if (wd <= T(0) || ht <= T(0))
		return T(0);
	return wd * ht;
}

template<typename T>
inline SizeGeometry<T> &SizeGeometry<T>::operator+=(const SizeGeometry<T> &s)
{ wd += s.wd; ht += s.ht; return *this; }

template<typename T>
inline SizeGeometry<T> &SizeGeometry<T>::operator-=(const SizeGeometry<T> &s)
{ wd -= s.wd; ht -= s.ht; return *this; }



template<typename T>
template<typename T2>
inline SizeGeometry<T> &SizeGeometry<T>::operator*=(T2 c)
{ wd *= c; ht *= c; return *this; }

template<typename T>
template<typename T2>
inline SizeGeometry<T> &SizeGeometry<T>::operator/=(T2 c)
{
    assert(c != T(0));
    wd = wd/c; ht = ht/c;
    return *this;
}

template<>
template<>
inline SizeGeometry<int> &SizeGeometry<int>::operator*=(float c)
{ wd = static_cast<int>(roundf(c * wd)); ht = static_cast<int>(roundf(c * ht)); return *this; }

template<>
template<>
inline SizeGeometry<int> &SizeGeometry<int>::operator/=(float c)
{
    assert(c != float(0));
    wd = static_cast<int>(roundf(wd / c)); ht = static_cast<int>(roundf(ht / c));
    return *this;
}

template<>
template<>
inline SizeGeometry<long> &SizeGeometry<long>::operator*=(float c)
{ wd = static_cast<long>(roundf(c * wd)); ht = static_cast<long>(roundf(c * ht)); return *this; }

template<>
template<>
inline SizeGeometry<long> &SizeGeometry<long>::operator/=(float c)
{
    assert(c != float(0));
    wd = static_cast<long>(roundf(wd / c)); ht = static_cast<long>(roundf(ht / c));
    return *this;
}

template<>
template<>
inline SizeGeometry<int> &SizeGeometry<int>::operator*=(double c)
{ wd = static_cast<int>(round(c * wd)); ht = static_cast<int>(round(c * ht)); return *this; }

template<>
template<>
inline SizeGeometry<int> &SizeGeometry<int>::operator/=(double c)
{
    assert(c != double(0));
    wd = static_cast<int>(round(wd / c)); ht = static_cast<int>(round(ht / c));
    return *this;
}

template<>
template<>
inline SizeGeometry<long> &SizeGeometry<long>::operator*=(double c)
{ wd = static_cast<long>(round(c * wd)); ht = static_cast<long>(round(c * ht)); return *this; }

template<>
template<>
inline SizeGeometry<long> &SizeGeometry<long>::operator/=(double c)
{
    assert(c != double(0));
    wd = static_cast<long>(round(wd / c)); ht = static_cast<long>(round(ht / c));
    return *this;
}



template<typename T>
inline bool operator==(const SizeGeometry<T> &s1, const SizeGeometry<T> &s2)
{ return s1.getWidth() == s2.getWidth() && s1.getHeight() == s2.getHeight(); }

template<typename T>
inline bool operator!=(const SizeGeometry<T> &s1, const SizeGeometry<T> &s2)
{ return s1.getWidth() != s2.getWidth() || s1.getHeight() != s2.getHeight(); }

template<>
inline bool operator==(const SizeGeometry<float> &s1, const SizeGeometry<float> &s2)
{ return fuzzyEqual(s1.getWidth(), s2.getWidth()) && fuzzyEqual(s1.getHeight(), s2.getHeight()); }

template<>
inline bool operator!=(const SizeGeometry<float> &s1, const SizeGeometry<float> &s2)
{ return !fuzzyEqual(s1.getWidth(), s2.getWidth()) || !fuzzyEqual(s1.getHeight(), s2.getHeight()); }

template<>
inline bool operator==(const SizeGeometry<double> &s1, const SizeGeometry<double> &s2)
{ return fuzzyEqual(s1.getWidth(), s2.getWidth()) && fuzzyEqual(s1.getHeight(), s2.getHeight()); }

template<>
inline bool operator!=(const SizeGeometry<double> &s1, const SizeGeometry<double> &s2)
{ return !fuzzyEqual(s1.getWidth(), s2.getWidth()) || !fuzzyEqual(s1.getHeight(), s2.getHeight()); }




template<typename T>
inline const SizeGeometry<T> operator+(const SizeGeometry<T> & s1, const SizeGeometry<T> & s2)
{ return SizeGeometry<T>(s1.getWidth()+s2.getWidth(), s1.getHeight()+s2.getHeight()); }

template<typename T>
inline const SizeGeometry<T> operator-(const SizeGeometry<T> &s1, const SizeGeometry<T> &s2)
{ return SizeGeometry<T>(s1.getWidth()-s2.getWidth(), s1.getHeight()-s2.getHeight()); }

template<typename T, typename T2>
inline const SizeGeometry<T> operator*(const SizeGeometry<T> &s, T2 c)
{
	SizeGeometry<T> SizeGeometry = s;
	SizeGeometry *= c;
	return SizeGeometry;
}

template<typename T, typename T2>
inline const SizeGeometry<T> operator*(T2 c, const SizeGeometry<T> &s)
{
	SizeGeometry<T> SizeGeometry = s;
	SizeGeometry *= c;
	return SizeGeometry;
}

template<typename T>
inline const SizeGeometry<T> operator/(const SizeGeometry<T> &s, T c)
{
	SizeGeometry<T> SizeGeometry = s;
	SizeGeometry /= c;
	return SizeGeometry;
}

template<typename T>
inline void SizeGeometry<T>::transpose()
{
    T tmp = wd;
    wd = ht;
    ht = tmp;
}

template<typename T>
template<typename T2>
inline SizeGeometry<T>::operator SizeGeometry<T2>() const
{
	return SizeGeometry<T2>(static_cast<T2>(getWidth()), static_cast<T2>(getHeight()));
}

template<>
template<>
inline SizeGeometry<float>::operator SizeGeometry<int>() const
{
	return SizeGeometry<int>(static_cast<int>(roundf(getWidth())), static_cast<int>(roundf(getHeight())));
}

template<>
template<>
inline SizeGeometry<float>::operator SizeGeometry<long>() const
{
	return SizeGeometry<long>(static_cast<long>(roundf(getWidth())), static_cast<long>(roundf(getHeight())));
}


template<>
template<>
inline SizeGeometry<double>::operator SizeGeometry<int>() const
{
	return SizeGeometry<int>(static_cast<int>(round(getWidth())), static_cast<int>(round(getHeight())));
}

template<>
template<>
inline SizeGeometry<double>::operator SizeGeometry<long>() const
{
	return SizeGeometry<long>(static_cast<long>(round(getWidth())), static_cast<long>(round(getHeight())));
}


template<typename T>
inline std::ostream& operator<<(std::ostream& o, const SizeGeometry<T>& s)
{
	o << s.getWidth() << " x " << s.getHeight();
	return o;
}

template<typename T>
std::istream& operator>>(std::istream& i, SizeGeometry<T>& s)
{
	T w, h;
	i.setf(std::ios_base::skipws);
	i >> w;
	char c = i.peek();
	while (c == '\t' || c == ' ' || c == ',' || c == 'x') {
		i.get(c);
		c = i.peek();
		if (!i.good())
			throw std::runtime_error("SizeGeometry: unexpected error/end reading the stream");
	}
	i >> h;
	s = SizeGeometry<T>(w, h);
	return i;
}


#endif
