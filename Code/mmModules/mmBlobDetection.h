#ifndef MMBLOBDETECTOR_H_
#define MMBLOBDETECTOR_H_

using namespace cv;
using namespace std;

namespace mmLib {
namespace mmModules{
struct mmBlobDetectorSettings{
	mmBlobDetectorSettings();
	vector<Rect> entranceExitAreaParam;
	int splitThreshParam;
	int borderThreshParam;
	int minDetWidthParam;
	int minDetHeigthParam;
	int increasedTLCornerParam;
	int increasedBRCornerParam;
	int windowSizeParam;
};

class mmBlobDetector{
public:
	mmBlobDetector();
	mmBlobDetector(mmLib::mmModules::mmBlobDetectorSettings &blobSets);
	mmBlobDetector(mmLib::mmModules::mmBlobDetectorSettings &blobSets, mmLib::mmModules::appSettings &appSets);
	~mmBlobDetector();

	vector<mmLib::mmModules::mmBlob> BlobDetection(Mat frame, Mat foreground);
	vector<mmLib::mmModules::mmBlob> BlobDetectionBehaviour(Mat frame, Mat foreground, vector<mmLib::mmModules::mmBlob> activeTracks);
	vector<mmLib::mmModules::mmBlob> BlobDetectionAppModel(Mat frame, Mat foreground, vector<mmLib::mmModules::mmBlob> activeTracks);

	vector<Point> getBlobParticle(Mat frame, vector<mmLib::mmModules::mmBlob> blobList, int gridGranularity);
private:
	vector<mmLib::mmModules::mmBlob> detectNewBlob(Mat frame, Mat foreground);
	mmLib::mmModules::mmBlob setParameters(Mat frame, Mat foreground, mmLib::mmModules::mmBlob blobNew, mmLib::mmModules::mmBlob blobOld, mmLib::mmModules::BLOB_TYPE type);
	bool isBlobInWantedRegion(Mat frame, mmLib::mmModules::mmBlob blob);
	bool areBlobsContained(mmLib::mmModules::mmBlob blob1, mmLib::mmModules::mmBlob blob2);

	mmLib::mmModules::appModel* aMod;
	vector<Rect> entranceExitArea;
	int splitThresh;
	int borderThresh;
	int minDetWidth;
	int minDetHeigth;
	int increasedTLCorner;
	int increasedBRCorner;
	int windowSize;
	int totalFrameProcessed;
	int startAnomaly;
	bool needToTrain;
};
bool isPointInRect(Rect rect, Point p);
static int CompContour(const void* a, const void* b, void* );
}
}

#endif /* MMTRACKER_H_ */
