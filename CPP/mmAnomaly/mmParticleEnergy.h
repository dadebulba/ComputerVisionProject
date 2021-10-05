#ifndef MMPARTICLENERGY_H
#define MMPARTICLENERGY_H

using namespace cv;
using namespace std;

namespace mmLib{
namespace mmAnomaly{
enum ANOMALY_TYPE{
	STANDARD,LOWANOMALY,HIGHANOMALY,
};

struct mmParticleEnergySettings{
	mmParticleEnergySettings();
	vector<vector<float> > profileEnVectParam;
	vector<Rect> surveilanceRectParam;
	Size surveilanceRectDimParam;
	Size filterSizeParam;
	Size surroundingAreaParam;
	double sigmaParam;
	double threshHighParam;
	double threshLowParam;
	int valuePeriodParam;
	int profilePeriodParam;
	int profileFrameStartParam;
	int profileLengthParam;
	int trackingPeriodParam;
};

class mmParticleEnergy{
public:
	mmParticleEnergy();
	mmParticleEnergy(mmLib::mmAnomaly::mmParticleEnergySettings &peSet);
	~mmParticleEnergy();

	void setUpModule(Mat frame);
	void setProfileEnVect(vector<vector<float> > energyVect);
	void clearVariables();

	void processFrame(Mat frame, vector<Point2f> particles);

	vector<bool> getAnomaly();
	vector<mmLib::mmAnomaly::ANOMALY_TYPE> getAnomalyType();
	vector<float> getProfileEnVect(int index);
	vector<float> getValueEnVect();
	Mat getMatProfile();
	Mat getMatValue();
	bool getRec();
private:
	Mat computeParticleEnergy(Mat energyMatrix, vector<Point2f> particles);
	vector<float> computeEnergyPerArea(Mat energyMatrix);

	vector<vector<float> > profileEnVect;
	vector<float> valueEnVect;
	vector<Rect> surveilanceRect;
//	vector<bool> isAnomaly;
	Size surveilanceRectDim;
	Size filterSize;
	Size surroundingArea;
	double sigma;
	double threshHigh;
	double threshLow;
	int valuePeriod;
	int profilePeriod;
	int profileFrameStart;
	int profileLength;
	int trackingPeriod;
	int countFrame;
	int totalFrameProcessed;
	int profileEnIndex;
	bool firstFrame;
	bool needToRec;

	Mat matProfile;
	Mat matValue;
};
}
}
#endif // MMPARTICLENERGY_H
