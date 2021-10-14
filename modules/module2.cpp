
#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <math.h>
#include <numeric>
#include "../mmLib.h"
#include <iostream>
#include "./module2.h"

using namespace cv;
using namespace std;

int module2(module2Config &config, bool verbose) {
	Mat prevGray, frame, gray, backgImage, VelocityImage;
	cv::VideoCapture capture(config.videoPath);

	int isColor = 1;
	int fps     = capture.get(CV_CAP_PROP_FPS);
	int frameW  = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	int frameH  = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

    if (!capture.isOpened())
        return 0;
    capture.read(frame);

    int const rows = frame.rows;
    int const cols = frame.cols;
    int nFrameCount = 0;
    int nFrameInit = 0;
    prevGray = Mat(rows,cols,CV_8UC1,cv::Scalar(0));

    bool needToInit = false;
	bool stopInit = true;

	TermCriteria termcrit = TermCriteria((TermCriteria::COUNT) + (TermCriteria::EPS), 10, 0.03);
    Size winSize(10,10);
    vector<Point2f> points[2];
    vector<Point2f> tempoints;

    Point2f Singlept;

    vector<int> particlesMoved;
    //initialize variable
    mmLib::mmAnomaly::mmGaussianModelSettings gmSets;
    gmSets.heightParam = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    gmSets.widthParam = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    gmSets.threshParam = config.threshold; //number of abnormal particle to consider the frame as abnormal
    gmSets.numAnomalyParam = config.numAnomalyParam; //number of abnormal consecutive frames to raise the alarm
    gmSets.numNormalParam = config.numNormalParam; //number of normal consecutive frames to raise down the alarm

	//create object
    mmLib::mmAnomaly::mmGaussianModel* GM = new mmLib::mmAnomaly::mmGaussianModel(gmSets);

    //create the 4 windows
	backgImage = Mat(rows,cols,CV_8UC3,CV_RGB(0,0,0));
	VelocityImage = Mat(rows,cols,CV_8UC1,cv::Scalar(0));
	namedWindow("GMM result", 1);
	cvMoveWindow("GMM result",0,0);

    while (true){
		++nFrameCount;
		if (!capture.read(frame)){
			break;
		}
		cvtColor(frame,gray,COLOR_RGB2GRAY);

	    GaussianBlur(gray,gray,Size(5,5),0,0, BORDER_DEFAULT);

		if(stopInit == true){
			needToInit = true;
			stopInit = false;
		}

		//1. Init particles grid (done every 4 frame)
		if( needToInit ){

			points[1].clear();
            for(int r=0; r < rows; r=r+8)
            for(int c=0; c < cols; c=c+8){
				Singlept.x = c;
				Singlept.y = r;
                points[1].push_back(Singlept);
			}
			needToInit = false;
		}
		//proces frames
		else if(!points[0].empty()){
			vector<uchar> status;
			vector<float> err;

			/*
			 * prevImg	first 8-bit input image or pyramid constructed by buildOpticalFlowPyramid.
			 * nextImg	second input image or pyramid of the same size and the same type as prevImg.
			 *	prevPts	vector of 2D points for which the flow needs to be found; point coordinates must be single-precision floating-point numbers.
			 *	nextPts	output vector of 2D points (with single-precision floating-point coordinates) containing the calculated new positions of input features in the second image; when OPTFLOW_USE_INITIAL_FLOW flag is passed, the vector must have the same size as in the input.
			 *	status	output status vector (of unsigned chars); each element of the vector is set to 1 if the flow for the corresponding features has been found, otherwise, it is set to 0.
			 *	err	output vector of errors; each element of the vector is set to an error for the corresponding feature, type of the error measure can be set in flags parameter; if the flow wasn't found then the error is not defined (use the status parameter to find such cases).
			 *	winSize	size of the search window at each pyramid level.
			 *	maxLevel	0-based maximal pyramid level number; if set to 0, pyramids are not used (single level), if set to 1, two levels are used, and so on; if pyramids are passed to input then algorithm will use as many levels as pyramids have but no more than maxLevel.
			 *	criteria	parameter, specifying the termination criteria of the iterative search algorithm (after the specified maximum number of iterations criteria.maxCount or when the search window moves by less than criteria.epsilon.
			*/
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0);

			GM->MagnitudeFunt(frame, VelocityImage, points[1], tempoints, status, nFrameInit, needToInit);

			if(nFrameInit == 1 && needToInit){
				backgImage = GM->updateBackgroundModel(frame,VelocityImage,backgImage);
				particlesMoved.push_back(GM->findAnomaly(frame));

				//imshow("background", backgImage);
				//imshow("velocityImage", VelocityImage);
				VelocityImage = Mat(rows,cols,CV_8UC1,cv::Scalar(0));
			}
		}
		//draw anomaly
		if (GM->getAnomaly()){
			if(verbose){
				cout<<"Anomaly at:"<<nFrameCount<<endl;
			}
			rectangle(frame,Point(0,0),Point(cols-1,rows-1),Scalar(0,0,255),3);
		}
		else
			rectangle(frame,Point(0,0),Point(cols-1,rows-1),Scalar(0,255,0),3);

		imshow("GMM result",frame);

		swap(points[1], points[0]); //Moved above
		gray.copyTo(prevGray);
		waitKey(10);

		frame.release(); gray.release();
	}
    if(verbose){
    	sort(particlesMoved.begin(), particlesMoved.end());
		cout<<"List of max particles moved: ";
		for(int i=10; i >0; i--){
			cout<<particlesMoved[particlesMoved.size()-i]<<" ";
		}
		cout <<"\n Min particles moved = "<<*min_element(particlesMoved.begin(), particlesMoved.end());
		cout <<"\n Max particles moved = "<<*max_element(particlesMoved.begin(), particlesMoved.end());
		cout << "\n Mean particles moved  = "<< accumulate( particlesMoved.begin(), particlesMoved.end(), 0.0)/particlesMoved.size()<<endl<<endl;
    }
    prevGray.release(); backgImage.release(); VelocityImage.release();

    delete GM;
}
