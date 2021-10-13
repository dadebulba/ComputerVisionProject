#include "../mmLib.h"

using namespace cv;
using namespace std;

namespace mmLib
{
	namespace mmAnomaly
	{
		mmParticleAccumulationSettings::mmParticleAccumulationSettings()
		{
			//	sigmaParam = 0.05;
			//	threshParam = 15;
			minMovementParam = 20;
			gridGranularityParam = 8;
			trackPeriodParam = 50;
		}

		mmParticleAccumulation::mmParticleAccumulation()
		{
			//	sigma = 0.05; //energy function's variance value
			//	thresh = 15; //threshold for anomaly detection
			vector<Point> stuff;
			stuff.push_back(Point(0, 0));
			stuff.push_back(Point(0, 0));
			stuff.push_back(Point(0, 0));
			stuff.push_back(Point(0, 0));
			surveilanceArea.push_back(stuff); //surveilance area
			stuff.clear();
			minMovement = 20;	 //min movement for a particle to be considered active
			gridGranularity = 8; //the system select one particle each gridGranularity pixel
			trackPeriod = 50;	 //temporal window size for energy accumulation
			totalFrameProcessed = 0;
			countFrame = 0;
			firstFrame = true;
		}

		mmParticleAccumulation::mmParticleAccumulation(mmParticleAccumulationSettings &paS)
		{
			surveilanceArea = paS.surveilanceAreaParam; //surveilenace area vertex
														//	sigma = paS.sigmaParam;
														//	thresh = paS.threshParam;
			minMovement = paS.minMovementParam;			//min movement for a particle to be considered as active
			gridGranularity = paS.gridGranularityParam; //grid dimensionality
			trackPeriod = paS.trackPeriodParam;			//tracking period
			totalFrameProcessed = 0;
			countFrame = 0;
			firstFrame = true;
			paS.surveilanceAreaParam.clear();
		}

		mmParticleAccumulation::~mmParticleAccumulation()
		{
			surveilanceArea.clear();
			actualPos.clear();
			finalParticle.clear();
			startParticle.clear();
			pathLenght.clear();
			pathLenghtX.clear();
			pathLenghtXFin.clear();
			pathLenghtY.clear();
			pathLenghtYFin.clear();
			firstFrameParticle.clear();
			prevFrame.release();
			particleAdvection.release();
			finalParticleMat.release();
		}

		void mmParticleAccumulation::setUpModule(Mat frame)
		{
			if (firstFrame)
			{
				firstFrame = false;

				if (surveilanceArea.size() == 0)
				{
					vector<Point> stuff;
					stuff.push_back(Point(0, 0));
					stuff.push_back(Point(0, frame.rows - 1));
					stuff.push_back(Point(frame.cols - 1, frame.rows - 1));
					stuff.push_back(Point(frame.cols - 1, 0));
					surveilanceArea.push_back(stuff);
					stuff.clear();
				}
			}
			frame.release();
		}

		//find particle inside the surveilance area
		void mmParticleAccumulation::selectParticle(Mat frame)
		{
			actualPos.clear();

			for (int i = 0; i < frame.cols; i = i + gridGranularity)
			{
				for (int j = 0; j < frame.rows; j = j + gridGranularity)
				{
					for (int k = 0; k < surveilanceArea.size(); k++)
					{
						double dist = pointPolygonTest(surveilanceArea[k], Point(i, j), true);
						if (dist >= 0)
						{
							actualPos.push_back(Point(i, j));
						}
					}
				}
			}
			frame.release();
		}

