#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <boost/algorithm/string.hpp>
#include "opencv/cxcore.h"
#include "./modules/module3.h"

#define CONFIG_ROW_LEN 8
#define VERBOSE true

using namespace std;
using namespace cv;

void stringSplit(string s, string delim, vector<string> &out){
	size_t pos_start = 0, pos_end, delim_len = delim.length();
	string token;

	while ((pos_end = s.find (delim, pos_start)) != string::npos) {
		token = s.substr (pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		out.push_back (token);
	}

	out.push_back (s.substr (pos_start));
}

void extractRectangles(string s, vector<Rect> &out){
	vector<string> elem;
	vector<int> rect;

	boost::split(elem, s, [](char c){return c==' ' || c=='[' || c=='(' || c==')' || c==']' || c==';';});
	for (int i=0; i<elem.size(); i++){
		if (elem[i] != ""){
			rect.push_back(stoi(elem[i]));

			if (rect.size() == 4){
				out.push_back(Rect(rect[0], rect[1], rect[2], rect[3]));
				rect.clear();
			}
		}
	}
}

string computeOutputVideoPath(string inputPath){
	vector<string> outputVideoPathSplit;
	string out = "";
	bool prefixDone = false;

	stringSplit(inputPath, ".", outputVideoPathSplit);
	for (int i=0; i<outputVideoPathSplit.size(); i++) {
		if (outputVideoPathSplit[i] != ""){
			if (!prefixDone){
				prefixDone = true;
				out = out + outputVideoPathSplit[i] + "_OUTPUT.";
			}
			else {
				out = out + outputVideoPathSplit[i];
				break;
			}
		}
		else
			out = out + ".";
	}

	return out;
}

int main(){
	// Open config file
	ifstream config("../config/module3.csv");
	string line;

	// Read headers
	getline(config, line);

	while (getline(config, line)){
		vector<string> split;
		stringSplit(line, ",", split);

		if (split.size() != CONFIG_ROW_LEN){
			cerr<<"Invalid config file - "<<split.size()<<" arguments instead of "<<CONFIG_ROW_LEN<<endl;
			return 1;
		}

		// Parsing values
		module3Config configObj;
		vector<Rect> rectangles;

		configObj.videoPath = split[0];
		configObj.outputVideoPath = computeOutputVideoPath(split[0]);
		configObj.widthScale = stoi(split[1]);
		configObj.heightScale = stoi(split[2]);
		configObj.accumulationPhase_frameStart = stoi(split[3]);
		configObj.accumulationPhase_frameEnd = stoi(split[4]);
		configObj.thresholdHigh = stoi(split[5]);
		configObj.thresholdLow = stoi(split[6]);

		extractRectangles(split[7], rectangles);
		configObj.rectangles = rectangles;

		if (VERBOSE){
			cout<<"Parsing "<<configObj.videoPath<<" with parameters:\n\t- Output file: "<<configObj.outputVideoPath<<"\n\t- Width scale: "<<configObj.widthScale<<"\n\t- Height scale: "<<configObj.heightScale<<endl;
			cout<<"\t- ACC frame start: "<<configObj.accumulationPhase_frameStart<<"\n\t- ACC frame end: "<<configObj.accumulationPhase_frameEnd<<endl;
			cout<<"\t- Threshold high: "<<configObj.thresholdHigh<<"\n\t- Threshold low: "<<configObj.thresholdLow<<"\n\t- Rectangles: "<<split[7]<<endl<<endl;
		}

		//Parse the video
		module3(configObj, VERBOSE);

	}

}
















//TODO: MODULO 1
/*
#include "stdlib.h"
#include "stdio.h"
#include <math.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"
#include "./mmLib.h"

using namespace cv;
using namespace std;

int main(){
	int countFrame = 0;
	int numPoly = 1;
	bool isGreen = true;
	Mat foreground, frame, immResize, im, immagine, isn, iOut, iVideo;
	vector<mmBlob> blobList, activeTracks;
	vector<blobListModule> *bLIn;
	vector<blobListModule> *bLOut;

	//windows
	namedWindow("Video", 1);
	cvMoveWindow("Video",200,0);

	//capture a video
	VideoCapture cap;
	string nameVideo = "Porta1";
	cap.open("/home/matteo/workspace/opencv/VIDEO/"+nameVideo+".mov");

	//check if there is a video
	if(!cap.isOpened()){
		cout << "ERROR: Stream not found" << endl;
		return 0;
	}

	//create video vriter
	double fps = cap.get(CV_CAP_PROP_FPS);
	double cols = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double rows = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	double fourC = cap.get(CV_CAP_PROP_FOURCC);
	double isColor = 1;
	VideoWriter video(nameVideo+"Processed.avi" , CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(cols, rows), isColor );

	mmLib::mmModules::appSettings appSet;

	mmLib::mmModules::mmBlobDetectorSettings blobSet;
	blobSet.increasedBRCornerParam = 10;
	blobSet.increasedTLCornerParam = 10;
	blobSet.minDetHeigthParam = 7;
	blobSet.minDetWidthParam = 7;
	blobSet.splitThreshParam = 60;
	blobSet.borderThreshParam = 30;

	mmLib::mmModules::codeBookParameters cbSet;
	cbSet.cbMemory = 300;
	cbSet.alphaValue = 0.7;
	cbSet.betaValue = 1.2;

	mmLib::mmModules::harrisDetectorSettings hSet;
	hSet.maskParam = imread("hug"+nameVideo+".png");
    hSet.threshParam = 220;
	hSet.maxFrameParam = 70;

	//create entranceExitAreaParam zones
	Mat mask = imread("mask"+nameVideo+".png");
	Mat prova = Mat::zeros(mask.rows,mask.cols,CV_8UC1);
	int numPoints = 5;
	vector<Point2f> vert(numPoints);
	vert[0] = Point(75,50);
	vert[1] = Point(215,50);
	vert[2] = Point(215,150);
	vert[3] = Point(75,150);
	vert[4] = Point(75,50);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	for( int j = 0; j < numPoints-1; j++ ){
		line( prova, vert[j],  vert[(j+1)], Scalar( 255,255,255 ), 3, 8 );
	}
	findContours(prova, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);

	vector<Rect> nnVisibleArea;
	nnVisibleArea.push_back(Rect(vert[0].x,vert[0].y,abs(vert[1].x-vert[0].x),abs(vert[2].y-vert[1].y)));
	blobSet.entranceExitAreaParam = nnVisibleArea;

	Rect rectImm = Rect(0,0,mask.cols,mask.rows);

	//initialize objects
	mmLib::mmModules::mmCodeBookBgSubstraction* bgSub = new mmLib::mmModules::mmCodeBookBgSubstraction(cbSet);
	mmLib::mmModules::mmBlobDetector* tracker = new mmLib::mmModules::mmBlobDetector(blobSet,appSet);
	mmLib::mmModules::blobStructure* bStruct = new mmLib::mmModules::blobStructure();
	mmLib::mmModules::harrisDetector* hDet = new mmLib::mmModules::harrisDetector(hSet);

	prova.release(); vert.clear(); nnVisibleArea.clear(); hierarchy.clear();
	while(true){
//		cout << "FRAME= " << countFrame << endl;
		bStruct->InitBlobStructure(numPoly);
		bStruct->setPolygons(contours,numPoly);

		hDet->InitHarris();

		cap >> frame;
		resize(frame, immResize, Size(0,0), 0.5, 0.5);
		if(frame.empty()){
			break;
		}
		frame.release();

		Rect r = Rect(10,10,300,220);
		im = Mat(immResize,r);
		im.copyTo(immagine);
		immagine.copyTo(isn,mask);

		//Get foreground
		foreground = bgSub->processFrame(isn);

		//Obtain the tracks
		blobList = tracker->BlobDetectionAppModel(isn, foreground, activeTracks);
		activeTracks.clear();
		activeTracks = blobList;

//		//Update delle liste dei blob e del Harris detector
		bStruct->UpdateLists(blobList);
		hDet->UpdateHarris(immagine);

		//ottengo le due liste di blob
		bLIn = bStruct->getBIn();
		bLOut = bStruct->getBOut();

		//draw blobs
		iOut = mmDrawBlobsAndPoints(immagine, foreground, blobList);

		im.copyTo(iVideo);


		//draw allarms
		if (!hDet->isHarrisChanged()){
			rectangle(iOut,rectImm,Scalar(0,255,0),2);
			rectangle(iOut,rectImm,Scalar(0,255,0),2);
			isGreen = true;
		}
		else{
			if (bLIn[0].size() == 0 && isGreen){
				rectangle(iOut,rectImm,Scalar(0,255,255),2);
				rectangle(iOut,rectImm,Scalar(0,255,0),2);
			}
			else{
				rectangle(iOut,rectImm,Scalar(0,0,255),2);
				rectangle(iOut,rectImm,Scalar(0,255,0),2);
				isGreen = false;
			}
		}

		rectangle(iOut,nnVisibleArea[0],Scalar(125,125,0),1);
		imshow("Video", iOut);

		iOut.copyTo(iVideo);
		iOut.release();

//		save video
		if (video.isOpened()){
			video << iVideo;
		}

		iVideo.release();
		immResize.release(); foreground.release(); iOut.release(); im.release(); immagine.release(); isn.release();
		blobList.clear();
		countFrame++;
		waitKey(10);
	}
	contours.clear(); mask.release();
	delete bgSub;
	delete tracker;
	delete hDet;
	delete bStruct;
}
*/

//TODO:MODULO 2
/*#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <math.h>
#include "mmLib/mmLib.h"

using namespace cv;
using namespace std;

int main(){
    Mat prevGray, frame, gray, backgImage, VelocityImage;
	cv::VideoCapture capture("../VIDEO/Anomaly1.avi");

	int isColor = 1;
	int fps     = 25;
	int frameW  = 640;
	int frameH  = 480;
	VideoWriter video( "UMN.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(frameW, frameH), isColor );

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

    TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
    Size winSize(10,10);
    vector<Point2f> points[2];
    vector<Point2f> tempoints;

    Point2f Singlept;
    //initialize variable
    mmLib::mmAnomaly::mmGaussianModelSettings gmSets;
    gmSets.heightParam = 25;
    gmSets.widthParam = 35;
    gmSets.threshParam = 6;
    gmSets.numAnomalyParam = 3;
    gmSets.numNormalParam = 1;

	FILE* pFile = fopen("UMN.txt", "w");
	fprintf(pFile, "UMN\n");
	fprintf(pFile,"thresh=%d\n",gmSets.threshParam);
	fprintf(pFile,"num Anomaly=%d\n",gmSets.numAnomalyParam);
	fprintf(pFile,"num Normal=%d\n",gmSets.numNormalParam);
	fprintf(pFile,"win size::h=%d, w=%d\n",gmSets.heightParam,gmSets.widthParam);
	fprintf(pFile,"\n");
	fclose(pFile);

	//create object
    mmLib::mmAnomaly::mmGaussianModel* GM = new mmLib::mmAnomaly::mmGaussianModel(gmSets);

	backgImage = Mat(rows,cols,CV_8UC3,CV_RGB(0,0,0));
	VelocityImage = Mat(rows,cols,CV_8UC1,cv::Scalar(0));
	namedWindow("GMM result", 1);
	cvMoveWindow("GMM result",0,0);
	namedWindow("background", 1);
	cvMoveWindow("background",330,0);
	namedWindow("velocityImage", 1);
	cvMoveWindow("velocityImage",660,0);

    while (true){
		++nFrameCount;
		if (!capture.read(frame))
			break;

		cvtColor(frame,gray,CV_RGB2GRAY);

		if(stopInit == true){
			needToInit = true;
			stopInit = false;
		}

		//initialize module
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
//			cout << "FRAME: " << nFrameCount << endl;
			vector<uchar> status;
			vector<float> err;
			calcOpticalFlowPyrLK(prevGray, gray, points[0], points[1], status, err, winSize, 3, termcrit, 0);
			VelocityImage = GM->MagnitudeFunt(frame, VelocityImage, points[1], tempoints, status, nFrameInit, needToInit);

			if(nFrameInit == 1 && needToInit){
				GM->updateBackgroundModel(frame,VelocityImage,backgImage);
				GM->findAnomaly(frame);

				imshow("background", backgImage);
				imshow("velocityImage", VelocityImage);
				VelocityImage = Mat(rows,cols,CV_8UC1,cv::Scalar(0));
			}
		}
		//draw anomaly
		if (GM->getAnomaly())
			rectangle(frame,Point(0,0),Point(cols-1,rows-1),Scalar(0,0,255),3);
		else
			rectangle(frame,Point(0,0),Point(cols-1,rows-1),Scalar(0,255,0),3);

		imshow("GMM result",frame);

		if (video.isOpened()){
			video << VelocityImage;
		}

        swap(points[1], points[0]);
		gray.copyTo(prevGray);
		waitKey(10);

		frame.release(); gray.release();
	}
    prevGray.release(); backgImage.release(); VelocityImage.release();

    delete GM;
}*/

//TODO:MODULO 3
/*
#include "stdlib.h"
#include "stdio.h"
#include <math.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"
#include "mmLib.h"

using namespace cv;
using namespace std;

//CvVideoWriter* video;

int main(){
	string nameVideo = "cam3";
	string nameVideoFile = "../video/"+nameVideo+".mp4";
	string nameVideoToSave = nameVideo+"4Vid.avi";
	//string nameVideoText = nameVideo+".txt";
	//string nameVideoImg = nameVideo+".png";

	//capture a video
	VideoCapture cap;
	cap.open(nameVideoFile);

	//check if there is a video
	if(!cap.isOpened()){
		cout << "ERROR: Stream not found" << endl;
		return 0;
	}

	double scaleX = 0.99;
	double scaleY = 0.99;
	int dimX = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	int dimY = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	int fps     = cap.get(CV_CAP_PROP_FPS);
	int totalFrames = cap.get(CV_CAP_PROP_FRAME_COUNT);
	cout<<dimX<<"-"<<dimY<<"-"<<fps<<"-"<<totalFrames<<endl;

	int frameW  = dimX;
	int frameH  = dimY;
	VideoWriter video(nameVideoToSave, CV_FOURCC('D', 'I', 'V', 'X'), fps, cvSize(frameW/4, frameH/4), true);

	//surveillance area vertex

	vector<vector<Point> > contours;
	vector<Point> stuff;
	stuff.push_back(Point(70,185));
	stuff.push_back(Point(190,185));
	stuff.push_back(Point(190,110));
	stuff.push_back(Point(460,110));
	stuff.push_back(Point(460,185));
	stuff.push_back(Point(460,260));
	stuff.push_back(Point(70,260));
	stuff.push_back(Point(70,185));
	contours.push_back(stuff);


	//create objects
	mmLib::mmAnomaly::mmParticleAccumulationSettings paSet;
	paSet.trackPeriodParam = 1;
	paSet.minMovementParam = 1;
	paSet.gridGranularityParam = 6;
	mmLib::mmAnomaly::mmParticleAccumulation* pAccumul = new mmLib::mmAnomaly::mmParticleAccumulation(paSet);

	mmLib::mmAnomaly::mmParticleEnergySettings peSet;
	vector<Rect> rectVect;
	rectVect.push_back(Rect(40,35,150,150));
	//rectVect.push_back(Rect(70,185,120,75));
	//rectVect.push_back(Rect(190,185,120,75));
	//rectVect.push_back(Rect(190,110,120,75));
	peSet.surveilanceRectParam = rectVect;
	peSet.sigmaParam = 0.1;
	peSet.filterSizeParam = Size(31,31);
	peSet.profileFrameStartParam = 0;
	peSet.profileLengthParam = 120;
	peSet.profilePeriodParam = 120;
	peSet.valuePeriodParam = 2;
	peSet.trackingPeriodParam = paSet.trackPeriodParam+1;
	peSet.threshHighParam = 20;
	peSet.threshLowParam = 20;
	mmLib::mmAnomaly::mmParticleEnergy* pEnergy = new mmLib::mmAnomaly::mmParticleEnergy(peSet);

	FILE* pFile = fopen("Uni.txt", "w");
	fprintf(pFile,"UNI\n");

	fprintf(pFile,"Surveilance Area: ");
	for (int i=0;i<stuff.size(); i++){
		fprintf(pFile,"(%d,%d), ",stuff[i].x,stuff[i].y);
	}

	fprintf(pFile,"\n");
	fprintf(pFile,"TrackPeriod=%d\n",paSet.trackPeriodParam);
	fprintf(pFile,"MinMovement=%d\n",paSet.minMovementParam);
	fprintf(pFile,"GridGranularity=%d\n",paSet.gridGranularityParam);
	fprintf(pFile,"\n");
	fprintf(pFile,"Surveilance Rect: ");
	for (int i=0;i<rectVect.size(); i++){
		fprintf(pFile,"(x,y,w,h)=(%d,%d,%d,%d), ",rectVect[i].x,rectVect[i].y,rectVect[i].width,rectVect[i].height);
	}
	fprintf(pFile,"\n");
	fprintf(pFile,"Sigma=%f\n",peSet.sigmaParam);
	fprintf(pFile,"FilterSizeParam=(%d,%d)\n",peSet.filterSizeParam.height,peSet.filterSizeParam.width);
	fprintf(pFile,"Frame recorded=(%d-%d)\n",peSet.profileFrameStartParam,peSet.profileFrameStartParam+peSet.profileLengthParam);
	fprintf(pFile,"Profile period=%d\n",peSet.profilePeriodParam);
	fprintf(pFile,"Value period=%d\n",peSet.valuePeriodParam);
	fprintf(pFile,"\n");
	fclose(pFile);

	Mat immagine, frame, reference, iteration, iAnomaly, particleAdvection, iOut;
	bool oneP = true;
	bool oneV = true;
	bool firstFrame = true;
	int countIt = 0;
	int countFrame = 0;
	vector<float> energyProfile, energyValue;
	double maxRef, maxIt;
	vector<Point2f> vect;
	vector<mmLib::mmAnomaly::ANOMALY_TYPE> anomalyVect;

	while(true){
		//cout<<countFrame<<endl;
		cap >> immagine;
		if(immagine.empty()){
			break;
		}

		resize(immagine,frame,Size(dimX/4,dimY/4),scaleX,scaleY);


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
			if(oneV){
				cout << "REC PHASE" << endl;
				oneV = false;
			}
			if ((countFrame+1)%(paSet.trackPeriodParam+1)==0 && countFrame > peSet.profileFrameStartParam){
				pEnergy->processFrame(frame,vect);

				if (countFrame != 0 && (countFrame+1)%(peSet.profilePeriodParam)==0){
					FILE* pFile = fopen("Uni.txt", "a");
					fprintf(pFile,"*********************************FRAME: %d*********************************\n",countFrame);
					energyProfile = pEnergy->getProfileEnVect(countIt);
					fprintf(pFile,"ENERGY PROFILE: %d", countIt);
					for(int l=0; l<energyProfile.size(); l++){
						fprintf(pFile,"-->(%d):: %f ",l,energyProfile[l]);
					}
					fprintf(pFile,"\n");
					fprintf(pFile,"THRESH HIGH: %d", countIt);
					for(int l=0; l<energyProfile.size(); l++){
						fprintf(pFile,"-->(%d):: %f ",l,energyProfile[l]+peSet.threshHighParam);
					}
					fprintf(pFile,"\n");
					fprintf(pFile,"THRESH LOW: %d", countIt);
					for(int l=0; l<energyProfile.size(); l++){
						fprintf(pFile,"-->(%d):: %f ",l,energyProfile[l]-peSet.threshLowParam);
					}
					fprintf(pFile,"\n");
					fclose(pFile);
					energyProfile.clear();

					reference = pEnergy->getMatProfile();
					minMaxLoc(reference,0,&maxRef,0,0);
					reference = reference/maxRef;
					reference = reference*255;
					countIt++;
				}
			}
			if(countFrame>(peSet.profileLengthParam+peSet.profileFrameStartParam) || !pEnergy->getRec()){
				cout << "STOP REC-->FRAME=  " <<  countFrame << endl;
				pEnergy->clearVariables();
				//pEnergy->setRec(false);
				pAccumul->clearVariables();
//				pAccumul = new mmLib::mmAnomaly::mmParticleAccumulation(paSet);
				cap.open(nameVideoFile);
				countFrame = -1;
				countIt = 0;
			}
		}
		else{
			//evaluation phase
			if (oneP){
				cout << "EVALUATION PHASE" << endl;
				oneP = false;
			}
			if ((countFrame+1)%(paSet.trackPeriodParam+1)==0){
				pEnergy->processFrame(frame,vect);

				if (countFrame != 0 && (countFrame+1)%(peSet.valuePeriodParam)==0){
					FILE* pFile = fopen("Uni.txt", "a");
					fprintf(pFile,"*********************************FRAME: %d*********************************\n",countFrame);
					energyValue = pEnergy->getValueEnVect();
					fprintf(pFile,"ENERGY VALUE: %d", countIt);
					for(int l=0; l<energyValue.size(); l++){
						fprintf(pFile,"-->(%d):: %f ",l,energyValue[l]);
					}
					fprintf(pFile,"\n");
					anomalyVect = pEnergy->getAnomalyType();
					if (anomalyVect[0] == mmLib::mmAnomaly::STANDARD)
						fprintf(pFile,"0::STD ");
					else if (anomalyVect[0] == mmLib::mmAnomaly::HIGHANOMALY)
						fprintf(pFile,"0::HIGHANOMALY ");
					else if (anomalyVect[0] == mmLib::mmAnomaly::LOWANOMALY)
						fprintf(pFile,"0::LOWANOMALY ");


					if (anomalyVect[1] == mmLib::mmAnomaly::STANDARD)
						fprintf(pFile,"1::STD ");
					else if (anomalyVect[1] == mmLib::mmAnomaly::HIGHANOMALY)
						fprintf(pFile,"1::HIGHANOMALY ");
					else if (anomalyVect[1] == mmLib::mmAnomaly::LOWANOMALY)
						fprintf(pFile,"1::LOWANOMALY ");


					if (anomalyVect[2] == mmLib::mmAnomaly::STANDARD)
						fprintf(pFile,"2::STD ");
					else if (anomalyVect[2] == mmLib::mmAnomaly::HIGHANOMALY)
						fprintf(pFile,"2::HIGHANOMALY ");
					else if (anomalyVect[2] == mmLib::mmAnomaly::LOWANOMALY)
						fprintf(pFile,"2::LOWANOMALY ");
					fprintf(pFile,"\n");
					fclose(pFile);
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
				//cout<<"Entro size 0"<<endl;
				rectangle(iAnomaly,rectVect[0],Scalar(125,125,125),2);
				//rectangle(iAnomaly,rectVect[1],Scalar(0,255,0),2);
				//rectangle(iAnomaly,rectVect[2],Scalar(0,255,0),2);
			}
			else{
				//cout<<"Entro FOR "<<rectVect.size()<<endl;
				for (int l=0; l<rectVect.size();l++){
					//cout<<anomalyVect[l]<<"-"<<endl;
					if (anomalyVect[l] == mmLib::mmAnomaly::STANDARD)
						rectangle(iAnomaly,rectVect[l],Scalar(0,255,0),2);
					else if (anomalyVect[l] == mmLib::mmAnomaly::HIGHANOMALY)
						rectangle(iAnomaly,rectVect[l],Scalar(0,0,255),2);
					else if (anomalyVect[l] == mmLib::mmAnomaly::LOWANOMALY)
						rectangle(iAnomaly,rectVect[l],Scalar(255,0,0),2);
				}
				//cout<<endl<<endl;
			}
		}


		imshow("Video",frame);
		imshow("Anomaly", iAnomaly);
		waitKey(10);

		if (video.isOpened() && (!pEnergy->getRec() || (pEnergy->getRec() && countFrame > peSet.profileFrameStartParam && countFrame < (peSet.profileFrameStartParam+peSet.profileLengthParam)))){
			video << iAnomaly;
		}

		countFrame++;
		frame.release(); iAnomaly.release();
	}

	reference.release(); iteration.release();	energyProfile.clear(); energyValue.clear(); vect.clear(); anomalyVect.clear(); rectVect.clear();
	// contours.clear(); stuff.clear();
	delete pAccumul;
	delete pEnergy;
}
*/

//TODO: MODULO 4
/*//train file generation
#include "stdlib.h"
#include "stdio.h"
#include <math.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"
#include "mmLib/mmLib.h"

using namespace cv;

//CvVideoWriter* video;

int main(){
	//variables
	vector<string> inputFile;
	inputFile.push_back("FightA");
	inputFile.push_back("FightB");
	inputFile.push_back("FightC");
	inputFile.push_back("FightF");
	inputFile.push_back("ShakeA");
	inputFile.push_back("ShakeB");
	inputFile.push_back("TalkA");
	inputFile.push_back("TalkB");
	//number of frames for video
	vector<int> numFrame;
	numFrame.push_back(339);
	numFrame.push_back(338);
	numFrame.push_back(303);
	numFrame.push_back(293);
	numFrame.push_back(1191);
	numFrame.push_back(1192);
	numFrame.push_back(1791);
	numFrame.push_back(1791);
	//number start frame
	vector<int> startRec;
	startRec.push_back(103);
	startRec.push_back(107);
	startRec.push_back(100);
	startRec.push_back(103);
	startRec.push_back(100);
	startRec.push_back(100);
	startRec.push_back(100);
	startRec.push_back(140);
	//number end frame
	vector<int> endRec;
	endRec.push_back(227);
	endRec.push_back(290);
	endRec.push_back(201);
	endRec.push_back(195);
	endRec.push_back(200);
	endRec.push_back(200);
	endRec.push_back(245);
	endRec.push_back(300);
	//anomaly
	vector<bool> isAnomaly;
	isAnomaly.push_back(1);
	isAnomaly.push_back(1);
	isAnomaly.push_back(1);
	isAnomaly.push_back(1);
	isAnomaly.push_back(0);
	isAnomaly.push_back(0);
	isAnomaly.push_back(0);
	isAnomaly.push_back(0);
	int sigma = 1;
	int tau = 1;
	int gridGranularity = 8;

	for(int i=0; i<inputFile.size(); i++){
		int countFrame = 0;
		int countIntFrame = 0;
		int countParticle = 0;
		int numParticle = 0;
		Mat frame, immResize, foreground, iOut;
		vector<mmBlob> blobList;
		vector<Point> frameParticle;
		string nameVideoFile = "../VIDEO/"+inputFile[i]+".avi";
		//capture a video
		VideoCapture cap;
		cap.open(nameVideoFile);
		if(!cap.isOpened()){
			cout << "ERROR: Stream not found" << endl;
			return 0;
		}

		double fps = cap.get(CV_CAP_PROP_FPS);
		double cols = cap.get(CV_CAP_PROP_FRAME_WIDTH);
		double rows = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
		double fourC = cap.get(CV_CAP_PROP_FOURCC);
		double scaleX = 0.5;
		double scaleY = 0.5;
		int isColor = 1;

		//file to save
		VideoWriter video = VideoWriter("/home/matteo/workspace/opencv/Upload/Train/Video/"+inputFile[i]+"Test.avi", fourC, fps, cvSize(cols, rows), isColor);
		FileStorage xmlFile("/home/matteo/workspace/opencv/Upload/Train/particle/"+inputFile[i]+"Test.xml", cv::FileStorage::WRITE);

		//initalize objects
		mmLib::mmModules::mmBlobDetectorSettings tSet;
		tSet.increasedBRCornerParam = 20;
		tSet.increasedTLCornerParam = 10;
		tSet.minDetHeigthParam = 7;
		tSet.minDetWidthParam = 7;

		mmLib::mmModules::codeBookParameters cbSet;
		cbSet.cbMemory = 100;
		cbSet.alphaValue = 0.8;
		cbSet.betaValue = 1.2;

		mmLib::mmModules::mmCodeBookBgSubstraction* bgSub = new mmLib::mmModules::mmCodeBookBgSubstraction(cbSet);
		mmLib::mmModules::mmBlobDetector* tracker = new mmLib::mmModules::mmBlobDetector(tSet);

		xmlFile << "numFrame" << numFrame[i];
		xmlFile << "isAnomaly" << (int)isAnomaly[i];
		xmlFile << "start" << startRec[i];
		xmlFile << "end" << endRec[i];
		xmlFile << "sigma" << sigma;
		xmlFile << "tau" << tau;
		xmlFile << "ScaleX" << scaleX;
		xmlFile << "ScaleY" << scaleY;

		while(countFrame<numFrame[i]){
			//		cout << "FRAME= " << countFrame << endl;
			cap >> frame;
			resize(frame, immResize, Size(0,0), scaleX, scaleY);
			if(frame.empty()){
				break;
			}

			foreground = bgSub->processFrame(immResize);
			blobList = tracker->BlobDetection(immResize,foreground);
			iOut = mmDrawBlobs(immResize,blobList);

			//create particle file
			if(countFrame>startRec[i] && countFrame<endRec[i]){
				frameParticle = tracker->getBlobParticle(immResize, blobList, gridGranularity);
				for(int j=0; j<frameParticle.size(); j++){
					ostringstream particle;
					particle<<countParticle;
					string particleString = "Particle"+particle.str();
					xmlFile << particleString;
					xmlFile << "{" << "x" << frameParticle[j].x;
					xmlFile << 	      "y" << frameParticle[j].y;
					xmlFile <<        "frame" << countIntFrame << "}";
					countParticle++;
				}
				countIntFrame++;
				numParticle+=frameParticle.size();

				if(video.isOpened())
					video << frame;
			}

			imshow("Video",iOut);
			waitKey(1);
			countFrame++;
			frame.release(); immResize.release(); foreground.release(); iOut.release();
			blobList.clear(); frameParticle.clear();
		}
		xmlFile << "numParticle" << numParticle;
		xmlFile.release();
		delete bgSub;
		delete tracker;
	}
}*/

/*//training SVM
#include "stdlib.h"
#include "stdio.h"
#include <math.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"
#include "mmLib/mmLib.h"

using namespace cv;

//CvVideoWriter* video;

int main(){
	string nameVideo = "FightA";
	string nameVideoFile = "../VIDEO/"+nameVideo+".avi";

	//capture a video
	VideoCapture cap;
	cap.open(nameVideoFile);
	if(!cap.isOpened()){
		cout << "ERROR: Stream not found" << endl;
		return 0;
	}

	double fps = cap.get(CV_CAP_PROP_FPS);
	double cols = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double rows = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	double fourC = cap.get(CV_CAP_PROP_FOURCC);
	double scaleX = 0.5;
	double scaleY = 0.5;
	int isColor = 1;

	mmLib::mmInteraction::mmBehaviourDetectorTrainSettings bdSets;
	vector<FileStorage> inputFiles;
	//input files-->particle files
	FileStorage fileSt;
	fileSt = FileStorage("Train/particle/FightATest.xml", cv::FileStorage::READ);
	inputFiles.push_back(fileSt);
	fileSt = FileStorage("Train/particle/FightBTest.xml", cv::FileStorage::READ);
	inputFiles.push_back(fileSt);
	fileSt = FileStorage("Train/particle/FightCTest.xml", cv::FileStorage::READ);
	inputFiles.push_back(fileSt);
	fileSt = FileStorage("Train/particle/FightFTest.xml", cv::FileStorage::READ);
	inputFiles.push_back(fileSt);
	fileSt = FileStorage("Train/particle/ShakeATest.xml", cv::FileStorage::READ);
	inputFiles.push_back(fileSt);
	fileSt = FileStorage("Train/particle/ShakeBTest.xml", cv::FileStorage::READ);
	inputFiles.push_back(fileSt);
	fileSt = FileStorage("Train/particle/TalkATest.xml", cv::FileStorage::READ);
	inputFiles.push_back(fileSt);
	fileSt = FileStorage("Train/particle/TalkBTest.xml", cv::FileStorage::READ);
	inputFiles.push_back(fileSt);
	fileSt.release();

	//input files-->video files
	vector<string> videoFiles;
	videoFiles.push_back("Train/Video/FightA");
	videoFiles.push_back("Train/Video/FightB");
	videoFiles.push_back("Train/Video/FightC");
	videoFiles.push_back("Train/Video/FightF");
	videoFiles.push_back("Train/Video/ShakeA");
	videoFiles.push_back("Train/Video/ShakeB");
	videoFiles.push_back("Train/Video/TalkA");
	videoFiles.push_back("Train/Video/TalkB");

	bdSets.trainFileParam = inputFiles;
	bdSets.dimensionalityParam = 300;
	bdSets.clusterBinsParam = 100;
	bdSets.videoFilesParam = videoFiles;

	FileStorage xmlFile("/home/matteo/workspace/opencv/Upload/Train/trainFiles/CentClusters_Telecamera1.xml", cv::FileStorage::WRITE);

	mmLib::mmInteraction::mmBehaviourDetector* bDet = new mmLib::mmInteraction::mmBehaviourDetector(bdSets);

	vector<mmLib::mmInteraction::trainParmeters> trainParam;
	mmLib::mmInteraction::trainParmeters trainParamStuff;

	for (int i=0; i<videoFiles.size(); i++){
		trainParamStuff.sigma = 1;
		trainParamStuff.tau = 1;
		trainParamStuff.gridGranularity = 8;
		trainParamStuff.scaleX = 0.5;
		trainParamStuff.scaleY = 0.5;
		int isAnomaly, numParticle;
		inputFiles[i]["isAnomaly"] >> isAnomaly;
		trainParamStuff.isAnomaly = isAnomaly;
		inputFiles[i]["numParticle"] >> numParticle;
		trainParamStuff.numParticle = numParticle;

		vector<Point> particleVect;
		vector<int> frameNum;
		for (int j=0; j<numParticle; j++){
			int x, y, frame;
			ostringstream particle;
			particle<<j;
			string particleString = "Particle" + particle.str();
			FileNode pa = inputFiles[i][particleString];
			pa["x"] >> x;
			pa["y"] >> y;
			pa["frame"] >> frame;
			particleVect.push_back(Point(x,y));
			frameNum.push_back(frame);
		}
		trainParamStuff.particles = particleVect;
		trainParamStuff.frameNum = frameNum;

		trainParam.push_back(trainParamStuff);
	}

	xmlFile << "Dimensionality" << bdSets.dimensionalityParam;
	xmlFile << "ClasterBins" << bdSets.clusterBinsParam;
	xmlFile << "Video";
	xmlFile << "{" << "Height" << rows;
	xmlFile        << "Width" << cols;
	xmlFile        << "ScaleX" << scaleX;
	xmlFile        << "ScaleY" << scaleY;
	xmlFile        << "Character" << fourC;
	xmlFile        << "FrameRate" << fps;
	xmlFile        << "isColor" << isColor << "}";

	//Train svm
	bDet->TrainSvm();

	xmlFile << "Sigma" << bDet->getSigma();
	xmlFile << "Tau" << bDet->getTau();
	xmlFile << "GridGranularity" << bDet->getGridGranularity();

	//get cluster centers
	vector<vector<float> > cent = bDet->getClasterCenterVect();
	xmlFile << "ClasterCenters" << "{";
	for (int i=0; i<cent.size(); i++){
		ostringstream center;
		center<<i;

		for(int j=0; j<cent[i].size(); j++){
			ostringstream x;
			x<<j;
			string in = "center"+center.str()+"x"+x.str();
			xmlFile << in << cent[i][j];
		}
	}
	xmlFile << "}";

	xmlFile.release(); trainParam.clear();

	//get model
	CvSVM model = bDet->getSVMmodel();
	model.save("model.svm");

	delete bDet;
}*/

/*//test SVM
#include "stdlib.h"
#include "stdio.h"
#include <math.h>
#include <iostream>
#include "opencv/cv.h"
#include "opencv/cxcore.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"
#include "mmLib/mmLib.h"

using namespace cv;

//CvVideoWriter* video;

int main(){
	string nameVideo = "ShakeC";
	string nameVideoFile = "../VIDEO/"+nameVideo+".avi";

	//capture a video
	VideoCapture cap;
	cap.open(nameVideoFile);
	if(!cap.isOpened()){
		cout << "ERROR: Stream not found" << endl;
		return 0;
	}

	double fps = cap.get(CV_CAP_PROP_FPS);
	double cols = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double rows = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	double fourC = cap.get(CV_CAP_PROP_FOURCC);
	double scaleX = 0.5;
	double scaleY = 0.5;
	int isColor = 1;

	//save files
	VideoWriter video = VideoWriter("/home/matteo/workspace/opencv/Upload/Test/result/Prova.avi", fourC, fps, cvSize(cols, rows), isColor);

	Mat frame, immResize, foreground, iOut;
	vector<mmBlob> blobList, activeTracks;
	vector<Point> frameParticle;
	Rect rect = Rect(0,0,cols*scaleX,rows*scaleY);
	bool isAnomaly = false;
	int gridGranularity = 8;
	int countFrame = 0;

	//windows
	namedWindow("Video", 1);
	cvMoveWindow("Video",600,0);

	//create objects
	mmLib::mmModules::appSettingsOld appSet;
	appSet.appAlpha = 0.95;
	appSet.appLambda = 0.95;
	appSet.appSigma = 0.5;
	appSet.appThresh = 0.05;

	mmLib::mmModules::mmBlobDetectorSettings tSet;
	tSet.increasedBRCornerParam = 20;
	tSet.increasedTLCornerParam = 10;
	tSet.minDetHeigthParam = 7;
	tSet.minDetWidthParam = 7;
	tSet.splitThreshParam = 70;
	tSet.windowSizeParam = 150;

	mmLib::mmModules::codeBookParameters cbSet;
	cbSet.cbMemory = 100;
	cbSet.alphaValue = 0.8;
	cbSet.betaValue = 1.2;

	FileStorage xmlInfo = FileStorage("Train/trainFiles/CentClusters_Telecamera1.xml", cv::FileStorage::READ);
	CvSVM mod;
	mod.load("model.SVM");

	mmLib::mmInteraction::mmBehaviourDetectorTestSettings bdSets;
	bdSets.windowLengthParam = 100;
	bdSets.xmlClusterCentersParam = xmlInfo;
	bdSets.svmModelStringParam = "SVMmodel.svm";

	mmLib::mmModules::mmCodeBookBgSubstraction* bgSub = new mmLib::mmModules::mmCodeBookBgSubstraction(cbSet);
	mmLib::mmModules::mmBlobDetector* tracker = new mmLib::mmModules::mmBlobDetector(tSet,appSet);
	mmLib::mmInteraction::mmBehaviourDetector* bDet = new mmLib::mmInteraction::mmBehaviourDetector(bdSets);

	//initialize module
	bDet->InitializeModule();

	while(true){
//		cout << "FRAME= " << countFrame << endl;
		cap >> frame;
		resize(frame, immResize, Size(0,0), scaleX, scaleY);
		if(frame.empty()){
			break;
		}

		//get foreground
		foreground = bgSub->processFrame(immResize);

		//get blob list
		blobList = tracker->BlobDetectionBehaviour(immResize, foreground, activeTracks);
		activeTracks = blobList;

		if(blobList.size() != 0)
			frameParticle = tracker->getBlobParticle(immResize, blobList, gridGranularity);

		//get anomaly
		isAnomaly = bDet->TestFrame(frame, frameParticle, "test.avi");

		//draw blobs
		iOut = mmDrawBlobs(immResize,blobList);

		//draw anomaly
		if(isAnomaly)
			rectangle(iOut,rect,Scalar(0,0,255),2);
		else
			rectangle(iOut,rect,Scalar(0,255,0),2);

		imshow("Video",iOut);
		waitKey(1);

		if(video.isOpened())
			video << iOut;

		foreground.release(); frame.release(); immResize.release(); iOut.release();
		blobList.clear(); frameParticle.clear();
		countFrame++;
	}
	delete bgSub;
	delete tracker;
	delete bDet;
}*/
