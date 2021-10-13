#ifndef MMPARTICLEACCUMULATION_H
#define MMPARTICLEACCUMULATION_H

using namespace cv;
using namespace std;

namespace mmLib{
namespace mmAnomaly{
struct mmParticleAccumulationSettings{
	mmParticleAccumulationSettings();
	vector<vector<Point> > surveilanceAreaParam;
//	double sigmaParam;
//	double threshParam;
	int minMovementParam;
	int gridGranularityParam;
	int trackPeriodParam;
};

class mmParticleAccumulation{
public:
	mmParticleAccumulation();
	mmParticleAccumulation(mmLib::mmAnomaly::mmParticleAccumulationSettings &paS);
	~mmParticleAccumulation();

	void setUpModule(Mat frame);
	void computeParticleAdvection(Mat frame);
	void clearVariables();

	vector<Point2f> getFinalParticle();
	vector<Point2f> getFirstFrameParticle();
	vector<float> getPathLenght();
	vector<float> getPathLenghtX();
	vector<float> getPathLenghtY();

	Mat getParticleAdvection();
	Mat getFinalParticleMat();
private:
	void selectParticle(Mat frame);

	vector<Point2f> actualPos;
	vector<Point2f> finalParticle;
	vector<Point2f> startParticle;
	vector<float> pathLenght;
	vector<float> pathLenghtFin;
	vector<float> pathLenghtX;
	vector<float> pathLenghtXFin;
	vector<float> pathLenghtY;
	vector<float> pathLenghtYFin;
	vector<Point2f> firstFrameParticle;
	vector<vector<Point> > surveilanceArea;
	Mat prevFrame;
	Mat particleAdvection;
	Mat finalParticleMat;

//	double sigma;
//	double thresh;
	bool firstFrame;
	int minMovement;
	int gridGranularity;
	int trackPeriod;
	int countFrame;
	int totalFrameProcessed;
};
}
}
#endif // MMPARTICLEACCUMULATIONOLD_H


/*
#ifndef MMPARTICLEACCUMULATION_H
#define MMPARTICLEACCUMULATION_H

using namespace cv;
using namespace std;

namespace mmLib{
namespace mmAnomaly{
struct mmParticleAccumulationSettings{
	mmParticleAccumulationSettings();
	vector<vector<Point> > surveilanceAreaParam;
//	double sigmaParam;
//	double threshParam;
	int minMovementParam;
	int gridGranularityParam;
	int trackPeriodParam;
};

class mmParticleAccumulation{
public:
	mmParticleAccumulation();
	mmParticleAccumulation(mmLib::mmAnomaly::mmParticleAccumulationSettings &paS);
	~mmParticleAccumulation();

	void computeParticleAdvection(Mat frame);
	void clearVariables();

	vector<Point2f> getFinalParticle();
	vector<Point2f> getZeroPosParticle();

	Mat getParticleAdvection();
	Mat getFinalParticleMat();

	vector<double> getFinalVelocity();
	vector<double> getFinalVelocityX();
	vector<double> getFinalVelocityY();
	vector<double> getFinalAngle();
private:
	void selectParticle(Mat frame);

	vector<Point2f> zeroPosParticle;
	vector<Point2f> actualPos;
	vector<Point2f> finalParticle;
	vector<Point2f> firstFrameParticle;
	vector<float> pathLenght;
	vector<vector<Point> > surveilanceArea;
	Mat prevFrame;
	Mat particleAdvection;
	Mat finalParticleMat;

	vector<double> finalVelocity;
	vector<double> finalVelocityX;
	vector<double> finalVelocityY;
	vector<double> finalAngle;

	time_t startT;
	time_t endT;

//	double sigma;
//	double thresh;
	int minMovement;
	int gridGranularity;
	int trackPeriod;
	int countFrame;
	int totalFrameProcessed;
	double computeVelocity(Point2f start, Point2f end);
	double computeAngle(Point2f start, Point2f end);
};
}
}
#endif // MMPARTICLEACCUMULATIONOLD_H
*/
