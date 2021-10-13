/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmPedestrianDetector.cpp
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/

#include "../mmLib.h"
//#include "mmPedestrianDetector.h"
namespace mmLib {

namespace mmInteraction {

mmPedestrianDetector::mmPedestrianDetector() {
	_hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

}

mmPedestrianDetector::~mmPedestrianDetector() {
}
std::vector<cv::Rect> mmPedestrianDetector::getPedstrianList(cv::Mat image)
{

	std::vector<cv::Rect> found, foundFiltered;
//	if((image.size().height < 70) && (image.size().width < 35))
//		return foundFiltered;
	_hog.detectMultiScale(image, found, 0, cv::Size(8,8), cv::Size(32,32), 1.05, 2);
	size_t i, j;
	for( i = 0; i < found.size(); i++ )
	{
		cv::Rect r = found[i];
		for( j = 0; j < found.size(); j++ )
			if( j != i && (r & found[j]) == r)
				break;
		if( j == found.size() )
		{
			// the HOG detector returns slightly larger rectangles than the real objects.
			// so we slightly shrink the rectangles to get a nicer output.
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8);
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8);
			foundFiltered.push_back(r);
		}
	}
	return foundFiltered;
}
}

}
