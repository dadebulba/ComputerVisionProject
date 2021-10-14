#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include "../mmLib.h"
#include "./module3.h"

using namespace cv;
using namespace std;

bool hasEnding (string const &fullString, string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

int module3(module3Config &config, bool verbose){
	string nameVideoFile = config.videoPath;
	string nameVideoToSave = config.outputVideoPath;
	
	//capture a video
	VideoCapture cap;
	cap.open(nameVideoFile);

	//check if there is a video
	if(!cap.isOpened()){
		cout << "ERROR: Stream not found" << endl;
		return 0;
	}

	double scaleX = 1.0;
	double scaleY = 1.0;
	int dimX = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int dimY = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	int fps     = cap.get(CV_CAP_PROP_FPS);
	int totalFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);

	if (verbose){
		cout<<"\t- Frame width: "<<dimX<<"\n\t- Frame height: "<<dimY<<"\n\t- FPS: "<<fps<<"\n\t- N° frames: "<<totalFrames<<endl<<endl;
	}

	int frameW  = dimX;
	int frameH  = dimY;
	VideoWriter video;
	if (hasEnding(nameVideoToSave, "avi"))
		video = VideoWriter(nameVideoToSave, CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(frameW/config.widthScale, frameH/config.heightScale), true);
	else if (hasEnding(nameVideoToSave, "mp4"))
		video = VideoWriter(nameVideoToSave, CV_FOURCC('M', 'P', '4', 'V'), fps, cvSize(frameW/config.widthScale, frameH/config.heightScale), true);

	//create objects
	mmLib::mmAnomaly::mmParticleAccumulationSettings paSet;
	paSet.trackPeriodParam = 1;
	paSet.minMovementParam = 1;
	paSet.gridGranularityParam = 6;
	mmLib::mmAnomaly::mmParticleAccumulation* pAccumul = new mmLib::mmAnomaly::mmParticleAccumulation(paSet);

	mmLib::mmAnomaly::mmParticleEnergySettings peSet;
	vector<Rect> rectVect = config.rectangles;
	peSet.surveilanceRectParam = config.rectangles;
	peSet.sigmaParam = 0.05;
	peSet.filterSizeParam = Size(31,31);
	peSet.profileFrameStartParam = 0;
	peSet.profileLengthParam = 120;
	peSet.profilePeriodParam = 120;
	peSet.valuePeriodParam = 2;
	peSet.trackingPeriodParam = paSet.trackPeriodParam+1;
	peSet.threshHighParam = 20;
	peSet.threshLowParam = 20;
	mmLib::mmAnomaly::mmParticleEnergy* pEnergy = new mmLib::mmAnomaly::mmParticleEnergy(peSet);

	Mat immagine, frame, reference, iteration, iAnomaly, particleAdvection, iOut;
	bool firstFrame = true;
	int countIt = 0;
	int countFrame = 0;
	vector<float> energyProfile, energyValue;
	double maxRef, maxIt;
	vector<Point2f> vect;
	vector<mmLib::mmAnomaly::ANOMALY_TYPE> anomalyVect;

	while(true){
		cap >> immagine;
		if(immagine.empty()){
			break;
		}

		resize(immagine,frame,Size(dimX/config.widthScale,dimY/config.heightScale),scaleX,scaleY);

		immagine.release();
		frame.copyTo(iAnomaly);

		//initailize modelue
		if(firstFrame){
			reference = Mat::zeros(frame.rows,frame.cols,CV_32FC3);
			iteration = Mat::zeros(frame.rows,frame.cols,CV_32FC3);
			firstFrame = false;
		}

		//initailize module
		pEnergy->setUpModule(frame);
		pAccumul->setUpModule(frame);
		//advect partcile
		pAccumul->computeParticleAdvection(frame);
		//get final particle
		vect = pAccumul->getFinalParticle();

		if(pEnergy->getRec()){
			//record phase
			if ((countFrame+1)%(paSet.trackPeriodParam+1)==0 && countFrame > peSet.profileFrameStartParam){
				pEnergy->processFrame(frame,vect);

				if (countFrame != 0 && (countFrame+1)%(peSet.profilePeriodParam)==0){
					energyProfile = pEnergy->getProfileEnVect(countIt);
					energyProfile.clear();

					reference = pEnergy->getMatProfile();
					minMaxLoc(reference,0,&maxRef,0,0);
					reference = reference/maxRef;
					reference = reference*255;
					countIt++;
				}
			}
			if(countFrame>(peSet.profileLengthParam+peSet.profileFrameStartParam) || !pEnergy->getRec()){
				if (verbose){
					cout << "\tStop accumulation phase (frame = " <<  countFrame << ")" <<endl<<endl;
				}
				pEnergy->clearVariables();
				pAccumul->clearVariables();
//				pAccumul = new mmLib::mmAnomaly::mmParticleAccumulation(paSet);
				cap.open(nameVideoFile);
				countFrame = -1;
				countIt = 0;
			}
		}
		else{
			//evaluation phase
			if ((countFrame+1)%(paSet.trackPeriodParam+1)==0){
				pEnergy->processFrame(frame,vect);

				if (countFrame != 0 && (countFrame+1)%(peSet.valuePeriodParam)==0){
					energyValue = pEnergy->getValueEnVect();
					anomalyVect = pEnergy->getAnomalyType();
					
					energyValue.clear();
					iteration = pEnergy->getMatValue();
					minMaxLoc(iteration,0,&maxIt,0,0);
					iteration = iteration/maxIt;
					iteration = iteration*255;
					countIt++;
				}
			}
			//draw anomaly
			if (anomalyVect.size() == 0){
				rectangle(iAnomaly,rectVect[0],Scalar(125,125,125),2);
			}
			else{
				for (int l=0; l<rectVect.size();l++){
					if (anomalyVect[l] == mmLib::mmAnomaly::STANDARD)
						rectangle(iAnomaly,rectVect[l],Scalar(0,255,0),2);
					else if (anomalyVect[l] == mmLib::mmAnomaly::HIGHANOMALY)
						rectangle(iAnomaly,rectVect[l],Scalar(0,0,255),2);
					else if (anomalyVect[l] == mmLib::mmAnomaly::LOWANOMALY)
						rectangle(iAnomaly,rectVect[l],Scalar(255,0,0),2);
				}
			}
		}

		imshow("Anomaly", iAnomaly);
		waitKey(1);

		if (video.isOpened()){
			video << iAnomaly;
		}

		countFrame++;
		frame.release(); iAnomaly.release();
	}

	reference.release(); iteration.release();	energyProfile.clear(); energyValue.clear(); vect.clear(); anomalyVect.clear(); rectVect.clear();
	delete pAccumul;
	delete pEnergy;
}

