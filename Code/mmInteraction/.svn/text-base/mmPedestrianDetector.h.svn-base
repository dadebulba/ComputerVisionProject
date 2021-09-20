/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmPedestrianDetector.h
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/


#ifndef MMPEDESTRIANDETECTOR_H_
#define MMPEDESTRIANDETECTOR_H_

namespace mmLib {

namespace mmInteraction{

class mmPedestrianDetector {
public:
	mmPedestrianDetector();
	virtual ~mmPedestrianDetector();
	std::vector<cv::Rect> getPedstrianList(cv::Mat image);
private:
	cv::HOGDescriptor _hog;
};

}

}

#endif /* MMPEDESTRIANDETECTOR_H_ */
