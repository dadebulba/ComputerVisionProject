/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmHomography.h
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/


#ifndef MMHOMOGRAPHY_H_
#define MMHOMOGRAPHY_H_
#include "../contrib/homography/homography.h"
namespace mmLib {
namespace mmUtilities {
class mmHomography {
public:
	mmHomography();
	void createHomographyMatrix(vector<pair<double, double> > pixels, vector<pair<double,double> > metrics, int cameraId, std::string cameraName, bool saveToXmlFile=true);
	void createHomographyMatrix(std::string xmlInputFile,int cameraId, std::string cameraName, bool saveToXmlFile=true);
	void loadMatrix(std::string xmlInputFile);
	cv::Point3f PointToMetrics(cv::Point3f point);
	cv::Point3f MetricsToPoint(cv::Point3f point);
	cv::Mat TransfMatrix;
	cv::Mat InverseTransfMatrix;
	void mmGetTransfMatrix();
	virtual ~mmHomography();


private:
	vector<pair<double, double> > _pixels;
	vector<pair<double, double> > _metrics;
	int _cameraId;
	std::string _cameraName;
	bool _error;
	double _H[3][3];
	double _invH[3][3];
	int _computeHomography(bool saveToXmlFile);

};
}
}

#endif /* MMHOMOGRAPY_H_ */
