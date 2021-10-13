#ifndef SIZEGEOMETRY_H_
#define SIZEGEOMETRY_H_

#include <iostream>


template<typename T = double>
class SizeGeometry
{
public:
	typedef T ValueType;
	
	SizeGeometry()
    : wd(T(-1)), ht(T(-1)) { }
    
	SizeGeometry(T w, T h)
    : wd(w), ht(h) { }

    bool isEmpty() const;
    bool isValid() const;

    T getWidth() const;
    T getHeight() const;
    void setWidth(T w);
    void setHeight(T h);
    void transpose();
	T getArea() const;

//    void scale(T w, T h);
//    void scale(const Size &s);

//    Size expandedTo(const Size &) const;
//    Size boundedTo(const Size &) const;

//    T &rwidth();
//    T &rheight();

	SizeGeometry<T> &operator+=(const SizeGeometry<T> &);
	SizeGeometry<T> &operator-=(const SizeGeometry<T> &);
    template<typename T2>
    SizeGeometry<T> &operator*=(T2 c);
    template<typename T2>
    SizeGeometry<T> &operator/=(T2 c);

	template<typename T2>
	operator SizeGeometry<T2>() const;

private:
    T wd;
    T ht;
};


// some useful operators
template<typename T2>
bool operator==(const SizeGeometry<T2> &, const SizeGeometry<T2> &);

template<typename T2>
bool operator!=(const SizeGeometry<T2> &, const SizeGeometry<T2> &);

template<typename T2>
const SizeGeometry<T2> operator+(const SizeGeometry<T2> &, const SizeGeometry<T2> &);

template<typename T2>
const SizeGeometry<T2> operator-(const SizeGeometry<T2> &, const SizeGeometry<T2> &);

template<typename T2, typename T3>
const SizeGeometry<T2> operator*(const SizeGeometry<T2> &, T3);

template<typename T2, typename T3>
const SizeGeometry<T2> operator*(T3, const SizeGeometry<T2> &);

template<typename T2, typename T3>
const SizeGeometry<T2> operator/(const SizeGeometry<T2> &, T3);


// stream operators
template<typename T>
std::ostream& operator<<(std::ostream& o, const SizeGeometry<T>& b);

template<typename T>
std::istream& operator>>(std::istream& i, SizeGeometry<T>& b);


#endif /*SIZEGEOMETRY_H_*/
