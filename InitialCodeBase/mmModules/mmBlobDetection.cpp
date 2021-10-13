/*
 * mmBlobDetector.cpp
 *
 *  Created on: Mar 19, 2013
 *      Author: matteo
 */

#include "../mmLib.h"
namespace mmLib {
namespace mmModules{
mmBlobDetectorSettings::mmBlobDetectorSettings(){
	splitThreshParam = 60;
	borderThreshParam = 10;
	minDetWidthParam = 10;
	minDetHeigthParam = 10;
	increasedTLCornerParam = 10;
	increasedBRCornerParam = 10;
	windowSizeParam = 100;
}

mmBlobDetector::mmBlobDetector(){
	splitThresh = 60;
	borderThresh = 10;
	minDetWidth = 10;
	minDetHeigth = 10;
	increasedTLCorner = 10;
	increasedBRCorner = 10;
	windowSize = 100;
	totalFrameProcessed = 0;
	startAnomaly = 0;
	needToTrain = false;
}


mmBlobDetector::mmBlobDetector(mmBlobDetectorSettings &blobSets){
	aMod = new mmLib::mmModules::appModel();
	entranceExitArea = blobSets.entranceExitAreaParam;
	blobSets.entranceExitAreaParam.clear();
	splitThresh = blobSets.splitThreshParam;
	borderThresh = blobSets.borderThreshParam;
	minDetWidth = blobSets.minDetWidthParam;
	minDetHeigth = blobSets.minDetHeigthParam;
	increasedTLCorner = blobSets.increasedTLCornerParam;
	increasedBRCorner = blobSets.increasedBRCornerParam;
	windowSize = blobSets.windowSizeParam;
	totalFrameProcessed = 0;
	startAnomaly = 0;
	needToTrain = false;
}

mmBlobDetector::mmBlobDetector(mmBlobDetectorSettings &blobSets, appSettings &appSets){
	aMod = new mmLib::mmModules::appModel(appSets); //color model
	entranceExitArea = blobSets.entranceExitAreaParam; //people have to pass in this area to enter/exit door
	blobSets.entranceExitAreaParam.clear();
	splitThresh = blobSets.splitThreshParam; //min distance for a splitted blob to be considered as two separated blobs
	borderThresh = blobSets.borderThreshParam; //frames border length (no operations are done in this area)
	minDetWidth = blobSets.minDetWidthParam; //min blob width to be considered as a valid blob
	minDetHeigth = blobSets.minDetHeigthParam;//min blob height to be considered as a valid blob
	increasedTLCorner = blobSets.increasedTLCornerParam; //increased TL corner in blob processing
	increasedBRCorner = blobSets.increasedBRCornerParam; //increased BR corner in blob processing
	windowSize = blobSets.windowSizeParam; //number of frames for svm process
	totalFrameProcessed = 0;
	startAnomaly = 0;
	needToTrain = false;
}

mmBlobDetector::~mmBlobDetector(){
	entranceExitArea.clear();
	aMod = NULL;
}

vector<mmBlob> mmBlobDetector::BlobDetection(Mat frame, Mat foreground){
	//input blobs
	vector<mmBlob> blobVec = detectNewBlob(frame,foreground);
	vector<mmBlob> sizeObjects = mmModules::SizeFilter(frame,foreground,blobVec,minDetWidth,minDetHeigth,increasedTLCorner,increasedBRCorner);
	blobVec.clear();
	vector<mmBlob> macroObjects = mmModules::UnionObjects(frame,foreground,sizeObjects);
	sizeObjects.clear();

	frame.release(); foreground.release();
	return macroObjects;
}

vector<mmBlob> mmBlobDetector::BlobDetectionBehaviour(Mat frame, Mat foreground, vector<mmBlob> activeTracks){
	vector<mmBlob> macroObjects = BlobDetection(frame,foreground);
	vector<mmBlob> visualObjects;
	vector<int> indexTR, indexOB;

	if(!needToTrain){
		//TODO:: solo x video uni
		/*if(totalFrameProcessed==100 && macroObjects.size()==1){
//			cout << "Start= " << totalFrameProcessed << endl;
			needToTrain = true;
			visualObjects.push_back(macroObjects[0]);
			startAnomaly = totalFrameProcessed;
		}
		//check if two blobs are close enough to create an interaction
		else{*/
			for(int i=0; i<macroObjects.size(); i++){
				for(int j=i+1; j<macroObjects.size(); j++){
					if(getDist(macroObjects[i],macroObjects[j])<splitThresh){
//						cout << "Start= " << totalFrameProcessed << endl;
						needToTrain = true;
						visualObjects.push_back(macroObjects[i]);
						visualObjects.push_back(macroObjects[j]);
						startAnomaly = totalFrameProcessed;
					}
				}
			}
//		}
	}
	else{
		//create the tracks vector
		for(int j=0; j<activeTracks.size(); j++)
			indexTR.push_back(-1);

		//track the blobs
		for(int i=0; i<macroObjects.size(); i++){
			indexOB.push_back(-1);
			for(int j=0; j<activeTracks.size(); j++){
				//std case
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

		//check for non visible tracks
		for(int i=0; i<indexTR.size();i++){
			if(indexTR[i]==-1){
				visualObjects.push_back(activeTracks[i]);
			}
		}

		//end condition
		if(visualObjects.size()==2){
			if(getDist(visualObjects[0],visualObjects[1])>splitThresh  && (totalFrameProcessed-startAnomaly)>=windowSize){
//				cout << "End= " << totalFrameProcessed << endl;
				visualObjects.clear();
				needToTrain=false;
				startAnomaly=0;
			}
		}
	}

	//if all blobs disappear from the scene--> end of train needed
	if(macroObjects.size() == 0 && needToTrain){
//		cout << "Forced End= " << totalFrameProcessed << endl;
		needToTrain=false;
	}

	totalFrameProcessed++;
	macroObjects.clear(); indexOB.clear(); indexTR.clear(); activeTracks.clear(); frame.release(); foreground.release();
	return visualObjects;
}

vector<mmBlob> mmBlobDetector::BlobDetectionAppModel(Mat frame, Mat foreground, vector<mmBlob> activeTracks){
	vector<mmBlob> macroObjects = BlobDetection(frame,foreground);

	//variables
	vector<mmBlob> visualObjects, vectorMT;
	vector<int> indexTR, indexOB, indexMT, indexTRInc;
	vector<Point> indexVO;
	int numObjects = macroObjects.size();
	int numTracks  = activeTracks.size();
	int indexSS;
	BLOB_TYPE typeInteraction;

	//initialize index tracks vector
	for(int j=0; j<numTracks;j++){
		indexTR.push_back(-1);
	}
//	FILE* blobFile = fopen("bList.txt", "a");
//	fprintf(blobFile,"\n");
//	fprintf(blobFile,"*********************************FRAME: %d*********************************\n",totalFrameProcessed);
//	fprintf(blobFile,"Oggetti trovati: %d\n",numObjects);
//	fprintf(blobFile,"Tracce esistenti: %d\n",numTracks);
//	fclose(blobFile);

	//for each object
	for(int i=0; i<numObjects; i++){
//		FILE* blobFile = fopen("bList.txt", "a");
		indexOB.push_back(-1);
		typeInteraction = NEW;
		bool isOut = false;

		//ceck correspondence in the active tracks vector
		for(int j=0; j<numTracks; j++){
			//Standard object-->track.centroid() C mObject.centroid() + only one correspondence between track and object
			if(areBlobsContained(macroObjects[i],activeTracks[j]) && indexOB[i] == -1 && indexTR[j] == -1){
//				fprintf(blobFile,"STD:   oggetto %d::(x,y)=(%d,%d) <-> con traccia %d::(x,y)=(%d,%d).\n",i,macroObjects[i].centroid().x,macroObjects[i].centroid().y,j,activeTracks[j].centroid().x,activeTracks[j].centroid().y);
				typeInteraction = STANDARD;
				indexOB[i]=j;
				indexTR[j]=i;
				indexVO.push_back(Point(i,j));

				macroObjects[i] = setParameters(frame, foreground, macroObjects[i], activeTracks[j], typeInteraction);
				macroObjects[i] = aMod->stdAppModel(frame,foreground,macroObjects[i],activeTracks[j]);
				visualObjects.push_back(macroObjects[i]);
			}
			//Merged objects-->track.centroid() C mObject.centroid() + more tracks for an objects
			else if(areBlobsContained(macroObjects[i],activeTracks[j]) && (indexOB[i] != -1) && (indexTR[j] == -1)){
				//tracks and indexes saved
				if(typeInteraction != MERGE){
					typeInteraction = MERGE;
					indexMT.push_back(indexOB[i]);
					vectorMT.push_back(activeTracks[indexOB[i]]);
				}
				indexMT.push_back(j);
				vectorMT.push_back(activeTracks[j]);

				//check if the blobs are merged in a forbidden region
				if(!isBlobInWantedRegion(frame,macroObjects[i])){
//					fprintf(blobFile,"MERGE: oggetto %d::(x,y)=(%d,%d) <-> con le tracce %d::(x,y)=(%d,%d) e %d::(x,y)=(%d,%d) DA NON EFFETTUARE.\n",i,macroObjects[i].centroid().x,macroObjects[i].centroid().y,j,activeTracks[j].centroid().x,activeTracks[j].centroid().y,indexOB[i],activeTracks[indexOB[i]].centroid().x,activeTracks[indexOB[i]].centroid().y);
					isOut = true;
					indexTR[j] = i;
				}
//				else{
//					fprintf(blobFile,"MERGE: oggetto %d::(x,y)=(%d,%d) <-> con le tracce %d::(x,y)=(%d,%d) e %d::(x,y)=(%d,%d) DA EFFETTUARE.\n",i,macroObjects[i].centroid().x,macroObjects[i].centroid().y,j,activeTracks[j].centroid().x,activeTracks[j].centroid().y,indexOB[i],activeTracks[indexOB[i]].centroid().x,activeTracks[indexOB[i]].centroid().y);
//				}
			}
			//Splitted object-->Merged objects-->track.centroid() C mObject.centroid() + more object for a track
			else if(areBlobsContained(macroObjects[i],activeTracks[j]) && (indexOB[i] == -1) && (indexTR[j] != -1)){
				mmBlob stuffBlob = macroObjects[i];
				bool isSplit = true;
				for (int k=0; k<visualObjects.size(); k++){
					if (indexVO[k].y == j && getDist(stuffBlob,visualObjects[k])<splitThresh){
//						fprintf(blobFile,"SPLIT: oggetto %d::(x,y)=(%d,%d) con oggetto %d=(%d,%d) <-> su traccia %d=(%d,%d).\n",i,macroObjects[i].centroid().x,macroObjects[i].centroid().y,indexVO[k].x,visualObjects[k].centroid().x,visualObjects[k].centroid().y,j,activeTracks[j].centroid().x,activeTracks[j].centroid().y);
						Rect unione = stuffBlob.boundingBoxProcessed()|visualObjects[k].boundingBoxProcessed();

						if(isSplit){
							indexSS = k;
							isSplit = false;
							typeInteraction = SPLIT;
						}
						else{
							visualObjects.erase(visualObjects.begin()+k);
							indexVO.erase(indexVO.begin()+k);
							indexOB[indexVO[k].x] = -1;
						}
						stuffBlob = mmBlob(frame, unione.x, unione.y, unione.width, unione.height);
						stuffBlob = setParameters(frame, foreground, stuffBlob, activeTracks[j], typeInteraction);
						stuffBlob = aMod->stdAppModel(frame,foreground,stuffBlob,activeTracks[j]);
						visualObjects[indexSS] = stuffBlob;
						indexVO[indexSS] = Point(i,j);
					}
				}
			}
		}
		//New object-->no correspondence between the object and the tracks
		if(typeInteraction == NEW){
//			fprintf(blobFile,"NEW:   oggetto %d::(x,y)=(%d,%d).\n",i,macroObjects[i].centroid().x,macroObjects[i].centroid().y);
			indexVO.push_back(Point(i,-1));
			macroObjects[i] = aMod->newAppModel(frame,foreground,macroObjects[i]);
			macroObjects[i] = setParameters(frame, foreground, macroObjects[i], mmBlob(), typeInteraction);
			visualObjects.push_back(macroObjects[i]);
		}
		//Solve merge
		else if(typeInteraction == MERGE){
			if(isOut){
				macroObjects[i]  = setParameters(frame, foreground, macroObjects[i], vectorMT[0], typeInteraction);
				macroObjects[i] = aMod->nonMergeAppModel(frame,foreground,macroObjects[i],vectorMT);
				visualObjects.pop_back();
				visualObjects.push_back(macroObjects[i]);
			}
			else{
				vector<mmBlob> vectorSM = aMod->SM(frame,foreground,macroObjects[i],vectorMT);

				for(int k=0; k<vectorSM.size(); k++){
					if(vectorSM[k].getType() != DELETED){
						if(k==0)
							visualObjects.pop_back();
						else{
							indexTR[indexMT[k]] = (i+1);
							indexVO.push_back(Point(i,indexMT[k]));
						}
						vectorSM[k] = setParameters(frame,foreground,vectorSM[k],vectorMT[k],typeInteraction);
						vectorSM[k] = aMod->mergeAppModel(frame,foreground,vectorSM[k],activeTracks[indexMT[k]]);
						visualObjects.push_back(vectorSM[k]);
					}
				}
				vectorSM.clear();
			}
		}
		indexMT.clear(); vectorMT.clear();
//		fclose(blobFile);
	}
	//check non visible track-->no object for a track
	int inactiveTracks = 0;
	double minVal;
	Point minIdx;
	if(numTracks>0){
		add(indexTR,1,indexTRInc);
		inactiveTracks = indexTR.size()-countNonZero(indexTRInc);
	}

	for(int i=0; i<inactiveTracks; i++){
		typeInteraction = STANDARD;
		minMaxLoc(indexTR,&minVal,0,&minIdx,0);
		mmBlob propagatedBlob = aMod->propagateObject(frame, foreground, activeTracks[minIdx.x]);


		if(isBlobInWantedRegion(frame,propagatedBlob) && propagatedBlob.getType() != DELETED){
			propagatedBlob = setParameters(frame, foreground, propagatedBlob,activeTracks[minIdx.x],typeInteraction);
			//check if the blob is in a entrance/exit area
			for(int j=0; j<entranceExitArea.size(); j++){
				if(isPointInRect(entranceExitArea[j],propagatedBlob.centroid())){
//					FILE* blobFile = fopen("bList.txt", "a");
//					fprintf(blobFile,"NON VISIBLE TRACK: la traccia::%d (x,y)=(%d,%d) è IN ZONA ALLARME.\n",minIdx.x,activeTracks[minIdx.x].centroid().x,activeTracks[minIdx.x].centroid().y);
//					fclose(blobFile);
					typeInteraction = NONVISIBLE;
					propagatedBlob = aMod->stdAppModel(frame,foreground,propagatedBlob,activeTracks[minIdx.x]);
					propagatedBlob.setType(typeInteraction);
					visualObjects.push_back(propagatedBlob);
					indexVO.push_back(Point(-1,minIdx.x));
				}
			}

			//if is outside the entrance/exit area check for intersections
			if(typeInteraction != NONVISIBLE){
				bool isIntersectedNew = false;
				for(int j=0; j<visualObjects.size(); j++){
					Rect unione = visualObjects[j].boundingBoxProcessed() & propagatedBlob.boundingBoxProcessed();
					mmBlob newBlob, track;
					typeInteraction = visualObjects[j].getType();
					if(unione != Rect(0,0,0,0)){
						if(typeInteraction == NEW){
							track = activeTracks[minIdx.x];
							newBlob = mmBlob(frame,unione.x,unione.y,unione.width,unione.height);
							isIntersectedNew = true;
						}
						else {
							track = activeTracks[indexVO[j].y];
							newBlob = aMod->propagateObject(frame,foreground,track);
						}
						newBlob = setParameters(frame,foreground,newBlob,track,typeInteraction);
						newBlob = aMod->stdAppModel(frame,foreground,newBlob,track);
						visualObjects[j] = newBlob;
					}
				}
				if(!isIntersectedNew){
					propagatedBlob = aMod->stdAppModel(frame,foreground,propagatedBlob,activeTracks[minIdx.x]);
					visualObjects.push_back(propagatedBlob);
					indexVO.push_back(Point(-1,minIdx.x));
				}
//				FILE* blobFile = fopen("bList.txt", "a");
//				fprintf(blobFile,"NON VISIBLE TRACK: la traccia %d::(x,y)=(%d,%d) è STATA RIPRISTINATA.\n",minIdx.x,activeTracks[minIdx.x].centroid().x,activeTracks[minIdx.x].centroid().y);
//				fclose(blobFile);
			}
			indexTR[minIdx.x] = numTracks;
		}
	}

//	totalFrameProcessed++;
	indexTR.clear(); indexOB.clear(); activeTracks.clear(); indexVO.clear(); vectorMT.clear(); macroObjects.clear(); indexMT.clear(); indexTRInc.clear();
	frame.release(); foreground.release();

	return visualObjects;
}

vector<Point> mmBlobDetector::getBlobParticle(Mat frame, vector<mmLib::mmModules::mmBlob> blobList, int gridGranularity){
	vector<Point> result;

	for(int i=0; i<blobList.size(); i++){
		for (int c=blobList[i].getTL().x; c<blobList[i].getBR().x; c=c+gridGranularity){
			for (int r=blobList[i].getTL().y; r<blobList[i].getBR().y; r=r+gridGranularity){
				if(isPointContRect(blobList[i].boundingBoxProcessed(),Point(c+gridGranularity/2,r+gridGranularity/2))){
					result.push_back(Point(c+gridGranularity/2,r+gridGranularity/2));
				}
			}
		}
	}
	blobList.clear(); frame.release();
	return result;
}

bool mmBlobDetector::areBlobsContained(mmBlob blob1, mmBlob blob2){
	bool result = false;

	if(blob1.isConteined(blob2.centroid()) || blob2.isConteined(blob1.centroid()))
		result = true;

	return result;
}

mmBlob mmBlobDetector::setParameters(Mat frame, Mat foreground, mmBlob blobNew, mmBlob blobOld, BLOB_TYPE type){
	mmBlob result = blobNew;

	if(type == NEW){
		result.setColor(assignRandomColor());
		result.setDispVect(Point(0,0));
	}
	else{
		result.setColor(blobOld.getColor());
		result.setDispVect(Point(blobNew.centroid().x - blobOld.centroid().x,blobNew.centroid().y - blobOld.centroid().y));
		result.setCentList(blobOld.getCentList());
	}
	result.setType(type);
	vector<Point> centList = result.getCentList();
	centList.push_back(result.centroid());
	result.setCentList(centList);

	centList.clear();
	frame.release(); foreground.release();

	return result;
}

//TODO: check se il blob mmBlob(frame,0,0,10,10) è problematico
bool mmBlobDetector::isBlobInWantedRegion(Mat frame, mmBlob blob){
	bool result = false;
	Rect rect = Rect(borderThresh,borderThresh,frame.cols-borderThresh,frame.rows-borderThresh);

	if(isPointInRect(rect,blob.centroid()))
		result = true;

	frame.release();
	return result;
}

bool isPointInRect(Rect rect, Point p){
	bool result = false;

	if(p.x >= rect.x && p.x <= (rect.x+rect.width) && p.y>= rect.y && p.y <= (rect.y+rect.height))
		result = true;

	return result;
}

static int CompContour(const void* a, const void* b, void* ){
	float           dx,dy;
	float           h,w,ht,wt;
	CvPoint2D32f    pa,pb;
	CvRect          ra,rb;
	CvSeq*          pCA = *(CvSeq**)a;
	CvSeq*          pCB = *(CvSeq**)b;
	ra = ((CvContour*)pCA)->rect;
	rb = ((CvContour*)pCB)->rect;
	pa.x = ra.x + ra.width*0.5f;
	pa.y = ra.y + ra.height*0.5f;
	pb.x = rb.x + rb.width*0.5f;
	pb.y = rb.y + rb.height*0.5f;
	w = (ra.width+rb.width)*0.5f;
	h = (ra.height+rb.height)*0.5f;

	dx = (float)(fabs(pa.x - pb.x)-w);
	dy = (float)(fabs(pa.y - pb.y)-h);

	//wt = MAX(ra.width,rb.width)*0.1f;
	wt = 0;
	ht = MAX(ra.height,rb.height)*0.3f;

	cvClearSeq(pCA);	pCA = NULL;
	cvClearSeq(pCB);	pCB = NULL;

	return (dx < wt && dy < ht);
}

vector<mmBlob> mmBlobDetector::detectNewBlob(Mat frame, Mat foreground){
	//initialize variables
	vector<mmBlob> blobVec;
	CvMemStorage*   storage = cvCreateMemStorage();
	CvBlobSeq* blobSeq = new CvBlobSeq();
	CvBlobSeq* Blobs = new CvBlobSeq();
	IplImage* iplCurrentForeground;
	int  blobNumber = 0;

	iplCurrentForeground = cvCreateImage(cvSize(frame.cols,frame.rows), IPL_DEPTH_8U, 1);
	iplCurrentForeground->imageData = (char *) foreground.data;
	CvSize S = cvSize(iplCurrentForeground->width,iplCurrentForeground->height);
	blobSeq->Clear();

	IplImage*       pIB = NULL;
	CvSeq*          cnt = NULL;
	CvSeq*          cnt_list = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvSeq*), storage );
	CvSeq*          clasters = NULL;

	//find contours
	pIB = cvCloneImage(iplCurrentForeground);
	cvThreshold(pIB,pIB,128,255,CV_THRESH_BINARY);
	cvFindContours(pIB,storage, &cnt, sizeof(CvContour), CV_RETR_EXTERNAL);
	cvReleaseImage(&pIB);

	//create a cnt list
	for(; cnt; cnt=cnt->h_next)
		cvSeqPush( cnt_list, &cnt);

	cnt = NULL;
	int claster_num = cvSeqPartition( cnt_list, storage, &clasters, CompContour, NULL );

	//process each contour
	for(int claster_cur=0; claster_cur<claster_num; ++claster_cur){
		int         cnt_cur;
		CvBlob      NewBlob;
		double     M00,X,Y,XX,YY;
		CvMoments   m;
		CvRect      rect_res = cvRect(-1,-1,-1,-1);
//		CvMat*      mat;
		CvMat      mat;

		for(cnt_cur=0; cnt_cur<clasters->total; ++cnt_cur){
			CvRect  rect;
			CvSeq*  cnt;
			int k = *(int*)cvGetSeqElem( clasters, cnt_cur );
			if(k!=claster_cur) continue;
			cnt = *(CvSeq**)cvGetSeqElem( cnt_list, cnt_cur );
			rect = ((CvContour*)cnt)->rect;

			cvClearSeq(cnt);
			cnt = NULL;

			if(rect_res.height<0)
				rect_res = rect;
			else{
				int x0,x1,y0,y1;
				x0 = MIN(rect_res.x,rect.x);
				y0 = MIN(rect_res.y,rect.y);
				x1 = MAX(rect_res.x+rect_res.width,rect.x+rect.width);
				y1 = MAX(rect_res.y+rect_res.height,rect.y+rect.height);
				rect_res.x = x0;
				rect_res.y = y0;
				rect_res.width = x1-x0;
				rect_res.height = y1-y0;
			}


		}

		if(rect_res.height < 1 || rect_res.width < 1){
			X = 0;
			Y = 0;
			XX = 0;
			YY = 0;
		}
		else{
//			mat = cvCreateMat(rect_res.height,rect_res.width,CV_8U);
			cvMoments( cvGetSubRect(iplCurrentForeground,&mat,rect_res), &m, 0 );
			M00 = cvGetSpatialMoment( &m, 0, 0 );
			if(M00 <= 0 ) continue;
			X = cvGetSpatialMoment( &m, 1, 0 )/M00;
			Y = cvGetSpatialMoment( &m, 0, 1 )/M00;
			XX = (cvGetSpatialMoment( &m, 2, 0 )/M00) - X*X;
			YY = (cvGetSpatialMoment( &m, 0, 2 )/M00) - Y*Y;
		}
//		cvReleaseMat(&mat);
		cvReleaseData(&mat);

		NewBlob = cvBlob(rect_res.x+(float)X,rect_res.y+(float)Y,(float)(4*sqrt(XX)),(float)(4*sqrt(YY)));
		Blobs->AddBlob(&NewBlob);
	}

	cvClearSeq(cnt_list);	cnt_list = NULL;
	cvClearSeq(clasters);	clasters = NULL;

	//detect small and intersected blobs and change them
	int blobNu = Blobs->GetBlobNum();
	for(int i=blobNu; i>0; i--)	{
		CvBlob* pB = Blobs->GetBlob(i-1);

		if(pB->h < S.height*0.002 || pB->w < S.width*0.002){
			Blobs->DelBlob(i-1);
			continue;
		}
		blobSeq->AddBlob(pB);
		pB = NULL;
	}

	Blobs->Clear();	Blobs->~CvBlobSeq(); Blobs=NULL;
	cvClearMemStorage(storage);
	cvReleaseMemStorage(&storage);

	//create final blobs
	blobNumber = blobSeq->GetBlobNum();
	for (int i=0; i< blobNumber; i++){
		CvBlob* cBlob = NULL;
		cBlob = blobSeq->GetBlob(i);

		int x, y, w, h;
		w = (cBlob->w<frame.cols)?cBlob->w:frame.cols-1;
		h = (cBlob->h<frame.rows)?cBlob->h:frame.rows-1;
		x = (cBlob->x - w/2>0)?cBlob->x - w/2:0;
		y = (cBlob->y - h/2>0)?cBlob->y - h/2:0;

		cBlob = NULL;

		mmBlob blob = mmBlob(frame, x, y, w, h);
		blobVec.push_back(blob);
	}

	frame.release(); foreground.release();
	blobSeq->Clear(); blobSeq->~CvBlobSeq(); blobSeq=NULL;
	cvReleaseImage(&iplCurrentForeground);

	return blobVec;
}
}
}
