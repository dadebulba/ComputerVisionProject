#ifndef MMGAUSSIANMODEL_H_
#define MMGAUSSIANMODEL_H_

#include <vector>
#include <cv.h>
#include <fstream>
#include <memory>

using namespace std;
using namespace cv;

namespace mmLib{
namespace mmAnomaly{
struct mmGaussianModelSettings{
	mmGaussianModelSettings();
	int numGaussianParam;
	double matchFactorParam;
	double lrweightParam;
	double lrvarianceParam;
	double highVarianceParam;
	double lowWeigthParam;
	double tProvaParam;

	double varianceParam;
	double meanValueParam;
	double weightParam;

	int mWidthParam;
	int mHeightParam;
	int heightParam;
	int widthParam;
	int threshParam;
	int numAnomalyParam;
	int numNormalParam;
};

struct gaussianPixel{
	double variance;
	double meanValue;
	double weight;
};

class mmGaussianModel{
public:
	mmGaussianModel();
	mmGaussianModel(mmLib::mmAnomaly::mmGaussianModelSettings &gmSets);
	~mmGaussianModel();

	void MagnitudeFunt(Mat img, Mat& VelocityImg, vector<Point2f> vectPoints, vector<Point2f>& , vector<uchar> status, int& numFrameInit, bool& isInit);
	Mat updateBackgroundModel(Mat& frame, Mat velocityImage, Mat background);


	void findAnomaly(Mat frame);
	void findAnomalyInsideRect(Mat frame,Rect anomalyRect);
	bool getAnomaly();
private:
	void initializeModel();
	bool findMatchGaussian(Point p, int r);
	void sortGD(int index);
	double getWeightVarianceRatio(int index, int indexGauss);
	bool checkBackground(int index);
	void updateGaussian(int index, int indexGauss, int r, double learningRate);
	int getLPGD(int index);
	void replaceGD(int index, int indexGauss, double r, double variance, double weight);
	void adjustWeight(int index, int indexGauss, double learningRate, bool matched);

	vector<mmLib::mmAnomaly::gaussianPixel> *gaussian;
	int numGaussian;
	int matchedIndex;
	double matchFactor;
	double lrweight;
	double lrvariance;
	double highVariance;
	double lowWeigth;
	double tProva;

	double variance;
	double meanValue;
	double weight;

	int mWidth;
	int mHeight;
	int height;
	int width;
	int thresh;
	int numAnomaly;
	int numNormal;

	int countFrame;
	bool BG;
	bool isAnomaly;
	bool firstFrame;
	vector<Point2f> particleVect;
	mmLib::mmAnomaly::gaussianPixel swap;
};
}
}
#endif /* MMGAUSSIANMODEL_H_ */
