/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmMaps.h
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/


#ifndef MMMAPS_H_
#define MMMAPS_H_

namespace mmLib {
namespace mmUtilities{
class mmMaps {
public:
	mmMaps();
	cv::Mat buildMap(std::string mapXmlFile);
	virtual ~mmMaps();
	int shift;
	void setShift(int Shift){shift=Shift;};
private:
	bool _error;
};
}
}

#endif /* MMMAPS_H_ */
