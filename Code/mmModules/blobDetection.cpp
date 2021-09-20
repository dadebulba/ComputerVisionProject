/*
 * blobDetection.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: paolo
 */

//#include "blobDetection.h"
#include <stdio.h>
#include "highgui.h"
#include <vector>
#include "../mmLib.h"
#define		MM_DEBUG		0

using namespace cv;

namespace mmLib {

namespace mmModules {

vector<mmBlob> BlobKiller(Mat frame, vector<mmBlob> blobList){
	vector<mmBlob> killedList;
	vector<mmBlob> purgedList;
	int blobNumber = blobList.size();
	for (int i = 0; i < blobNumber; i++)
	{
		killedList.push_back(blobList[i]);
		for (int j = 0; j < blobNumber; j++)
		{
			Rect rNull(0,0,0,0);
			Rect rectA(killedList[i].boundingBoxProcessed());
			Rect rectB(blobList[j].boundingBoxProcessed());
			// Se rectA & rectB == rNull significa che non c'è intersezione e blobList[i] và bene così
			if ((rectA & rectB) == rNull)
			{
				continue;

			}
			// C'è intersezione
			else
			{
				// creo blob unione
				Rect unione = rectA | rectB;
				mmBlob blobUnione(frame, unione.x, unione.y, unione.width, unione.height);

				// aggiungo il rect in killedList nella posizione i se chiamata 2 vole nello stesso ciclo sovrascrivo
				killedList[i] = blobUnione;
			}
		}
	}

	// Cancello blob uguali

	for (int i = 0; i < killedList.size(); i++)
	{
		bool check = false;
		for (int j = i+1; j < killedList.size(); j++)
		{
			if ((killedList[i].boundingBoxProcessed().x == killedList[j].boundingBoxProcessed().x) && (killedList[i].boundingBoxProcessed().y == killedList[j].boundingBoxProcessed().y))
			{
				check = true;

			}

		}
		if (check == false)
		{

			purgedList.push_back(killedList[i]);
		}

	}

	frame.release(); killedList.clear(); blobList.clear();
	return purgedList;
}

vector<mmBlob> BlobChoice(Mat frame, vector<mmBlob> blobList){
	vector<mmBlob> blobVect;
	int blobSize = blobList.size();
	Size s = frame.size();
	int tolX = 0.01 * s.width;
	int tolY = 0.01 * s.height;

	for (int i=0; i<blobSize; i++)
	{
		int x = blobList[i].centroid().x;
		int y = blobList[i].centroid().y;
		// vicino ai margini
		if (x < tolX 	||	x > (s.width - tolX)	||	y < tolY ||	y > (s.height - tolY)){
			continue;
		}
		else{
			blobVect.push_back(blobList[i]);
		}
	}
	frame.release(); blobList.clear();
	return blobVect;
}


vector<mmBlob> UnionObjects(cv::Mat frame, cv::Mat foreground, vector<mmBlob> blobList){
	vector<mmBlob> unionList;
	Rect rNull(0,0,0,0);
	int blobNum = blobList.size();

	for (int i=0; i<blobNum; i++){
		Rect rectA(blobList[i].boundingBoxProcessed());

		for (int j=0; j<blobNum; j++){
			Rect rectB(blobList[j].boundingBoxProcessed());
			// Se rectA & rectB == rNull significa che non c'è intersezione e blobList[i] và bene così altrimenti c'è intersezione
			if ((rectA & rectB) != rNull){
				// creo blob unione
				Rect unione = rectA | rectB;
				mmBlob blobUnione(frame, unione.x, unione.y, unione.width, unione.height);

				// aggiungo il rect in killedList nella posizione i se chiamata 2 vole nello stesso ciclo sovrascrivo
				blobList[i] = blobUnione;
				blobList[j] = blobUnione;
			}
		}
	}

	for (int i = 0; i < blobList.size(); i++){
		bool check = false;
		for (int j = i+1; j < blobList.size(); j++)	{
			if ((blobList[i].boundingBoxProcessed().x == blobList[j].boundingBoxProcessed().x) && (blobList[i].boundingBoxProcessed().y == blobList[j].boundingBoxProcessed().y)){
				check = true;
			}
		}
		if (check == false)	{
			unionList.push_back(blobList[i]);
		}
	}

	frame.release(); foreground.release(); blobList.clear();

	return unionList;
}

//TODO: i contorni connessi si potrebbero utilizzare in fase di creazione del blob per capire quali punti sono della persona e quali di f
vector<mmBlob> SizeFilter(cv::Mat frame, cv::Mat foreground, vector<mmBlob> blobList,int minBlobWidth, int minBlobHeigth, int setTL, int setBR){
	Size minBlobSize(minBlobWidth, minBlobHeigth);
	vector<mmBlob> blobVect;
	int tx,ty,bx,by;

	for (int i=0; i<blobList.size(); i++){
		vector<Point> p;
		if ((blobList[i].getWidth() > minBlobSize.width) && (blobList[i].getHeight() > minBlobSize.height)){
			tx = (blobList[i].getTL().x - setTL>=0)?blobList[i].getTL().x - setTL:0;
			ty = (blobList[i].getTL().y - setTL>=0)?blobList[i].getTL().y - setTL:0;
			bx = (tx+blobList[i].getWidth()+setBR<frame.cols)?tx+blobList[i].getWidth()+setBR:frame.cols-1;
			by = (ty+blobList[i].getHeight()+setBR<frame.rows)?ty+blobList[i].getHeight()+setBR:frame.rows-1;

			for (int c=tx; c<bx; c++){
				for (int r=ty; r<by; r++){
					if (foreground.at<uchar>(r,c) != 0){
						p.push_back(Point(c,r));
					}
				}
			}
			mmBlob blob = mmBlob(frame,p);
			blobVect.push_back(blob);
		}
		p.clear();
	}

	frame.release(); foreground.release(); blobList.clear();

	return blobVect;
}

static int CompareContour(const void* a, const void* b, void* )
{
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
    return (dx < wt && dy < ht);
}

void FindBlobsByCCClasters(IplImage* pFG, CvBlobSeq* pBlobs, CvMemStorage* storage)
{   /* Create contours: */
	IplImage*       pIB = NULL;
	CvSeq*          cnt = NULL;
	CvSeq*          cnt_list = cvCreateSeq(0,sizeof(CvSeq),sizeof(CvSeq*), storage );
	CvSeq*          clasters = NULL;
	int             claster_cur, claster_num;

	pIB = cvCloneImage(pFG);
	cvThreshold(pIB,pIB,128,255,CV_THRESH_BINARY);
	cvFindContours(pIB,storage, &cnt, sizeof(CvContour), CV_RETR_EXTERNAL);
	cvReleaseImage(&pIB);

	/* Create cnt_list.      */
	/* Process each contour: */
	for(; cnt; cnt=cnt->h_next)
	{
		cvSeqPush( cnt_list, &cnt);
	}

	claster_num = cvSeqPartition( cnt_list, storage, &clasters, CompareContour, NULL );

	//printf("cluster Number: %d\n", claster_num);
	for(claster_cur=0; claster_cur<claster_num; ++claster_cur)
	{
		int         cnt_cur;
		CvBlob      NewBlob;
		double      M00,X,Y,XX,YY; /* image moments */
		CvMoments   m;
		CvRect      rect_res = cvRect(-1,-1,-1,-1);
		CvMat       mat;

		//printf("cluster Number: %d\n", clasters->total);
		for(cnt_cur=0; cnt_cur<clasters->total; ++cnt_cur)
		{
			CvRect  rect;
			CvSeq*  cnt;
			int k = *(int*)cvGetSeqElem( clasters, cnt_cur );
			if(k!=claster_cur) continue;
			cnt = *(CvSeq**)cvGetSeqElem( cnt_list, cnt_cur );
			rect = ((CvContour*)cnt)->rect;

			if(rect_res.height<0)
			{
				rect_res = rect;
			}
			else
			{   /* Unite rects: */
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
			cvClearSeq(cnt);
		}

		if(rect_res.height < 1 || rect_res.width < 1)
		{
			X = 0;
			Y = 0;
			XX = 0;
			YY = 0;
		}
		else
		{
			cvMoments( cvGetSubRect(pFG,&mat,rect_res), &m, 0 );
			M00 = cvGetSpatialMoment( &m, 0, 0 );
			if(M00 <= 0 ) continue;
			X = cvGetSpatialMoment( &m, 1, 0 )/M00;
			Y = cvGetSpatialMoment( &m, 0, 1 )/M00;
			XX = (cvGetSpatialMoment( &m, 2, 0 )/M00) - X*X;
			YY = (cvGetSpatialMoment( &m, 0, 2 )/M00) - Y*Y;
		}
		NewBlob = cvBlob(rect_res.x+(float)X,rect_res.y+(float)Y,(float)(4*sqrt(XX)),(float)(4*sqrt(YY)));
		pBlobs->AddBlob(&NewBlob);

	}   /* Next cluster. */
	cvClearMemStorage(storage);
	cvReleaseMemStorage(&storage);
	cvClearSeq(cnt_list);
	cvClearSeq(clasters);
//	cvReleaseImage(&pFG);
//#if MM_DEBUG
//	{   // Debug info:
//		IplImage* pI = cvCreateImage(cvSize(pFG->width,pFG->height),IPL_DEPTH_8U,3);
//		cvZero(pI);
//		for(claster_cur=0; claster_cur<claster_num; ++claster_cur)
//		{
//			int         cnt_cur;
//			CvScalar    color = CV_RGB(255, 130, 75); //rand()%256,rand()%256,rand()%256);
//
//			for(cnt_cur=0; cnt_cur<clasters->total; ++cnt_cur)
//			{
//				CvSeq*  cnt;
//				int k = *(int*)cvGetSeqElem( clasters, cnt_cur );
//				if(k!=claster_cur) continue;
//				cnt = *(CvSeq**)cvGetSeqElem( cnt_list, cnt_cur );
//				cvDrawContours( pI, cnt, color, color, 0, 1, 8);
//			}
//
//			CvBlob* pB = pBlobs->GetBlob(claster_cur);
//			int x = cvRound(CV_BLOB_RX(pB)), y = cvRound(CV_BLOB_RY(pB));
//			cvEllipse( pI,
//					cvPointFrom32f(CV_BLOB_CENTER(pB)),
//					cvSize(MAX(1,x), MAX(1,y)),
//					0, 0, 360,
//					color, 1 );
//
//			Point center = cvPointFrom32f(CV_BLOB_CENTER(pB));
//			// Stampa la posizione del blob:
//			//printf("BLOB_X: x= %d y= %d - Image: %d/%d - Cluster: %d/%d\n", center.x, center.y, pFG->width, pFG->height, pI->width, pI->height);
//			Point tl = center - Point(CV_BLOB_WX(pB)/2, CV_BLOB_WY(pB)/2);
//			Point br = center + Point(CV_BLOB_WX(pB)/2, CV_BLOB_WY(pB)/2);
//			Mat mmImg = Mat(pI);
//			rectangle(mmImg, tl, br, Scalar(255,255,255), 2);
//		}
//
//		cvNamedWindow( "Clusters", 0);
//		cvShowImage( "Clusters",pI );
//
//		cvReleaseImage(&pI);
//
//	}   /* Debug info. */
//#endif

//	cvRelease(&cnt);
//	cvClear
//	cvClearSeq(cnt);
//	delete cnt;
//	delete cnt_list;
//	delete clasters;
}   /* cvFindBlobsByCCClasters */

clusterSearch::clusterSearch() {

}

clusterSearch::~clusterSearch() {
	delete this;
}

int clusterSearch::DetectNewBlob(IplImage* pImg, IplImage* pFGMask, CvBlobSeq* pNewBlobList, CvBlobSeq* /*pOldBlobList*/){
	int         result = 0;
	Point pDilate = Point(10,10);
	CvSize      S = cvSize(pFGMask->width,pFGMask->height);
	//cvCopy(pFGMask, m_pMaskBlobNew);

	CvBlobSeq       Blobs;
	CvMemStorage*   storage = cvCreateMemStorage();
	//pNewBlobList->Clear();
	{   /* Glue contours: */
		FindBlobsByCCClasters(pFGMask, &Blobs, storage );
	}   /* Glue contours. */
	{  //  Delete small and intersected blobs:
		for(int i=Blobs.GetBlobNum(); i>0; i--)
		{
			CvBlob* pB = Blobs.GetBlob(i-1);

			if(pB->h < S.height*0.002 || pB->w < S.width*0.002)
			{
				Blobs.DelBlob(i-1);
				continue;
			}
			pNewBlobList->AddBlob(pB);
		}   // Check next blob.
	}   //  Delete small and intersected blobs.
	cvClearMemStorage(storage);
	Blobs.Clear();
	result = pNewBlobList->GetBlobNum();
//	delete storage;
	return result;
}

void clusterSearch::Release(){
	delete this;
}

struct connection{
	int fisrt;
	int second;
};

clnf::clnf(){
	tolW = 0.03;
	tolH = 0.05;
}

clnf::~clnf(){

}

int clnf::DetectNewBlob(IplImage* pImg, IplImage* pFGMask, CvBlobSeq* pNewBlobList, CvBlobSeq* pOldBlobList){
	Mat img = pImg;
	Mat fgnd = pFGMask;
	// Creo ed inizializzo la matrice dei tag
	int** tagImg = new int*[img.size().width];
	for (int i = 0; i< img.size().width; i++)
	{
		tagImg[i] = new int[img.size().height];
	}
	for(int y=0; y<img.size().height; y++)
	{
		for (int x=0; x<img.size().width; x++)
		{
			tagImg[x][y] = 255;
		}
	}
	int tagNum = 0;
	//FILE * pFile = fopen("blobDec.txt", "w");
	int width = pImg->width;
	int heigth = pImg->height;

	vector<connection> links;

	// Primo pixel
	if (fgnd.at<uchar>(0,0) > 128)
	{
		tagNum++;
		tagImg[0][0] = tagNum;
	}

	// Prima riga
	for (int x=1; x<width; x++)
	{

		// Se il pixel è foreground
		if (fgnd.at<uchar>(0,x) > 128)
		{

			// Se il pixel precedente è foreground
			if (fgnd.at<uchar>(0,x-1) > 128)
			{
				tagImg[x][0] = tagImg[x-1][0];
				//tagImg.at<uchar>(0, x) = tagImg.at<uchar>(0, x-1);
			}

			// Altrimenti genero un blob nuovo
			else
			{
				tagNum++;
				tagImg[x][0] = tagNum;
				//tagImg.at<uchar>(0, x) = tagNum;

			}
		}
	}


	// TODO: C'è qualcosa che non va in questo pezzo di codice
	for(int y=1; y<heigth; y++)
	{
		for (int x=0; x<width; x++)
		{
			// Se (y,x) è foreground
			if (fgnd.at<uchar>(y, x) > 128)
			{
				//			controlla i contorni sx e sopra di tagImg.at<uchar>(y, x) e se c'è foreground mette il valore del tag in aux
				vector<int> aux;
				if (fgnd.at<uchar>(y-1,x-1) > 128)
				{
					aux.push_back(tagImg[x-1][y-1]);
					//aux.push_back(tagImg.at<uchar>(y-1, x-1));// iRet = tag.at<uchar>(y-1, x-1);
				}
				if (fgnd.at<uchar>(y-1,x) > 128)
				{
					aux.push_back(tagImg[x][y-1]);
					//aux.push_back(tagImg.at<uchar>(y-1, x));
				}
				if (fgnd.at<uchar>(y-1, x+1) > 128)
				{
					aux.push_back(tagImg[x+1][y-1]);
					//aux.push_back(tagImg.at<uchar>(y-1, x+1));
				}
				if (x > 0)
				{
					if (fgnd.at<uchar>(y, x-1) > 128)
					{
						aux.push_back(tagImg[x-1][y]);
						//aux.push_back(tagImg.at<uchar>(y, x-1));
					}
				}
				// Se non ci sono foreground confinanti creo un nuovo tag.
				if (aux.size() == 0)
				{
					tagNum++;
					tagImg[x][y] = tagNum;
					//tagImg.at<uchar>(y,x) = tagNum;
				}

				// Altrimenti prendo il tag + piccolo
				else
				{
					int minTag;
					for (int i=0; i< aux.size(); i++)
					{
						if (i==0)
						{
							minTag = aux[i];
							continue;
						}
						if ( minTag > aux[i])
						{
							// Creare una traccia che minTag e aux[i] devono essere fusi nello stesso blob
							minTag = aux[i];
						}

					}
					tagImg[x][y] = minTag;
					//tagImg.at<uchar>(y,x) = minTag;
				}
			}
		}
	}

	// Cerco il punto più in alto a sinistra
	int tooSmall = 0;
	for (int i=1; i< tagNum; i++)
	{
		float xx = width;
		float yy = heigth;
		float ww = 0;
		float hh = 0;

		for(int y=1; y<heigth; y++)
		{
			for (int x=1; x<width; x++)
			{
				if (tagImg[x][y] == i)
				{
					if (x < xx)
						xx = x;
					if (y < yy)
						yy = y;
					if (x > ww)
						ww = x;
					if (y > hh)
						hh = y;
				}
			}

		}
		// TODO: controllare le connessioni dei tag, se un tag è connesso ad un altro: eliminare il tag maggiore.
		// Aggiungo il cvBlob in pNewBlobList
		//if ((ww-xx > 0.066 * width) && (hh-yy > 0.1 * heigth))
		if ((ww-xx > tolW * width) && (hh-yy > tolH * heigth))
		{
			// Coordinate del centro.
			int centroX = (int) (xx + (ww-xx)/2);
			int centroY = (int) (yy + (hh-yy)/2);
			CvBlob NewBlob = cvBlob(centroX, centroY, (ww-xx), (hh-yy));
			pNewBlobList->AddBlob(&NewBlob);
		}
		else
			tooSmall++;
	}
//	FILE * pFile = fopen("debug.txt", "w");
//	fprintf(pFile, "TagNum = %d\n", tagNum);
//	for(int y=1; y<heigth; y++)
//	{
//		for (int x=1; x<width; x++)
//		{
//			fprintf(pFile, "%d ", tagImg[x][y]);
//		}
//		fprintf(pFile, "\n");
//	}
//	fclose(pFile);
//	//namedWindow("Debug", 1);
//	//imshow("Debug", tagImg);
//	waitKey();
	return tagNum - tooSmall -1;
}

void clnf::Release(){
	delete this;
}

CvBlobDetector* CreateClnf(){
	return (CvBlobDetector*) new clnf();
}

CvBlobDetector* CreateClusterSearch(){
	return (CvBlobDetector*) new clusterSearch();
}

}
}

