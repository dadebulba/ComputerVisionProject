/*
 * blobDetection.h
 *
 *  Created on: Nov 24, 2010
 *      Author: paolo
 */

#ifndef BLOBDETECTION_H_
#define BLOBDETECTION_H_

#include <cvaux.h>
#include <cvaux.hpp>

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION < 2
#include <cxtypes.h>
#include <cvvidsurv.hpp>
#endif

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2
#include "opencv2/core/types_c.h"
#include "opencv2/legacy/blobtrack.hpp"
#endif
#include <vector>
#include <cv.h>
#include <fstream>

using namespace std;
using namespace cv;

namespace mmLib {

namespace mmModules {

class clusterSearch:public CvBlobDetector
{
public:
	clusterSearch();
	~clusterSearch();
	int DetectNewBlob(IplImage* pImg, IplImage* pFGMask, CvBlobSeq* pNewBlobList, CvBlobSeq* /*pOldBlobList*/);
	void Release();



private:
	IplImage*       m_pMaskBlobNew;

};

class clnf:public CvBlobDetector
{
public:
	clnf();
	~clnf();
	int DetectNewBlob(IplImage* pImg, IplImage* pFGMask, CvBlobSeq* pNewBlobList, CvBlobSeq* /*pOldBlobList*/);
	void Release();

	// Settings
	double tolH;
	double tolW;
};
double getDistFromC(Point p, vector<Point> clusterP);
CvBlobDetector* CreateClnf();
CvBlobDetector* CreateClusterSearch();
void FindBlobsByCCClasters(IplImage* pFG, CvBlobSeq* pBlobs, CvMemStorage* storage);
static int CompareContour(const void* a, const void* b, void* );
vector<mmLib::mmModules::mmBlob> BlobKiller(Mat frame, vector<mmLib::mmModules::mmBlob> blobList);
vector<mmLib::mmModules::mmBlob> BlobChoice(Mat frame, vector<mmLib::mmModules::mmBlob> blobList);
vector<mmLib::mmModules::mmBlob> SizeFilter(Mat frame, Mat foreground, vector<mmLib::mmModules::mmBlob> blobList,int minBlobWidth, int minBlobHeigth, int setTL, int setBR);
vector<mmLib::mmModules::mmBlob> UnionObjects(Mat frame, Mat foreground, vector<mmLib::mmModules::mmBlob> blobList);
}

}

#endif /* BLOBDETECTION_H_ */

