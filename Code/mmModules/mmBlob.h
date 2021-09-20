#ifndef MMBLOB_H_
#define MMBLOB_H_

using namespace cv;

namespace mmLib {
namespace mmModules{
enum BLOB_TYPE{
	STANDARD,MERGE,NONMERGE,NEW,SPLIT,NONVISIBLE,DELETED
};

class mmBlob{
public:
	mmBlob();
	mmBlob(Mat frame, Point tl, Size sz);
	mmBlob(Mat frame, int x, int y, int w, int h);
	mmBlob(Mat frame, vector<Point> blobPoints);
	~mmBlob();

	Rect boundingBoxProcessed();
	Point centroid();
	Point getTL();
	Point getBR();
	Mat getImage();
	Mat getForeground();
	void setForeground(Mat f);
	int getBlobArea();
	int getWidth();
	int getHeight();
	int getDepth();
	void setDepth(int d);
	Point getDispVect();
	void setDispVect(Point p);
	vector<Point> getCentList();
	void setCentList(vector<Point> pList);
	vector<Point> getPointList();
	void setPointList(vector<Point> pList);
	Mat getM();
	void setM(Mat mat);
	Mat getP();
	void setP(Mat mat);
	BLOB_TYPE getType();
	void setType(BLOB_TYPE ty);
	Scalar getColor();
	void setColor(Scalar col);

	void pushPoint(Point p);
	bool isConteined(Point pt);
	void getBins(float* histBlue, float* histGreen, float* histRed);
	float assignedMSE;
	int blobID;
	vector<int> bindedKalman;
private:
	Mat blobBuilder(Mat frame);

	vector<Point> centList;
	vector<Point> pointList;
	Mat foreground;
	Mat image;
	Mat M;
	Mat prob;
	Point dispVect;
	Scalar color;
	BLOB_TYPE type;
	bool isFgSet;
	int coordX;
	int coordY;
	int width;
	int height;
	int depth;
};
Scalar assignRandomColor();
int getDist(mmBlob blob1, mmBlob blob2);
double getDistFromBlob(mmBlob blob,Point p);
Mat mmDrawBlobs(Mat img, vector<mmBlob> blobList);
Mat mmDrawBlobsAndPoints(Mat img, Mat foreground, vector<mmBlob> blobList);
Mat mmPrintTrack(Mat img, vector<mmBlob> blobList);

void mmShowHistogram(float* hist, String wName);
float mmHistogramMatchingMSE(float* histA, float* histB);
//float mmFullHistMatching(Mat a, Mat b);
float test_matching(Mat matA, Mat matB);
void my_hist (Mat image, float* histBlue, float* histGreen, float* histRed);
float HistogramMatching(float* histA, float* histB);
void HistogramMatching_v2 (Mat a, Mat b, float * Hred, float * Hblue, float * Hgreen);
float test_matching_v2(Mat matA, Mat backgroundA, Mat matB, Mat backgroundB);
}
}
#endif /* MMBLOB_H_ */