		void mmParticleAccumulation::computeParticleAdvection(Mat frame)
		{
			double dist;
			double Energy = -1;
			finalParticle.clear();
			Mat test;

			if (countFrame == 0)
			{
				//initialization
				selectParticle(frame);
				pathLenght.clear();
				pathLenghtX.clear();
				pathLenghtY.clear();
				pathLenghtFin.clear();
				pathLenghtXFin.clear();
				pathLenghtYFin.clear();
				firstFrameParticle.clear();
				for (int i = 0; i < actualPos.size(); i++)
				{
					pathLenght.push_back(0);
					pathLenghtX.push_back(0);
					pathLenghtY.push_back(0);
				}
				particleAdvection = Mat(frame.rows, frame.cols, CV_8UC3, cv::Scalar(0));
				finalParticleMat = Mat(frame.rows, frame.cols, CV_8UC3, cv::Scalar(0));
				test = Mat(frame.rows, frame.cols, CV_8UC3, cv::Scalar(0));
			}
			else
			{
				vector<Point2f> finalPos, movingParticle;
				vector<uchar> status;
				vector<float> err;
				//TODO::da inserire nelle costanti del modulo
				TermCriteria termcrit = TermCriteria(CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, 4, 0.04); //# of iterations | Epsilon (for Lukas-Kanade)
				Size winSize = Size(20, 20);													   //optical flow parameter
				//Optical flow
				calcOpticalFlowPyrLK(prevFrame, frame, actualPos, finalPos, status, err, winSize, 0, termcrit);
				for (int i = 0; i < finalPos.size(); i++)
				{ //print the advected particles
					line(particleAdvection, actualPos[i], finalPos[i], Scalar(0, 255, 0), 2, 8, 0);
				}
				//path lenght update
				for (int i = 0; i < finalPos.size(); i++)
				{
					for (int k = 0; k < surveilanceArea.size(); k++)
					{
						dist = pointPolygonTest(surveilanceArea[k], finalPos[i], true);
						if (dist >= 0)
						{
							pathLenght[i] = pathLenght[i] + sqrt(pow((finalPos[i].x - actualPos[i].x), 2) + pow((finalPos[i].y - actualPos[i].y), 2));
							pathLenghtX[i] = pathLenghtX[i] + abs(finalPos[i].x - actualPos[i].x);
							pathLenghtY[i] = pathLenghtY[i] + abs(finalPos[i].y - actualPos[i].y);
						}
						else
						{
							pathLenght[i] = 0;
							pathLenghtX[i] = 0;
							pathLenghtY[i] = 0;
						}
					}
				}
				//particleStart
				if (countFrame == 1)
				{
					startParticle = actualPos;
				}
				//analysis frame
				if (countFrame == trackPeriod)
				{
					for (int i = 0; i < actualPos.size(); i++)
					{
						for (int k = 0; k < surveilanceArea.size(); k++)
						{
							dist = pointPolygonTest(surveilanceArea[k], finalPos[i], true);
							if (pathLenght[i] > minMovement && dist >= 0)
							{
								movingParticle.push_back(finalPos[i]);
								firstFrameParticle.push_back(startParticle[i]);
								pathLenghtFin.push_back(pathLenght[i]);
								pathLenghtXFin.push_back(pathLenghtX[i]);
								pathLenghtYFin.push_back(pathLenghtY[i]);
							}
						}
					}

					finalParticle = movingParticle;
					//			for(int i=0; i<finalParticle.size(); i++){
					//				circle(finalParticleMat,finalParticle[i],1,Scalar(0,255,0),1);
					//				circle(finalParticleMat,firstFrameParticle[i],1,Scalar(0,0,255),1);
					//			}
					//			imshow("End",finalParticleMat);
					startParticle.clear();
					countFrame = -1;
				}
				swap(actualPos, finalPos);
				finalPos.clear();
				movingParticle.clear();
				status.clear();
				err.clear();
			}
			frame.copyTo(prevFrame);
			frame.release();

			countFrame++;
			totalFrameProcessed++;
		}

		void mmParticleAccumulation::clearVariables()
		{
			totalFrameProcessed = 0;
			countFrame = 0;
		}

		vector<Point2f> mmParticleAccumulation::getFinalParticle()
		{
			return finalParticle;
		}

		vector<Point2f> mmParticleAccumulation::getFirstFrameParticle()
		{
			return firstFrameParticle;
		}

		vector<float> mmParticleAccumulation::getPathLenght()
		{
			return pathLenghtFin;
		}

		vector<float> mmParticleAccumulation::getPathLenghtX()
		{
			return pathLenghtXFin;
		}

