#ifndef MMBEHAVIORDETECTOR_H
#define MMBEHAVIORDETECTOR_H

//#include <cassert>
//#include <iostream>
//#include <cmath>
//#include <fstream>
//#include <ctime>
//#include <cstdlib>
//
//// boost libs
#include <boost/program_options.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/scoped_ptr.hpp>

#include "opencv/functions.h"
#include "FastVideoGradientComputer.h"
#include "FastHog3DComputer.h"
#include "opencv/Video.h"
#include "numeric/functions.hpp"
#include "c++/4.6/numeric"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

using namespace cv;
using namespace std;

namespace mmLib{
namespace mmInteraction{
struct Position3D{
	double x;
	double y;
	double t;
	double sigma;
	double tau;
	boost::optional<Box3D> box3D;

	Position3D(double x_, double y_, double t_, double sigma_, double tau_)
	: x(x_), y(y_), t(t_), sigma(sigma_), tau(tau_)
	{ }

	Position3D(double x0_, double y0_, double t0_, double width_, double height_, double length_)
	: x(x0_ + 0.5 * width_), y(y0_ + 0.5 * height_), t(t0_ + 0.5 * length_),
	  sigma(0.5 * (width_ + height_)), tau(length_),
	  box3D(Box3D(x0_, y0_, t0_, width_, height_, length_))
	{ }
};

struct Position3DTLess{
	bool operator()(const Position3D& p1, const Position3D& p2) {
		return p1.t < p2.t;
	}
};

struct VideoProperies{
	double frameRate;
	double width;
	double height;
	double character;
	double scaleW;
	double scaleH;
	bool isColor;
};

struct trainParmeters{
	vector<Point> particles;
	vector<int> frameNum;
	double scaleX;
	double scaleY;
	int sigma;
	int tau;
	int gridGranularity;
	int numParticle;
//	int startRec;
//	int stopRec;
	bool isAnomaly;
};

struct mmBehaviourDetectorTrainSettings{
	mmBehaviourDetectorTrainSettings();
	vector<FileStorage> trainFileParam;
	vector<string> videoFilesParam;
	CvTermCriteria clusterTermCritParam;
	CvSVMParams svmParametersParam;
	int dimensionalityParam;
	int clusterBinsParam;
	int clusterAttemptsParam;
};

struct mmBehaviourDetectorTestSettings{
	mmBehaviourDetectorTestSettings();
	FileStorage xmlClusterCentersParam;
	string svmModelStringParam;
	int windowLengthParam;
	int frameStepParam;
	int numFrameAnomalyParam;
};

class mmBehaviourDetector{
public:
	mmBehaviourDetector();
	mmBehaviourDetector(mmLib::mmInteraction::mmBehaviourDetectorTrainSettings &bdTrain);
	mmBehaviourDetector(mmLib::mmInteraction::mmBehaviourDetectorTestSettings &bdTest);
	~mmBehaviourDetector();

	void TrainSvm();
	void TrainSvm(vector<mmLib::mmInteraction::trainParmeters> trainParamVect);
	void InitializeModule();
	bool TestFrame(Mat frame, vector<Point> particleVect);

	vector<vector<float> > getClasterCenterVect();
	CvSVM getSVMmodel();
	int getSigma();
	int getTau();
	int getGridGranularity();
private:
	void CreateParticle(FileStorage xmlFileString);
	void CreateParticle(mmLib::mmInteraction::trainParmeters file);
	int CreateParticle();
	void CreateModel(vector<vector<float> > features, Mat labels);
	void saveCenters(Mat clusterCenters);
	vector<vector<float> >  createHistogram(vector<vector<float> > histValue, Mat particleLabel, vector<int> totalLength, int dimension);
	vector<vector<float> >  normalizeHistogram(vector<vector<float> > histValue);
	vector<vector<float> > computeHugMod(string videoFile, int lenght, int dimensionality);
	bool getAnomaly(int numParticle);

	vector<FileStorage> trainFile;
	vector<string> videoFiles;
	CvTermCriteria clusterTermCrit;
	CvSVMParams svmParameters;
	int dimensionality;
	int clusterBins;
	int clusterAttempts;

	FileStorage xmlClusterCenters;
	string svmModelString;
	int windowLength;
	int frameStep;
	int numFrameAnomaly;

	mmLib::mmInteraction::VideoProperies videoProp;
	vector<vector<float> > clusterCenterVect;
	vector<vector<Point> > particleInWindow;
	vector<Mat> frameInWindow;
	vector<int> frameNumber;
	double scaleX;
	double scaleY;
	int gridGranularity;
	int sigma;
	int tau;
	int totalFrameProcessed;
	int countAnomaly;
	CvSVM mod;
	bool needToTest;
	bool systemAnomaly;
};
}
}

#endif //MMBEHAVIORDETECTOR_H
