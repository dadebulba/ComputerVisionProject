/*
 * mmBlobDetector.cpp
 *
 *  Created on: Mar 22, 2013
 *      Author: matteo
 */
#include "../mmLib.h"


namespace mmLib {
namespace mmModules{
mmBlob::mmBlob(){
	coordX = 0;
	coordY = 0;
	width = 10;
	height = 10;
	image = blobBuilder(Mat());
	depth = 0;
	isFgSet = false;
	dispVect = Point(0,0);
	type = STANDARD;
	blobID = -1;
	assignedMSE = -1;
}

mmBlob::mmBlob(Mat frame,Point tl, Size sz) {
	coordX = tl.x;
	coordY = tl.y;
	width = sz.width;
	height = sz.height;
	image = blobBuilder(frame);
	depth = 0;
	isFgSet = false;
	dispVect = Point(0,0);
	type = STANDARD;
	blobID = -1;
	assignedMSE = -1;
	frame.release();
}

mmBlob::mmBlob(Mat frame,int x, int y, int w, int h){
	coordX = x;
	coordY = y;
	width = w;
	height = h;
	image = blobBuilder(frame);
	depth = 0;
	isFgSet = false;
	dispVect = Point(0,0);
	type = STANDARD;
	blobID = -1;
	assignedMSE = -1;
	frame.release();
}

mmBlob::mmBlob(Mat frame, vector<Point> blobPoints){
	int tlX, tlY, brX, brY;
	for (int i=0; i<blobPoints.size(); i++)	{
		Point pt = blobPoints[i];
		if (i == 0)	{
			tlX = pt.x;
			tlY = pt.y;
			brX = pt.x;
			brY = pt.y;
			continue;
		}

		if (tlX > pt.x)
			tlX = pt.x;

		if (tlY > pt.y)
			tlY = pt.y;

		if (brX < pt.x)
			brX = pt.x;

		if (brY < pt.y)
			brY = pt.y;

	}
	width = abs(brX - tlX);
	height = abs(tlY - brY);
	coordX = tlX;
	coordY = tlY;
	image = blobBuilder(frame);
	depth = 0;
	isFgSet = false;
	dispVect = Point(0,0);
	type = STANDARD;
	blobID = -1;
	assignedMSE = -1;
	pointList = blobPoints;
	blobPoints.clear();
	frame.release();
}

mmBlob::~mmBlob(){
	image.release();
	foreground.release();
	M.release();
	prob.release();
	pointList.clear();
	centList.clear();
	bindedKalman.clear();
}

Mat mmBlob::blobBuilder(Mat frame){
	Size s = frame.size();
	int x = coordX;
	int y = coordY;
	int w = width;
	int h = height;
	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;
	if(x + w > s.width)
		w = s.width - x;
	if(y + h > s.height)
		h = s.height - y;
	Rect roi(x, y, w, h);
	Mat aux;
	frame.copyTo(aux);
	Mat blobMat(aux, roi);
	frame.release();
	aux.release();

	return blobMat;
}

Point mmBlob::centroid(){
	Point center;
	center = Point(coordX+width/2, coordY+height/2);
	return center;
}

Rect mmBlob::boundingBoxProcessed(){
	Rect blobRect;
	blobRect = Rect(coordX, coordY, width, height);
	return blobRect;
}

Point mmBlob::getTL(){
	Point tl = Point(coordX, coordY);
	return tl;
}

Point mmBlob::getBR(){
	Point br = Point(coordX, coordY);
	br = br + Point(width, height);
	return br;
}

Mat mmBlob::getImage(){
	return image;
}

Mat mmBlob::getForeground(){
	Mat result = Mat();

	if(isFgSet)
		result = foreground;

	return result;
}

void mmBlob::setForeground(Mat f){
	isFgSet = true;
	foreground = f;
}

int mmBlob::getBlobArea(){
	return height * width;
}

int mmBlob::getWidth(){
	return width;
}

int mmBlob::getHeight(){
	return height;
}

int mmBlob::getDepth(){
	return depth;
}

void mmBlob::setDepth(int d){
	depth = d;
}

Point mmBlob::getDispVect(){
	return dispVect;
}

void mmBlob::setDispVect(Point p){
	dispVect = p;
}

vector<Point> mmBlob::getPointList(){
	return pointList;
}

void mmBlob::setPointList(vector<Point> pList){
	pointList = pList;
}

vector<Point> mmBlob::getCentList(){
	return centList;
}

void mmBlob::setCentList(vector<Point> pList){
	centList = pList;
}

Mat mmBlob::getM(){
	return M;
}

void mmBlob::setM(Mat mat){
	M = mat;
}

Mat mmBlob::getP(){
	return prob;
}

void mmBlob::setP(Mat mat){
	prob = mat;
}

BLOB_TYPE mmBlob::getType(){
	return type;
}

void mmBlob::setType(BLOB_TYPE ty){
	type = ty;
}

Scalar mmBlob::getColor(){
	return color;
}

void mmBlob::setColor(Scalar col){
	color = col;
}

void mmBlob::pushPoint(Point p){
	pointList.push_back(p);
}

int getDist(mmBlob blob1, mmBlob blob2){
	return sqrt((blob1.centroid().x - blob2.centroid().x)*(blob1.centroid().x - blob2.centroid().x) + (blob1.centroid().y - blob2.centroid().y)*(blob1.centroid().y - blob2.centroid().y));
}

Scalar assignRandomColor(){
	Scalar col;

	int red = rand()%255;
	int green = rand()%255;
	int blu = rand()%255;

	col = Scalar(red,green,blu);
	return col;
}

double getPointDist(Point p1, Point p2){
	return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
}

double getDistFromBlob(mmBlob blob,Point p){
	return sqrt((blob.centroid().x-p.x)*(blob.centroid().x-p.x)+(blob.centroid().y-p.y)*(blob.centroid().y-p.y));
}

Mat mmDrawBlobs(Mat img, vector<mmBlob> blobList){
	Mat iOut;
	img.copyTo(iOut);
	int s = blobList.size();
	for(int i=0; i<s; i++)	{
		mmBlob istance = blobList[i];
		Rect r = istance.boundingBoxProcessed();
		rectangle(iOut, r, Scalar(128,128,10), 1);
	}
	img.release();	blobList.clear();
	return iOut;
}

Mat mmDrawBlobsAndPoints(Mat img, Mat foreground, vector<mmBlob> blobList){
	Mat iOut;
	img.copyTo(iOut);
	img.release();
	int s = blobList.size();

	for(int i=0; i< s; i++)	{
		mmBlob istance = blobList[i];
		Rect rect = istance.boundingBoxProcessed();
		Mat fg;
		fg = Mat(foreground,rect);

		if(istance.getPointList().empty()){
			for(int r=0; r<istance.getHeight(); r++){
				for (int c=0; c<istance.getWidth(); c++){

					if(fg.at<uchar>(r,c)!=(uchar)0){
						Point p = Point(c+istance.getTL().x,r+istance.getTL().y);
						circle(iOut,p,0.5,istance.getColor());
					}
				}
			}
		}
		else{
			vector<Point> pL = istance.getPointList();
			for(int j=0; j<pL.size(); j++){
				Point p = pL[j];
				circle(iOut,p,0.5,istance.getColor());
			}
		}
		rectangle(iOut, rect, istance.getColor(), 1);
		fg.release();
	}
	blobList.clear();
	foreground.release();
	return iOut;
}

Mat mmPrintTrack(Mat img, vector<mmBlob> blobList){
	Mat iOut;
	img.copyTo(iOut);
	img.release();
	int s = blobList.size();
	for (int i=0; i<s; i++){
		mmBlob blob = blobList[i];
		int l = blob.getCentList().size();
		for (int j=0; j<l; j++){
			Point p = blob.getCentList()[j];
			circle(iOut,p,1,blob.getColor());
		}
	}
	blobList.clear();
	return iOut;
}

bool mmBlob::isConteined(Point pt){
	bool bRet = false;
	if ((pt.x > coordX) && (pt.x < (coordX + width)) && (pt.y > coordY) && (pt.y < (coordY + height))){
		bRet = true;
	}
	return bRet;
}

// Crea bins per istogrammi - float* histBlue e gli altri vanno allocati esternamente come vettori da 256 float
void mmBlob::getBins(float* histBlue, float* histGreen, float* histRed){

	Mat blobMat;
	image.copyTo(blobMat);
	Mat red;
	Mat blue;
	Mat green;
	// BGR
	vector<Mat> channels;
	split(blobMat, channels);
	blue = channels[0];
	green = channels[1];
	red = channels[2];

	for (int j = 0; j<256; j++)
	{
		histRed[j] = 0;
		histBlue[j] = 0;
		histGreen[j] = 0;
	}
	for (int x=0; x<blobMat.size().width; x++)
	{
		for (int y=0; y<blobMat.size().height; y++)
		{
			histBlue[(int)blue.at<uchar>(y, x)] = histBlue[(int)blue.at<uchar>(y, x)] + 1;
			histGreen[(int)green.at<uchar>(y, x)] = 	histGreen[(int)green.at<uchar>(y, x) ]+ 1;
			histRed[(int)red.at<uchar>(y, x)] = histRed[(int)red.at<uchar>(y, x)] + 1;
		}
	}
	int sum = blobMat.size().height * blobMat.size().width;

}

void mmShowHistogram(float* hist, String wName){
	Mat draw(198, 352, CV_8UC1);
	for (int x=0; x<draw.size().width; x++)
	{
		for (int y=0; y<draw.size().height; y++)
		{
			draw.at<uchar>(y,x) = 255;
		}
	}

	// disegno l'asse y
	int lengthY = 0;
	for (int y = 11; y<draw.size().height - 11; y++)
	{
		draw.at<uchar>(y, 48) = 0;
		lengthY++;
	}

	// disegno l'asse x
	for (int x = 48; x<draw.size().width - 48; x++)
	{
		draw.at<uchar>(draw.size().height - 11, x) = 0;
	}

	// find max bin
	float maximo = 0;
	for (int i = 0; i< 256; i++)
	{
		if (maximo < hist[i])
			maximo = hist[i];
	}
	// normalize the bins on the max bin
	for (int i = 0; i< 256; i++)
	{
		hist[i] = hist[i] / maximo;
	}

	// draw bins
	for (int i = 0; i< 256; i++)
	{
		int base = draw.size().height - 11;
		for (int j = 0; j<lengthY * hist[i]; j++)
		{
			draw.at<uchar>(base - j, i + 49) = 0;
		}
	}
	namedWindow(wName, 0);
	imshow(wName, draw);

}

float mmHistogramMatchingMSE(float* histA, float* histB){
	int maxLength = 256;
	float mse  = 0;
	for (int i=0; i<maxLength; i++)
	{
		float aux;
		aux = histA[i] - histB[i];
		aux = aux * aux;
		mse += aux;
	}
	mse = mse / maxLength;
	return mse;
}

//float mmFullHistMatching(Mat a, Mat b){
//	float* rA = new float[256];
//	float* gA = new float[256];
//	float* bA = new float[256];
//	float* rB = new float[256];
//	float* gB = new float[256];
//	float* bB = new float[256];
//
//	float* histRed = new float[256];
//	float* histBlue = new float[256];
//	float* histGreen = new float[256];
//
//
//	Mat red;
//	Mat blue;
//	Mat green;
//	// BGR
//	vector<Mat> channels;
//	// Matrice a
//	split(a, channels);
//	blue = channels[0];
//	green = channels[1];
//	red = channels[2];
//
//	for (int j = 0; j<256; j++)
//	{
//		histRed[j] = 0;
//		histBlue[j] = 0;
//		histGreen[j] = 0;
//	}
//	for (int x=0; x<a.size().width; x++)
//	{
//		for (int y=0; y<a.size().height; y++)
//		{
//			histBlue[(int)blue.at<uchar>(y, x)] = histBlue[(int)blue.at<uchar>(y, x)] + 1;
//			histGreen[(int)green.at<uchar>(y, x)] = histGreen[(int)green.at<uchar>(y, x) ]+ 1;
//			histRed[(int)red.at<uchar>(y, x)] = histRed[(int)red.at<uchar>(y, x)] + 1;
//		}
//	}
//	rA = histRed;
//	bA = histBlue;
//	gA = histGreen;
//	// Matrice b
//	split(b, channels);
//	blue = channels[0];
//	green = channels[1];
//	red = channels[2];
//
//	for (int j = 0; j<256; j++)
//	{
//		histRed[j] = 0;
//		histBlue[j] = 0;
//		histGreen[j] = 0;
//	}
//	for (int x=0; x<a.size().width; x++)
//	{
//		for (int y=0; y<a.size().height; y++)
//		{
//			histBlue[(int)blue.at<uchar>(y, x)] = histBlue[(int)blue.at<uchar>(y, x)] + 1;
//			histGreen[(int)green.at<uchar>(y, x)] = 	histGreen[(int)green.at<uchar>(y, x) ]+ 1;
//			histRed[(int)red.at<uchar>(y, x)] = histRed[(int)red.at<uchar>(y, x)] + 1;
//		}
//	}
//	rB = histRed;
//	bB = histBlue;
//	gB = histGreen;
//
//
//	float mseBlue = mmHistogramMatchingMSE(bA, bB);
//	float mseGreen = mmHistogramMatchingMSE(gA, gB);
//	float mseRed = mmHistogramMatchingMSE(rA, rB);
//	float mse = mseBlue * 0.25 + mseRed * 0.25 + mseGreen * 0.5;
//
//	return mse;
//}

float test_matching(Mat matA, Mat matB)
{
	float result=0;
	float matchBlue, matchGreen, matchRed;
	float* blueA = new float[256];
	float* redA = new float[256];
	float* greenA = new float[256];
	float* blueB = new float[256];
	float* redB = new float[256];
	float* greenB = new float[256];
//
//	imshow("A", matA);
//	imshow("B", matB);

	my_hist (matA, blueA, redA, greenA);
	my_hist (matB, blueB, redB, greenB);


	matchRed= mmHistogramMatchingMSE(redA, redB);
	matchGreen= mmHistogramMatchingMSE(greenA, greenB);
	matchBlue= mmHistogramMatchingMSE(blueA, blueB);
	result= matchRed*0.25 + matchGreen*0.50 + matchBlue*0.25;
	return result;

}
void my_hist (Mat image, float* histBlue, float* histGreen, float* histRed)
{

	Mat red;
	Mat blue;
	Mat green;
	// BGR
	vector<Mat> channels;
	split(image, channels);
	blue = channels[0];
	green = channels[1];
	red = channels[2];

	for (int j = 0; j<256; j++)
	{
		histRed[j] = 0;
		histBlue[j] = 0;
		histGreen[j] = 0;
	}
	for (int x=0; x<image.size().width; x++)
	{
		for (int y=0; y<image.size().height; y++)
		{
			histBlue[(int)blue.at<uchar>(y, x)] = histBlue[(int)blue.at<uchar>(y, x)] + 1;
			histGreen[(int)green.at<uchar>(y, x)] = histGreen[(int)green.at<uchar>(y, x) ]+ 1;
			histRed[(int)red.at<uchar>(y, x)] = histRed[(int)red.at<uchar>(y, x)] + 1;
		}
	}

}

float HistogramMatching(float* histA, float* histB)
{
	int maxLength = 256;
	float mse  = 0;
	int th=2;
	int a[256],b[256], c[256];
	for (int j=0; j<maxLength; j++)
	{
		if (histA[j]>th)
			a[j]=1;
		else
			a[j]=0;
	}
	for (int j=0; j<maxLength; j++)
	{
		if (histB[j]>th)
			b[j]=1;
		else
			b[j]=0;
	}

	for (int j=0; j<maxLength; j++)
	{

		c[j]=a[j] * b[j];
		if (c[j]==1)
			mse++;
	}

	return 1/mse;
}

float test_matching_v2(Mat matA, Mat backgroundA, Mat matB, Mat backgroundB)
{
	float result=0;
	float matchBlue, matchGreen, matchRed;
	float* blueA = new float[256];
	float* redA = new float[256];
	float* greenA = new float[256];
	float* blueB = new float[256];
	float* redB = new float[256];
	float* greenB = new float[256];

	HistogramMatching_v2 (matA,backgroundA, blueA, redA, greenA);
	HistogramMatching_v2 (matB,backgroundB, blueB, redB, greenB);


	matchRed= mmHistogramMatchingMSE(redA, redB);
	matchGreen= mmHistogramMatchingMSE(greenA, greenB);
	matchBlue= mmHistogramMatchingMSE(blueA, blueB);
	result= matchRed*0.25 + matchGreen*0.50 + matchBlue*0.25;
	return result;

}
void HistogramMatching_v2 (Mat a, Mat b, float * Hred, float * Hblue, float * Hgreen)
{

	Mat red;
	Mat blue;
	Mat green;
	// BGR
	vector<Mat> channels;
	split(a, channels);
	blue = channels[0];
	green = channels[1];
	red = channels[2];

	for (int j = 0; j<256; j++)
	{
		Hred[j] = 0;
		Hblue[j] = 0;
		Hgreen[j] = 0;
	}

	for (int x=0; x<a.size().width; x++)
	{
		for (int y=0; y<a.size().height; y++)
		{
			if ((int)b.at<uchar>(y, x)==255)
			{

				Hblue[(int)blue.at<uchar>(y, x)] = Hblue[(int)blue.at<uchar>(y, x)] + 1;
				Hgreen[(int)green.at<uchar>(y, x)] = Hgreen[(int)green.at<uchar>(y, x) ]+ 1;
				Hred[(int)red.at<uchar>(y, x)] = Hred[(int)red.at<uchar>(y, x)] + 1;
			}
		}
	}

	red.release();
	blue.release();
	green.release();
}

//MatND mmBlob::histogram(){
//
//	// Visualizza il blob
//	namedWindow("Blob", 0);
//	//imshow("Blob", blobMat);
//
//	// creo l'histogram
//	MatND hist;
//	Mat hsv;
//	cvtColor(img, hsv, CV_BGR2HSV);
//	int channels[] = {0, 1, 2};
//	int hbins = 100, sbins = 100;
//	int histSize[] = {hbins, sbins};
//	float hranges[] = { 0, 180 };
//	float sranges[] = { 0, 256 };
//	const float* ranges[] = { hranges, sranges};
//	calcHist(&img, 1, channels, Mat(), hist, 2, histSize, ranges, true, false);
//	imshow("Blob", hist);
//	return hist;
//
//}
}
}
