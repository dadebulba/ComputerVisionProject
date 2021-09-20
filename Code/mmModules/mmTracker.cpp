/*
 * mmTracker.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: paolo
 */

#include "../mmLib.h"
#define		MM_DEBUG		1

using namespace cv;

namespace mmLib {
namespace mmModules{


trackerSettings::trackerSettings(){
	backgroundMod = GAUSSIAN_BACKGROUND_PARAM;
	blobDetMod = MORPHOLOGICAL_CLEANING;
	//blobDetMod = CLNF;
	trakingMod = MM_DB_TRACKER;
	printForeground = false;
	//printf("Entro qui e io non voglio che tu lo faccia!\n");

	// Parametri del background detector
	CvGaussBGStatModelParams params;
	params.win_size      = CV_BGFG_MOG_WINDOW_SIZE;
	params.bg_threshold  = CV_BGFG_MOG_BACKGROUND_THRESHOLD;
	params.std_threshold = CV_BGFG_MOG_STD_THRESHOLD;
	params.weight_init   = CV_BGFG_MOG_WEIGHT_INIT;
	params.variance_init = CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT;
	params.minArea       = CV_BGFG_MOG_MINAREA;
	params.n_gauss       = CV_BGFG_MOG_NGAUSSIANS;

	backgroundParam = params;

	// Blob Detector per clnf
	// Al momento non li caga!
	toleranceH = 0.02;
	toleranceW = 0.05;
	sThresh = 60;
	rectArea = 10;
	minDetWhidth = 10;
	minDetHeigth = 10;
	incresedDetTL = 10;
	incresedDetBR = 10;
}

// TODO: Non riesco a passare i parametri al FG detector
mmModules::mmGaussianFGDetector* chooseBgModule(trackerSettings t){
	mmModules::mmGaussianFGDetector* aux;
	//TODO: mettere apposto questo:
	//printf("choice: %d\n", t.backgroundMod);
	switch(t.backgroundMod)
	{
	//	case GAUSSIAN_BACKGROUND:
	//		aux = new mmModules::mmGaussianFGDetector();
	//		//aux = mmModules::mmCreateGaussianFGDetector(t.backgroundParam);
	//		break;
	case GAUSSIAN_BACKGROUND_PARAM:
		aux = new mmModules::mmGaussianFGDetector(t.backgroundParam);

		break;
	default:
		printf("WARNING: No such background subtractor. \n");
		aux = new mmModules::mmGaussianFGDetector();
	}
	return aux;
}

CvBlobDetector* chooseBDecModule(trackerSettings t){
	CvBlobDetector* aux;
	switch(t.blobDetMod)
	{
	case MORPHOLOGICAL_CLEANING:
		aux = mmModules::CreateClusterSearch();
		break;
	case CLNF:
		aux = mmModules::CreateClnf();
		//aux->tolH = t.toleranceH;
		//aux->tolW = t.toleranceW;
		break;
	default:
		printf("WARNING: No such blob detection module. \n");
		aux = mmModules::CreateClusterSearch();
	}
	return aux;
}


mmTracker::mmTracker() {
	// I parametri del tracker sono settati dal costruttore di default di trackerSettings
	tParam = trackerSettings();
	fDec = chooseBgModule(tParam);
	bDec = chooseBDecModule(tParam);

	dThresh = 60;
	centThresh = 30;
	minBlobWidth = 7;
	minBlobHeigth = 7;

	// Choose Tracker
	switch(tParam.trakingMod)
	{
	case MM_DB_TRACKER:
		db = new mmDatabase();
		break;
	case MM_KALMAN_TRACKER:
		kDb = new mmKalmanDatabase();

		break;
	default:
		printf("WARNING: No such tracker module. \n");
	}
}

mmTracker::mmTracker(trackerSettings &t){
	tParam = t;
	fDec = chooseBgModule(tParam);
	bDec = chooseBDecModule(tParam);
	aMod = new mmLib::mmModules::appModel();

	nonVisibleArea = t.nonVisibleAreaParam;
	dThresh = t.sThresh;
	centThresh = t.rectArea;
	minBlobWidth = t.minDetWhidth;
	minBlobHeigth = t.minDetHeigth;
	incTL = t.incresedDetTL;
	incBR = t.incresedDetBR;
	windowSize= t.windowSizeParam;
	totalFrameProcessed = 0;
	startAnomaly = 0;
	needToTrain = false;
	t.nonVisibleAreaParam.clear();

	// Choose Tracker
	switch(tParam.trakingMod){
	case MM_DB_TRACKER:
		db = new mmDatabase();
		break;
	case MM_KALMAN_TRACKER:
		kDb = new mmKalmanDatabase();
		break;
	default:
		printf("WARNING: No such tracker module. \n");
	}
}

mmTracker::mmTracker(trackerSettings &t, appSettings &a){
	tParam = t;
	fDec = chooseBgModule(tParam);
	bDec = chooseBDecModule(tParam);
	aMod = new mmLib::mmModules::appModel(a);

	nonVisibleArea = t.nonVisibleAreaParam;
	dThresh = t.sThresh;
	centThresh = t.rectArea;
	minBlobWidth = t.minDetWhidth;
	minBlobHeigth = t.minDetHeigth;
	incTL = t.incresedDetTL;
	incBR = t.incresedDetBR;
	windowSize= t.windowSizeParam;
	totalFrameProcessed = 0;
	startAnomaly = 0;
	needToTrain = false;
	t.nonVisibleAreaParam.clear();

//	//DEBUG
//	FILE* blobFile = fopen("Log/bList.txt","w");
//	fprintf(blobFile, "Starting...\n");
//	fclose(blobFile);
}

mmTracker::~mmTracker() {
//	bDec->Release();
	fDec->Release();
	nonVisibleArea.clear();
}
void mmTracker::Release(){
	fDec->Release();
}
Mat mmTracker::BackgroundSubtraction(Mat frame){
	Mat currentFrame;
	Mat currentForeground;
	IplImage* iplCurrentFrame;
	frame.copyTo(currentFrame);
	iplCurrentFrame = new IplImage(currentFrame);

	//printf("winsize: %d\n", fDec->mmParams.win_size);
	fDec->ChangeParams(tParam.backgroundParam);
	fDec->Process(iplCurrentFrame);

	currentForeground = Mat(fDec->GetMask());
	//printf("print foreground: %s\n", (tParam.printForeground)?"true":"false");
	if (tParam.printForeground == true)
	{
		namedWindow("Foreground", 1);
		//Mat outF;
		//resize(currentForeground, outF, currentForeground.size() * 2);
		imshow("Foreground", currentForeground);
	}

	currentFrame.release(); frame.release();
	return currentForeground;
}
////////////////////////////////////////////
vector<mmBlob> mmTracker::detectNewBlob(Mat frame, Mat foreground){
	vector<mmBlob> blobVec;
	CvBlobSeq* blobSeq = new CvBlobSeq();
	CvBlobSeq* blobDummy = new CvBlobSeq();

	IplImage* iplCurrentFrame;
	iplCurrentFrame = cvCreateImage(cvSize(frame.cols,frame.rows), IPL_DEPTH_8U, 3);
	iplCurrentFrame->imageData = (char *) frame.data;
	IplImage* iplCurrentForeground;
	iplCurrentForeground = cvCreateImage(cvSize(frame.cols,frame.rows), IPL_DEPTH_8U, 1);
	iplCurrentForeground->imageData = (char *) foreground.data;

	int blobNumber = bDec->DetectNewBlob(iplCurrentFrame, iplCurrentForeground, blobSeq, blobDummy);
	for (int i=0; i< blobNumber; i++){
		CvBlob* cBlob = blobSeq->GetBlob(i);

		int x, y, w, h;
		w = (cBlob->w<frame.cols)?cBlob->w:frame.cols-1;
		h = (cBlob->h<frame.rows)?cBlob->h:frame.rows-1;
		x = (cBlob->x - w/2>0)?cBlob->x - w/2:0;
		y = (cBlob->y - h/2>0)?cBlob->y - h/2:0;

		mmBlob blob = mmBlob(frame, x, y, w, h);
		blobVec.push_back(blob);
	}

	frame.release();
	foreground.release();

	blobSeq->Clear();
	delete blobSeq;
	blobDummy->Clear();
	delete blobDummy;

	cvReleaseImage(&iplCurrentFrame);
	cvReleaseImage(&iplCurrentForeground);

	return blobVec;
}

mmBlob setParameters(Mat frame, mmBlob blobNew, mmBlob blobOld, bool isNew, bool isMerge){
	mmBlob result = blobNew;

	if(isNew){
		result.setType(NEW);
		result.setColor(assignRandomColor());
		result.setDispVect(Point(0,0));
	}
	else{
		if (isMerge){
			result.setType(MERGE);
		}
		else{
			result.setType(STANDARD);
		}
		result.setColor(blobOld.getColor());
		result.setDispVect(Point(blobNew.centroid().x - blobOld.centroid().x,blobNew.centroid().y - blobOld.centroid().y));
		result.setCentList(blobOld.getCentList());
	}
	vector<Point> centList = result.getCentList();
	centList.push_back(result.centroid());
	result.setCentList(centList);

	return result;
}

vector<Point> mmTracker::getBlobParticle(Mat frame, vector<mmBlob> blobList, int gridGranularity){
	vector<Point> result;

	for(int i=0; i<blobList.size(); i++)
		for (int c=blobList[i].getTL().x; c<blobList[i].getBR().x; c=c+gridGranularity)
			for (int r=blobList[i].getTL().y; r<blobList[i].getBR().y; r=r+gridGranularity)
				if(isPointContRect(blobList[i].boundingBoxProcessed(),Point(c+gridGranularity/2,r+gridGranularity/2)))
					result.push_back(Point(c+gridGranularity/2,r+gridGranularity/2));

	blobList.clear(); frame.release();
	return result;
}

vector<mmBlob> mmTracker::BlobDetectionFiltered(Mat frame, Mat foreground){
	//input blobs
	vector<mmBlob> blobVec = detectNewBlob(frame,foreground);
	vector<mmBlob> sizeObjects = mmModules::SizeFilter(frame,foreground,blobVec,minBlobWidth,minBlobHeigth,incTL,incBR);
	blobVec.clear();
	vector<mmBlob> macroObjects = mmModules::UnionObjects(frame,foreground,sizeObjects);
	sizeObjects.clear();

	frame.release(); foreground.release();
	return macroObjects;
}

vector<mmBlob> mmTracker::BlobDetectionStd(Mat frame, Mat foreground, vector<mmBlob> activeTracks){
	//input blobs
	vector<mmBlob> blobVec = detectNewBlob(frame,foreground);
	vector<mmBlob> sizeObjects = mmModules::SizeFilter(frame,foreground,blobVec,minBlobWidth,minBlobHeigth,incTL,incBR);
	blobVec.clear();
	vector<mmBlob> macroObjects = mmModules::UnionObjects(frame,foreground,sizeObjects);
	sizeObjects.clear();
	vector<mmBlob> visualObjects;
	vector<int> indexTR, indexOB;

	if(!needToTrain){
		//TODO:solo x video reccati
		if(totalFrameProcessed==100 && macroObjects.size()==1){
			needToTrain = true;
			visualObjects.push_back(macroObjects[0]);
			startAnomaly = totalFrameProcessed;
		}
		//merged blobs-->start test if they are clone enough
		else{
			for(int i=0; i<macroObjects.size(); i++){
				for(int j=i+1; j<macroObjects.size(); j++){
					if(getDist(macroObjects[i],macroObjects[j])<dThresh){
						needToTrain = true;
						visualObjects.push_back(macroObjects[i]);
						visualObjects.push_back(macroObjects[j]);
						startAnomaly = totalFrameProcessed;
					}
				}
			}
		}
	}
	else{
		for(int j=0; j<activeTracks.size(); j++)
			indexTR.push_back(-1);

		for(int i=0; i<macroObjects.size(); i++){
			indexOB.push_back(-1);
			for(int j=0; j<activeTracks.size(); j++){
				//std blobs
				if((activeTracks[j].isConteined(macroObjects[i].centroid()) || macroObjects[i].isConteined(activeTracks[j].centroid())) && (indexOB[i] == -1) && (indexTR[j] == -1)){
					visualObjects.push_back(macroObjects[i]);
					indexOB[i] = j;
					indexTR[j] = i;
				}
				//splitted blobs
				else if((activeTracks[j].isConteined(macroObjects[i].centroid()) || macroObjects[i].isConteined(activeTracks[j].centroid())) && (indexOB[i] == -1) && (indexTR[j] != -1)){
					visualObjects.push_back(macroObjects[i]);
					indexOB[i] = j;
					indexTR[j] = i;
				}
				//merged blobs
				else if((activeTracks[j].isConteined(macroObjects[i].centroid()) || macroObjects[i].isConteined(activeTracks[j].centroid())) && (indexOB[i] != -1) && (indexTR[j] == -1)){
					indexTR[j] = i;
				}
			}
		}

		//nn visible blobs
		for(int i=0; i<indexTR.size();i++){
			if(indexTR[i]==-1){
				visualObjects.push_back(activeTracks[i]);
			}
		}
		//stop test-->if the the two blobs are far enough and the the blobs remained clone one with each other for at least windowSize frames
		if(visualObjects.size()==2){
			if(getDist(visualObjects[0],visualObjects[1])>dThresh  && (totalFrameProcessed-startAnomaly)>=windowSize){
				visualObjects.clear();
				needToTrain=false;
				startAnomaly=0;
			}
		}
	}

	//stop test-->if there are no blobs in the frame
	if(macroObjects.size() == 0 && needToTrain){
		needToTrain=false;
	}

	frame.release(); foreground.release();
	indexTR.clear(); indexOB.clear();
	totalFrameProcessed++;
	return visualObjects;
}

vector<mmBlob> mmTracker::BlobDetectionApp(Mat frame, Mat foreground, vector<mmBlob> activeTracks){
	//input blobs
	vector<mmBlob> blobVec = detectNewBlob(frame,foreground);
	vector<mmBlob> sizeObjects = mmModules::SizeFilter(frame,foreground,blobVec,minBlobWidth,minBlobHeigth,incTL,incBR);
	blobVec.clear();
	vector<mmBlob> macroObjects = mmModules::UnionObjects(frame,foreground,sizeObjects);
	sizeObjects.clear();

	//variables
	bool isNew, isMerge, isNonMerge, isFirstMerge, isSplit, isNonVisibleNew;
	double distSO, minVal;
	Point minInd;
	int indexSS, countNV = 0;
	int numObjects = macroObjects.size();
	int numTracks  = activeTracks.size();
	mmBlob mObject, track, stuffBlob, newVO;
	Rect rectA, rectB, unione;
	vector<mmBlob> visualObjects, vectorMT, vectorSM;
	vector<int> indexTR, indexOB, indexMT;
	vector<Point> indexVO;

	//initialize index tracks vector
	for(int j=0; j<numTracks;j++){
		indexTR.push_back(-1);
	}

	//for each object
	for(int i=0; i<numObjects; i++){
		mObject = macroObjects[i];
		indexOB.push_back(-1);

		indexMT.clear();
		vectorMT.clear();

		isNew = true;
		isMerge = false;
		isNonMerge = false;

		//ceck correspondence in the active tracks vector
		for(int j=0; j<numTracks; j++){
			track = activeTracks[j];

			//Standard object-->track.centroid() C mObject.centroid() + only one correspondence between track and object
			if((mObject.isConteined(track.centroid()) || track.isConteined(mObject.centroid())) && indexOB[i] == -1 && indexTR[j] == -1){

				isNew = false;
				indexOB[i]=j;
				indexTR[j]=i;
				indexVO.push_back(Point(i,j));

				mObject = setParameters(frame, mObject, track, isNew, isMerge);
				mObject = aMod->stdAppModel(frame,foreground,mObject,track);

				visualObjects.push_back(mObject);
			}
			//Merged objects-->track.centroid() C mObject.centroid() + more tracks for an objects
			else if((mObject.isConteined(track.centroid()) || track.isConteined(mObject.centroid())) && (indexOB[i] != -1) && (indexTR[j] == -1)){
				isNew = false;
				//if the blob is in a certain area of the image-->merge operation not to be done
				if (isObjectOutOfRange(frame, mObject)){
					isNonMerge = true;
					indexTR[j] = i;
				}
				//tracks and indexes saved
				if (!isMerge){
					isMerge = true;
					indexMT.push_back(indexOB[i]);
					vectorMT.push_back(activeTracks[indexOB[i]]);
				}
				indexMT.push_back(j);
				vectorMT.push_back(activeTracks[j]);
			}
			//Splitted object-->Merged objects-->track.centroid() C mObject.centroid() + more object for a track
			else if((mObject.isConteined(track.centroid()) || track.isConteined(mObject.centroid())) && (indexOB[i] == -1) && (indexTR[j] != -1)){
				stuffBlob = mObject;
				isSplit = true;

				//TODO::CAMBIATO-->find the blobs in visual objects to check split
				for (int k=0; k<visualObjects.size(); k++){
					if (indexVO[k].y == j){
						distSO = getDist(stuffBlob,visualObjects[k]);

						if (distSO<dThresh){
							isNew = false;
							rectA = stuffBlob.boundingBoxProcessed();
							rectB = visualObjects[k].boundingBoxProcessed();
							unione = rectA|rectB;

							stuffBlob = mmBlob(frame, unione.x, unione.y, unione.width, unione.height);
							stuffBlob = setParameters(frame, stuffBlob, track, false, false);
							stuffBlob.setType(SPLIT);

							if(isSplit){
								indexSS = k;
								isSplit = false;
							}
							else{
								visualObjects = deleteBlobFromVect(visualObjects,k);
								indexVO = deletePoint(indexVO,k);
								indexOB[indexVO[k].x] = -1;
							}

							stuffBlob = aMod->stdAppModel(frame,foreground,stuffBlob,track);
							visualObjects[indexSS] = stuffBlob;
							//TODO::potrebbe nn servire
							indexVO[indexSS] = Point(i,j);
//							indexOB[i] = j;
						}
					}
				}
			}
		}
		//New object-->no correspondence between the object and the tracks
		if (isNew){
			indexVO.push_back(Point(i,-1));

			mObject = aMod->newAppModel(frame,foreground,mObject);
			mObject = setParameters(frame, mObject, mmBlob(), isNew, false);

			visualObjects.push_back(mObject);
		}
		//Merge solved
		if (isMerge){
			isFirstMerge = true;
			if (isNonMerge){
				mObject  = setParameters(frame,mObject,vectorMT[0],isNew,isMerge);
				mObject = aMod->nonMergeAppModel(frame,foreground,mObject,vectorMT);

				visualObjects.pop_back();
				visualObjects.push_back(mObject);
			}
			else{
				vectorSM = aMod->SM(frame,foreground,mObject,vectorMT);

				for (int k=0; k<vectorSM.size(); k++){
					if ((vectorSM[k].getWidth() != 10 || vectorSM[k].getHeight() != 10 || vectorSM[k].getTL() != Point(0,0)) && (vectorSM[k].getWidth() >10 || vectorSM[k].getHeight() > 10)){
						if (isFirstMerge){
							visualObjects.pop_back();
							isFirstMerge = false;
						}
						else{
							indexTR[indexMT[k]] = (i+1);
							indexVO.push_back(Point(i,indexMT[k]));
						}
						vectorSM[k] = setParameters(frame,vectorSM[k],vectorMT[k],isNew,isMerge);
						vectorSM[k] = aMod->mergeAppModel(frame,foreground,vectorSM[k],activeTracks[indexMT[k]]);

						visualObjects.push_back(vectorSM[k]);
						isFirstMerge = false;
					}
				}
				vectorSM.clear();
			}
			indexMT.clear(); vectorMT.clear();
		}
	}
	//check non visible track-->no object for a track
	for (int i=0;i<indexTR.size();i++){
		countNV = (indexTR[i] == -1)?countNV+1:countNV;
	}
	for (int i=0;i<countNV;i++){
		minMaxLoc(indexTR,&minVal,0,&minInd,0);
		isNonVisibleNew = false;

		//for each non visible area
		for (int j=0; j<nonVisibleArea.size();j++){
			stuffBlob = aMod->propagateObject(frame, foreground, activeTracks[minInd.x]);
			if (stuffBlob.getTL() != Point(0,0) && stuffBlob.getBR() != Point(10,10)){
				stuffBlob = setParameters(frame,stuffBlob,activeTracks[minInd.x],false,false);

				//if the predicted blob is in the non visible area-->sign as non visible object, otherwise it could be processed normally
				if(isPointContRect(nonVisibleArea[j],stuffBlob.centroid())){
					stuffBlob = aMod->stdAppModel(frame,foreground,stuffBlob,activeTracks[minInd.x]);
					stuffBlob.setType(NONVISIBLE);
					visualObjects.push_back(stuffBlob);
				}
				else{
					//check all the visual objects for intersection
					for(int k=0; k<visualObjects.size(); k++){
						unione = visualObjects[k].boundingBoxProcessed() & stuffBlob.boundingBoxProcessed();

						if(unione != Rect(0,0,0,0)){
							//if the intersection is made with a new blob-->the union must be preserved
							if(visualObjects[k].getType() == NEW){
								newVO = mmBlob(frame,unione.x,unione.y,unione.width,unione.height);
								isNonVisibleNew = true;

								newVO = setParameters(frame,newVO,activeTracks[minInd.x],true,false);
								newVO = aMod->stdAppModel(frame,foreground,newVO,activeTracks[minInd.x]);
							}
							//otherwise the blob must be corrected
							else{
								newVO = aMod->propagateObject(frame,foreground,activeTracks[indexVO[k].y]);
								if (visualObjects[k].getType() == STANDARD){
									newVO = setParameters(frame,newVO,activeTracks[indexVO[k].y],false,false);
								}
								else if (visualObjects[k].getType() == MERGE){
									newVO = setParameters(frame,newVO,activeTracks[indexVO[k].y],false,true);
								}
								newVO = aMod->stdAppModel(frame,foreground,newVO,activeTracks[indexVO[k].y]);
							}
							visualObjects[k] = newVO;
						}
					}
					if (!isNonVisibleNew){
						stuffBlob = aMod->stdAppModel(frame,foreground,stuffBlob,activeTracks[minInd.x]);
						visualObjects.push_back(stuffBlob);
						indexVO.push_back(Point(-1,minInd.x));
					}
				}
				indexTR[minInd.x] = numTracks;
			}
		}
	}

	macroObjects.clear(); vectorMT.clear(); vectorSM.clear(); indexTR.clear(); indexOB.clear(); indexMT.clear(); indexVO.clear();

	return visualObjects;
}

bool isPointContRect(Rect r, Point p){
	if (p.x >= r.x && p.x <= (r.x+r.width) && p.y>= r.y && p.y <= (r.y+r.height)){
		return true;
	}
	else{
		return false;
	}
}

//TODO:: la funzione ritorna true se il blob si trova nei pressi dei contorni del frame
bool mmTracker::isObjectOutOfRange(Mat frame, mmBlob blob){
	bool result = true;

	if ((blob.getWidth() != 10 || blob.getHeight() != 10 || blob.getTL() != Point(0,0)) &&
			(blob.centroid().x > centThresh && blob.centroid().x < (frame.cols - centThresh)) &&
			(blob.centroid().y > centThresh && blob.centroid().y < (frame.rows - centThresh))){
		result = false;
	}

	return result;
}

vector<Point> deletePoint(vector<Point> vectInt, int pos){
	vector<Point> result;

	for (int i=0; i<vectInt.size(); i++){
		if (vectInt[i].x != pos){
			result.push_back(vectInt[i]);
		}
	}

	vectInt.clear();
	return result;
}

vector<mmBlob> deleteBlobFromVect(vector<mmBlob> stuff,int i){
	vector<mmBlob> result;
	int size = stuff.size();

	for (int j=0; j<size; j++){
		if (j!=i){
			result.push_back(stuff[j]);
		}
	}
	stuff.clear();
	return result;
}

////////////////////////////////////////////
vector<mmBlob> mmTracker::BlobDetection(Mat frame, Mat foreground, int minBlobWidth, int minBlobHeigth){
	vector<mmBlob> blobVec;

	Mat currentFrame, currentForeground;
	frame.copyTo(currentFrame);
	foreground.copyTo(currentForeground);
	IplImage* iplCurrentFrame = new IplImage(currentFrame);
	IplImage* iplCurrentForeground = new IplImage(currentForeground);
	int blobNumber;
	CvBlobSeq* blobSeq = new CvBlobSeq();
	CvBlobSeq* blobDummy = new CvBlobSeq();
	currentFrame.release(); currentForeground.release();

	blobNumber = bDec->DetectNewBlob(iplCurrentFrame, iplCurrentForeground, blobSeq, blobDummy);


	for (int i=0; i< blobNumber; i++)
	{
		CvBlob* cBlob = blobSeq->GetBlob(i);
		int x, y, w, h;
		//printf("blob %d\n", i);
		w = cBlob->w;
		h = cBlob->h;
		x = cBlob->x - w/2;
		y = cBlob->y - h/2;
		mmBlob blob = mmBlob(frame, x, y, w, h);
		blobVec.push_back(blob);
		//blob.hist = blob.histogram(frame);
		float* histBlue = new float[256] ;
		float* histGreen = new float[256];
		float* histRed = new float [256];
		blob.getBins(histBlue, histGreen, histRed);
		//mmShowHistogram(histBlue, "Blue Histogram");
		//mmShowHistogram(histGreen, "Green Histogram");
		//mmShowHistogram(histRed, "Red histogram");
		//float compare = mmHistogramMatchingMSE(histBlue, histRed);
		//printf("Comare = %f\n", compare);
	}
	vector<mmBlob> killedBlob = mmModules::BlobKiller(frame, blobVec);
	blobVec.clear();

	// uccido i blob troppo vicini al margine
	int killedSize = killedBlob.size();
	Size s = frame.size();
	int tolX = 0.01 * s.width;
	int tolY = 0.01 * s.height;
	//printf ("Non accettati blob vicini ai bordi: X=%d y=%d\n", tolX, tolY);
	vector<mmBlob> noMarginBlob;
	for (int i=0; i<killedSize; i++)
	{
		int x = killedBlob[i].centroid().x;
		int y = killedBlob[i].centroid().y;
		// vicino ai margini
		if (x < tolX 				||
				x > (s.width - tolX)	||
				y < tolY				||
				y > (s.height - tolY))
		{
			continue;
		}
		else
		{
			noMarginBlob.push_back(killedBlob[i]);
		}
	}

	// SIZE FILTER --> Uccido blob troppo piccoli
	//	Size minBlobSize(15, 40);
	Size minBlobSize(minBlobWidth, minBlobHeigth);
	vector<mmBlob> blobList;
	//vector<Point> bPoints;
	for(int i=0; i<noMarginBlob.size(); i++){
		if ((noMarginBlob[i].getWidth() > minBlobSize.width) && (noMarginBlob[i].getHeight() > minBlobSize.height)){
			blobList.push_back(noMarginBlob[i]);
		}
	}

//	FILE* pFile = fopen("Log/log.txt", "a");
//	fprintf(pFile,"Nomargin size = %d\n", noMarginBlob.size());
//	fprintf(pFile,"Bloblist size = %d\n", blobList.size());
//	fclose(pFile);

	// Aggiungo l'immagine di foreground al blob
	for(int i=0; i<blobList.size(); i++){

		//UP M e prob

		int x,y,w,h;
		x = blobList[i].boundingBoxProcessed().x;
		y = blobList[i].boundingBoxProcessed().y;
		w = blobList[i].boundingBoxProcessed().width;
		h = blobList[i].boundingBoxProcessed().height;
		if (x<0){
			x = 0;
		}
		if (y<0){
			y = 0;
		}
		if (x + w > foreground.size().width){
			w = foreground.size().width - x;
		}
		if (y + h > foreground.size().height){
			h = foreground.size().height - y;
		}
//		FILE* pFile = fopen("Log/log.txt", "a");
//		fprintf(pFile,"Coordinate blob: x= %d y = %d w = %d h = %d\n", blobList[i].boundingBoxProcessed().x, blobList[i].boundingBoxProcessed().y, blobList[i].boundingBoxProcessed().width, blobList[i].boundingBoxProcessed().height);
//		fprintf(pFile,"Coordinate blob corrette: x= %d y = %d w = %d h = %d\n", x, y, w, h);
//		fclose(pFile);
		Rect r(x, y, w, h);
		Mat piece(foreground, r);
		Mat piece1;
		piece.copyTo(piece1);
		piece.release();
		blobList[i].setForeground(piece1);
		piece1.release();
	}
	delete blobSeq;

	frame.release(); foreground.release(); killedBlob.clear(); noMarginBlob.clear();
	return blobList;
}

void mmTracker::DbTracker(vector<mmBlob>& blobList, vector<mmBlob>& oldList, float threshold){

	db->checkItTwice(blobList, threshold);

	oldList = blobList;
}

double PointDistance(Point a, Point b)
{
	double xA, xB, yA, yB, sq1, sq2, result;
	xA = (double)a.x;
	xB = (double)b.x;
	yA = (double)a.y;
	yB = (double)b.y;
	sq1 = (xA - xB)*(xA - xB);
	sq2 = (yA - yB)*(yA - yB);
	result = sqrt(sq1 + sq2);
	return result;
}

void mmTracker::kalmanTracker(Mat frame, vector<mmBlob> blobList, double threshold){

	// nel caso la threshold è = 0 allora la settiamo standard come 1/8 della dimensione minore del frame.
	if(threshold == 0){
		for (int i=0; i<blobList.size(); i++){
			double dist = PointDistance(blobList[i].centroid(), blobList[i].getTL());
			if (dist > threshold)
				threshold = dist;
		}
	}
	threshold = threshold+2;
	framecounter++;
	FILE* pFile = fopen("Log/log.txt", "a");
	fprintf(pFile, "************************* FRAME NUMBER: %d *******************\n",framecounter);
	fprintf(pFile, "Bloblist size: %d\n",blobList.size());
	fprintf(pFile, "Kalman List size: %d\n",kDb->kList.size());
	fprintf(pFile, "Threshold: %f\n",threshold);
	fclose(pFile);
	vector<mmBlob> auxList = blobList;

	if (blobList.size() > kDb->kList.size())
	{
		for(int i=0; i<kDb->kList.size(); i++)
		{
			double distMin = PointDistance(auxList[0].centroid(), kDb->kList[i].pt);
			int index = 0;
			for (int j=1; j<auxList.size(); j++)
			{
				double dist = PointDistance(auxList[j].centroid(), kDb->kList[i].pt);
				if (dist < distMin)
				{
					distMin = dist;
					index = j;
				}
			}
			auxList.erase(auxList.begin() + index);
		}

		// Blob non assegnati
		for (int i = 0; i<auxList.size(); i++)
		{
			KalmanIstance ist;
			ist.kFilter = new mmModules::KalmanFilter();
			ist.pt = auxList[i].centroid();
			kDb->maxId++;
			ist.id = kDb->maxId;
			ist.img = auxList[i].getImage();
			ist.fgnd = auxList[i].getForeground();
			ist.isOccluded = 0;
			kDb->kList.push_back(ist);

			int lastOne = kDb->kList.size() - 1;
			int x = kDb->kList[lastOne].pt.x;
			int y = kDb->kList[lastOne].pt.y;
			kDb->kList[lastOne].kFilter->predictionBegin((float) x, (float) y);
			kDb->kList[lastOne].kFilter->predictionUpdate((float) x, (float) y);
			kDb->kList[lastOne].pt = Point(x, y);
		}

	}
#if MM_DEBUG
	/////////// TODO: SOLO PER DEBUG ////////////

	for (int i=0; i<blobList.size(); i++){
		char fileName[50];
		sprintf(fileName, "Log/Blob_%03d.jpg", i);
		imwrite(fileName, blobList[i].getImage());
	}
	for (int i=0; i<kDb->kList.size(); i++){
		char fileName[50];
		sprintf(fileName, "Log/Kalman_%03d.jpg", kDb->kList[i].id);
		imwrite(fileName, kDb->kList[i].img);
	}

	/////////////////////////////////////////////
#endif

	// Assegnamo ad ogni blob il Kalman + adatto, se il kalman esce da un'occlusione si lavora per similitudine, altrimenti per prossimità
	for (int i = 0; i<kDb->kList.size(); i++){
		FILE* pFile = fopen("Log/log.txt", "a");
		fprintf(pFile, "Kalman %d: is occluded? %d\n", kDb->kList[i].id, kDb->kList[i].isOccluded);
		fclose(pFile);
		if ((kDb->kList[i].isOccluded > 0) && (blobList.size() > 0)){
			float simil = test_matching_v2(kDb->kList[i].img, kDb->kList[i].fgnd, blobList[0].getImage(), blobList[0].getForeground());;
			int indexB = 0;
			for (int j=1; j<blobList.size(); j++){
				float match = test_matching_v2(kDb->kList[i].img, kDb->kList[i].fgnd, blobList[j].getImage(), blobList[j].getForeground());
				if (match<simil){
					simil = match;
					indexB = j;
				}
			}
			double dist = PointDistance(blobList[indexB].centroid(), kDb->kList[i].pt);
			FILE* pFile = fopen("Log/log.txt", "a");
			fprintf(pFile, "Kalman %d: is more similar to blob %d with matching %f and Dist = %f\n", kDb->kList[i].id, indexB, simil, dist);
			fclose(pFile);
			if (dist < threshold)
				blobList[indexB].bindedKalman.push_back(i);
			kDb->kList[i].isOccluded++;
			if (kDb->kList[i].isOccluded > 5)
				kDb->kList[i].isOccluded =0;
		}
		else{
			double distMin;
			int index = kDb->findCloser(kDb->kList[i], blobList, distMin);
			if (index == -1)
				break;
			FILE* pFile = fopen("Log/log.txt", "a");
			fprintf(pFile, "Kalman %d: Closer blob is %d	Dist = %f\n", kDb->kList[i].id, index, distMin);
			fclose(pFile);
			if (distMin < threshold)
				blobList[index].bindedKalman.push_back(i);
		}
	}


	for (int i = 0; i<blobList.size(); i++){
		FILE* pFile = fopen("Log/log.txt", "a");
		fprintf(pFile, "Blob %d	Width: %d	Heigth: %d\n", i, blobList[i].getWidth(), blobList[i].getHeight());
		fprintf(pFile, "Bloblist[%d].Bindedsize() = %d\n", i, blobList[i].bindedKalman.size());
		fclose(pFile);

		// Caso in cui ogni blob ha un kalman
		if (blobList[i].bindedKalman.size() == 1){
			int j = blobList[i].bindedKalman[0];
			float x, y;
			x = blobList[i].centroid().x;
			y = blobList[i].centroid().y;
			kDb->kList[j].kFilter->predictionUpdate(x, y);
			kDb->kList[j].img = blobList[i].getImage();
			kDb->kList[j].fgnd = blobList[i].getForeground();
		}

		// Caso in cui un blob ha più di un kalman
		else if (blobList[i].bindedKalman.size() > 1){
			// OCCLUSIONE TODO: Mettere apposto
			for (int j=0; j<blobList[i].bindedKalman.size(); j++){
				kDb->kList[blobList[i].bindedKalman[j]].isOccluded = 1;
			}
			// Cerco nei blob senza kalman se ce n'è uno che metcha + del blob attuale per il kalman.
			// Faccio l'update delle posizioni ma NON dell'immagine

			// Ciclo tutti i blob
			bool newBlobfound = false;
			for (int j = 0; j<blobList.size(); j++){
				// se ne trovo uno senza kalman legati
				if (blobList[j].bindedKalman.size() == 0){
					float matchDist;
					int index;
					// allora ciclo tutti i kalman legati al blob i-esimo e ne ricavo quello che matcha di + con quello libero (ovvero j)
					for(int t=0; t<blobList[i].bindedKalman.size(); t++){
						FILE* pFile = fopen("Log/log.txt", "a");

						if (t == 0){

							//matchDist = test_matching(blobList[j].getImage(), kDb->kList[blobList[i].bindedKalman[t]].img); // TODO: aggiungere funzione histMatching(img(bloblist[j]), img(bloblist[i].kalmanBinded[t]))
							matchDist = test_matching_v2(blobList[j].getImage(), blobList[j].getForeground(), kDb->kList[blobList[i].bindedKalman[t]].img, kDb->kList[blobList[i].bindedKalman[t]].fgnd);
							index = blobList[i].bindedKalman[t];
#if MM_DEBUG
							char fileName[50];
							sprintf(fileName, "Log/Alone_%03d.jpg", kDb->kList[blobList[i].bindedKalman[t]].id);
							imwrite(fileName, kDb->kList[blobList[i].bindedKalman[t]].img);
							fprintf(pFile, "match blob %d on kalman %d = %f\n", j, kDb->kList[blobList[i].bindedKalman[t]].id, matchDist);
#endif

						}
						else{
							//float match = test_matching(blobList[j].getImage(), kDb->kList[blobList[i].bindedKalman[t]].img); // TODO: aggiungere funzione histMatching(img(bloblist[j]), img(bloblist[i].kalmanBinded[t]))
							float match = test_matching_v2(blobList[j].getImage(), blobList[j].getForeground(), kDb->kList[blobList[i].bindedKalman[t]].img, kDb->kList[blobList[i].bindedKalman[t]].fgnd);
#if MM_DEBUG
							char fileName[50];
							sprintf(fileName, "Log/Alone_%03d.jpg", kDb->kList[blobList[i].bindedKalman[t]].id);
							imwrite(fileName, kDb->kList[blobList[i].bindedKalman[t]].img);
							fprintf(pFile, "match blob %d on kalman %d = %f\n", j, kDb->kList[blobList[i].bindedKalman[t]].id, match);
#endif
							if(match<matchDist){
								matchDist = match;
								index = blobList[i].bindedKalman[t];
							}
						}
						fclose(pFile);
						kDb->kList[blobList[i].bindedKalman[t]].isOccluded = 1;
					}
					int x = blobList[j].centroid().x;
					int y = blobList[j].centroid().y;
					FILE* pFile = fopen("Log/log.txt", "a");
					fprintf(pFile, "Al blob %d assegno kalman %d con matching %f\n", j, kDb->kList[index].id, matchDist);
					fclose(pFile);
					//kDb->kList[blobList[i].bindedKalman[index]].kFilter->predictionUpdate((float)x, (float)y);
					double dist = PointDistance(blobList[j].centroid(), kDb->kList[index].pt);
					if (dist < threshold){
						kDb->kList[index].kFilter->predictionUpdate((float)x, (float)y);
						kDb->kList[index].img = blobList[j].getImage();
						kDb->kList[index].fgnd= blobList[j].getForeground();
						newBlobfound = true;
					}
				}
			}
			if (!newBlobfound){
				for (int j = 0; j< blobList[i].bindedKalman.size(); j++){
					Rect r = blobList[i].boundingBoxProcessed();
					int x = kDb->kList[blobList[i].bindedKalman[j]].pt.x;
					int y = kDb->kList[blobList[i].bindedKalman[j]].pt.y;
					Point p(x,y);
					if (r.contains(p)){
						kDb->kList[blobList[i].bindedKalman[j]].kFilter->predictionUpdate((float)x, (float)y);
					}
					else{
						int x = blobList[i].centroid().x;
						int y = blobList[i].centroid().y;
						kDb->kList[blobList[i].bindedKalman[j]].kFilter->predictionUpdate((float)x, (float)y);
					}
				}

			}

		}
		else{
			// BLOB LIBERO SENZA KALMAN
		}
	}


	// Predico tutti i kalman
	for (int i = 0; i<kDb->kList.size(); i++)
	{
		CvPoint pt;
		kDb->kList[i].kFilter->predictionReport(pt);
		kDb->kList[i].pt = Point(pt.x, pt.y);
		FILE* pFile = fopen("Log/log.txt", "a");
		fprintf(pFile, "Predicted kList[%d]: %d / %d\n",i, pt.x, pt.y);
		fclose(pFile);
	}




	// Cancello kalman fuori dallo schermo
	for (int i = 0; i < kDb->kList.size(); i++)
	{
		CvPoint pt;
		pt.x = kDb->kList[i].pt.x;
		pt.y = kDb->kList[i].pt.y;

		// se sono all'interno dello schermo li salvo in newlist altrimenti li cancello
		if ((pt.x < 0) || (pt.x > frame.size().width) || (pt.y < 0) || (pt.y > frame.size().height) )
		{
			FILE* pFile = fopen("Log/log.txt", "a");
			fprintf(pFile, "Cancello blob: %d\n", i);//kDb->kList[i].id);
			kDb->kList.erase(kDb->kList.begin() + i);
			fprintf(pFile, "Dimensione list = %d\n", kDb->kList.size());
			i--;
			fclose(pFile);
		}
		else
		{
			// Se sono nei bordi

		}
	}

}
}
}