		vector<float> mmParticleAccumulation::getPathLenghtY()
		{
			return pathLenghtYFin;
		}

		Mat mmParticleAccumulation::getParticleAdvection()
		{
			return particleAdvection;
		}

		Mat mmParticleAccumulation::getFinalParticleMat()
		{
			return finalParticleMat;
		}
	}
}

/*
#include "../mmLib.h"

using namespace cv;
using namespace std;

namespace mmLib{
namespace mmAnomaly{
mmParticleAccumulationSettings::mmParticleAccumulationSettings(){
//	sigmaParam = 0.05;
//	threshParam = 15;
	minMovementParam = 20;
	gridGranularityParam = 8;
	trackPeriodParam = 50;
}

mmParticleAccumulation::mmParticleAccumulation(){
//	sigma = 0.05; //energy function's variance value
//	thresh = 15; //threshold for anomaly detection
	vector<Point> stuff;
	stuff.push_back(Point(0,0)); stuff.push_back(Point(0,0)); stuff.push_back(Point(0,0)); stuff.push_back(Point(0,0));
	surveilanceArea.push_back(stuff); //surveilance area
	stuff.clear();
	minMovement = 20; //min movement for a particle to be considered active
	gridGranularity = 8; //the system select one particle each gridGranularity pixel
	trackPeriod = 50; //temporal window size for energy accumulation
	totalFrameProcessed = 0;
	countFrame = 0;
}

mmParticleAccumulation::mmParticleAccumulation(mmParticleAccumulationSettings &paS){
	surveilanceArea = paS.surveilanceAreaParam;
//	sigma = paS.sigmaParam;
//	thresh = paS.threshParam;
	minMovement = paS.minMovementParam;
	gridGranularity = paS.gridGranularityParam;
	trackPeriod = paS.trackPeriodParam;
	totalFrameProcessed = 0;
	countFrame = 0;
}

mmParticleAccumulation::~mmParticleAccumulation(){
	surveilanceArea.clear();
	actualPos.clear();
	finalParticle.clear();
	pathLenght.clear();
	finalVelocity.clear();
	finalVelocityX.clear();
	finalVelocityY.clear();
	finalAngle.clear();
	zeroPosParticle.clear();
	prevFrame.release();
	particleAdvection.release();
	finalParticleMat.release();
}

//find particle inside the surveilance area
void mmParticleAccumulation::selectParticle(Mat frame){
	actualPos.clear();

	for (int i=0; i<frame.cols; i=i+gridGranularity){
		for (int j=0; j<frame.rows; j=j+gridGranularity){
			for (int k=0; k<surveilanceArea.size(); k++){
				double dist = pointPolygonTest(surveilanceArea[k],Point(i,j),true);
				if(dist>=0){
					actualPos.push_back(Point(i,j));
				}
			}
		}
	}
}

void mmParticleAccumulation::computeParticleAdvection(Mat frame){
	double dist;
	double Energy = -1;
	finalParticle.clear();
	finalVelocity.clear();
	finalVelocityX.clear();
	finalVelocityY.clear();
	finalAngle.clear();

	if (countFrame == 0){
		//initialization
		selectParticle(frame);
		pathLenght.clear();
		for (int i=0; i<actualPos.size();i++){
			pathLenght.push_back(0);
		}
		zeroPosParticle.clear();
		zeroPosParticle = actualPos;
		particleAdvection = Mat(frame.rows,frame.cols,CV_8UC3,cv::Scalar(0));
		finalParticleMat = Mat(frame.rows,frame.cols,CV_8UC3,cv::Scalar(0));
//		time (&startT);
		startT = clock();
	}
	else{
		vector<Point2f> finalPos, movingParticle;
		vector<uchar> status;
		vector<float> err;
		//TODO::da inserire nelle costanti del modulo
		TermCriteria termcrit = TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,4,0.04); //# of iterations | Epsilon (for Lukas-Kanade)
		Size winSize = Size(20,20);//optical flow parameter
		//Optical flow
		calcOpticalFlowPyrLK(prevFrame, frame, actualPos, finalPos, status, err, winSize, 0, termcrit);
		for (int i=0; i<finalPos.size(); i++) { //print the advected particles
			line(particleAdvection, actualPos[i],finalPos[i], Scalar(0,255,0),2,8,0);
		}
		//path lenght update
		for (int i=0; i<finalPos.size();i++){
			for (int k=0; k<surveilanceArea.size(); k++){
				dist = pointPolygonTest(surveilanceArea[k],finalPos[i],true);
				if (dist>=0){
					pathLenght[i] = pathLenght[i] + sqrt(pow((finalPos[i].x-actualPos[i].x),2)+pow((finalPos[i].y-actualPos[i].y),2));
				}
				else{
					pathLenght[i] = 0;
				}
			}
		}
		//analysis frame
		if (countFrame==trackPeriod){
//			time (&endT);
			endT = clock();
			for (int i=0; i<actualPos.size();i++){
				for (int k=0; k<surveilanceArea.size(); k++){
					dist = pointPolygonTest(surveilanceArea[k],finalPos[i],true);
					if (pathLenght[i]>minMovement && dist>=0){
						movingParticle.push_back(finalPos[i]);
//						cout << "X= " << zeroPosParticle[i].x << endl;
//						cout << "Y= " << zeroPosParticle[i].y << endl;
						finalVelocity.push_back(computeVelocity(zeroPosParticle[i], finalPos[i]));
						finalVelocityX.push_back(computeVelocity(Point2f(zeroPosParticle[i].x,0), Point2f(finalPos[i].x,0)));
						finalVelocityY.push_back(computeVelocity(Point2f(0,zeroPosParticle[i].y), Point2f(0,finalPos[i].y)));
						finalAngle.push_back(computeAngle(zeroPosParticle[i], finalPos[i]));
					}
				}
			}

			finalParticle = movingParticle;
//			cout << "S:: " << finalParticle.size() << endl;
			for(int i=0; i<finalParticle.size(); i++){
				circle(finalParticleMat,finalParticle[i],1,Scalar(0,255,0),1);
			}
			countFrame=-1;
		}
		swap(actualPos,finalPos);
		finalPos.clear(); movingParticle.clear(); status.clear(); err.clear();
	}
	frame.copyTo(prevFrame);
	frame.release();

	countFrame++;
	totalFrameProcessed++;
}

void mmParticleAccumulation::clearVariables(){
	totalFrameProcessed = 0;
	countFrame = 0;
}

vector<Point2f> mmParticleAccumulation::getFinalParticle(){
	return finalParticle;
}

vector<Point2f> mmParticleAccumulation::getZeroPosParticle(){
	return zeroPosParticle;
}

Mat mmParticleAccumulation::getParticleAdvection(){
	return particleAdvection;
}


Mat mmParticleAccumulation::getFinalParticleMat(){
	return finalParticleMat;
}

double mmParticleAccumulation::computeVelocity(Point2f start, Point2f end){
	double s = sqrt(pow(end.x-start.x,2)+pow(end.y-start.y,2));
	float sec = ((float)endT-(float)startT)/1000;
	double velocity = s/((endT-startT)/1000);
	return velocity;
}

double mmParticleAccumulation::computeAngle(Point2f start, Point2f end){
    double angle = 0;
    double angleRad = 0;

    if(end.y != start.y){
    	angleRad = atan((end.x-start.x)/(end.y-start.y));
    	angle = (180/pi)*angleRad;
    	if(angle<0)
    		angle = 360+angle;
    	if(start.y>end.y)
    		angle = angle+180;
    }
    else{
        if (start.x >= end.x)
            angle = 90;
        else
            angle = 270;
    }
    return angle;
}

vector<double> mmParticleAccumulation::getFinalVelocity(){
	return finalVelocity;
}

vector<double> mmParticleAccumulation::getFinalVelocityX(){
	return finalVelocityX;
}

vector<double> mmParticleAccumulation::getFinalVelocityY(){
	return finalVelocityY;
}

vector<double> mmParticleAccumulation::getFinalAngle(){
	return finalAngle;
}

}
}
*/
