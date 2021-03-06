/*
 * mmBlobDetector.cpp
 *
 *  Created on: Jun 11, 2013
 *      Author: matteo
 */

#include "../mmLib.h"

using namespace std;
using namespace cv;

bool mm_isPointContRect(Rect r, Point p){
	if (p.x >= r.x && p.x <= (r.x+r.width) && p.y>= r.y && p.y <= (r.y+r.height)){
		return true;
	}
	else{
		return false;
	}
}

namespace mmLib{
namespace mmAnomaly{
mmGaussianModelSettings::mmGaussianModelSettings(){
	numGaussianParam = 3;
	matchFactorParam = 2.5;
	lrweightParam = 0.001;
	lrvarianceParam = 0.001;
	highVarianceParam = 150;
	lowWeigthParam = 0.001;
	tProvaParam = 0.5;

	varianceParam = 1;
	meanValueParam = 0;
	weightParam = 0;

	mWidthParam = 320;
	mHeightParam = 240;
	heightParam = 25;
	widthParam = 35;
	threshParam = 6;
	numAnomalyParam = 3;
	numNormalParam = 2;
}

mmGaussianModel::mmGaussianModel(){
	numGaussian = 3;
	matchFactor = 2.5;
	lrweight = 0.001;
	lrvariance = 0.001;
	highVariance = 150;
	lowWeigth = 0.001;
	tProva = 0.5;

	variance = 1;
	meanValue = 0;
	weight = 0;

	mWidth = 320;
	mHeight = 240;
	height = 25;
	width = 35;
	thresh = 6;
	numAnomaly = 3;
	numNormal = 2;

	isAnomaly = false;
	firstFrame = true;
	countFrame=0;
	initializeModel();
}

mmGaussianModel::mmGaussianModel(mmGaussianModelSettings &gmSets){
	numGaussian = gmSets.numGaussianParam; //number of gaussian in the model
	matchFactor = gmSets.matchFactorParam; //variable for gaussian matching
	lrweight = gmSets.lrweightParam; //gaussian weight
	lrvariance = gmSets.lrvarianceParam;//gaussian variance
	highVariance = gmSets.highVarianceParam; //gaussian variance for a new gaussian
	lowWeigth = gmSets.lowWeigthParam; //gaussian weight for a new gaussian
	tProva = gmSets.tProvaParam; //variable for a new gaussian

	variance = gmSets.varianceParam; //initial gaussian variance
	meanValue = gmSets.meanValueParam; //initial gaussian mean value
	weight = gmSets.weightParam; //initial gaussian weight

	mWidth = gmSets.mWidthParam; //frame width
	mHeight = gmSets.mHeightParam; //frame height
	height = gmSets.heightParam; //analysis window height
	width = gmSets.widthParam; //analysis window width
	thresh = gmSets.threshParam; //number of abnormal particle to consider the frame as abnormal
	numAnomaly = gmSets.numAnomalyParam; //number of abnormal consecutive frames to raise the alarm
	numNormal = gmSets.numNormalParam; //number of normal consecutive frames to raise down the alarm

	isAnomaly = false;
	firstFrame = true;
	countFrame=0;
	initializeModel();
}

mmGaussianModel::~mmGaussianModel(){
	particleVect.clear();
	if(!firstFrame)	{
		for(int i=0; i<mWidth*mHeight; i++)	{
			gaussian[i].erase(gaussian[i].begin(),gaussian[i].end());
		}
		gaussian->clear();
		delete [] gaussian;
	}
}

//For each pixel create numGaussian gaussian
void mmGaussianModel::initializeModel(){
	gaussianPixel gPixel;
	gPixel.variance = variance;
	gPixel.meanValue = meanValue;
	gPixel.weight = weight;

	gaussian = new vector<gaussianPixel>[mWidth*mHeight];

	for(int i=0; i<mWidth; i++)	{
		for(int j=0; j<mHeight; j++){
			for(int k=0;k<numGaussian; k++)	{
				int index = j*mWidth+i;
				gaussian[index].push_back(gPixel);
			}
		}
	}
	swap = gPixel;
	firstFrame = false;
}

/*
 * Every 3 frame calc the Euclidean distance between first and third frame points displacements
 * img: image frame
 * VelocityImg: empty image (used for output)
 * vectPoints: vector of 2D points containing the calculated new positions of input features in the second image
 * tempPoints: vector used to store points coordinate when numFrameInit is 0
 * status:	each element of the vector is set to 1 if the flow for the corresponding features has been found, 0 otherwise
 */
void mmGaussianModel::MagnitudeFunt(Mat img, Mat& VelocityImg, vector<Point2f> vectPoints, vector<Point2f>& tempPoints, vector<uchar> status, int& numFrameInit, bool& isInit){
	if(numFrameInit == 0){
		for(int m=0; m<vectPoints.size(); m++){
			tempPoints.push_back(vectPoints[m]);
		}
	}
	else if (numFrameInit == 3)	{
		float dist=0;

		for(int m=0; m<vectPoints.size(); m++){
			//check status is 1 and then check if vectPoints and tempPoints are bounded between in image size
			if( status[m] && (vectPoints[m].x >= 0 && vectPoints[m].y >=0 && tempPoints[m].x >=0 && tempPoints[m].y >=0) && (vectPoints[m].x < img.cols && vectPoints[m].y < img.rows && tempPoints[m].x < img.cols && tempPoints[m].y < img.rows)){
				//calc euclidean distance between tempPoints and vectPoints
				dist = sqrt(pow(abs(tempPoints[m].x - vectPoints[m].x),2)+pow(abs(tempPoints[m].y - vectPoints[m].y),2));

				int x = (int)vectPoints[m].x;
				int y = (int)vectPoints[m].y;
				int value = (int)dist;

				//if distance is greater than 8 pixel then save it in velocity image
				if(dist > 8)
					VelocityImg.at<uchar>(y,x)=value;
				else
					VelocityImg.at<uchar>(y,x)=0;
			}
		}
		numFrameInit = 0;
		isInit = true;
	}
	++numFrameInit;

	img.release();  vectPoints.clear(); status.clear();
}

/*
 * frame: image frame
 * VelocityImage: vector containing distances computed between couple of points
 * background: prev background, first is empty
 */
Mat mmGaussianModel::updateBackgroundModel(Mat& frame, Mat velocityImage, Mat background){
	//put background into temp
	Mat temp = Mat(background.rows,background.cols,CV_8UC3,cv::Scalar(0));
	temp = background.clone();

	bool isBG = false;
	for (int i=0;i<mHeight;i++)	{
		for (int j=0;j<mWidth;j++){
			//if there is a distance value at position i,j in velocityImage
			if((int)velocityImage.at<uchar>(i,j) != 0){
				int r= (int)velocityImage.at<uchar>(i,j);

				//check gaussian matching the value in velocityImage[i,j]
				isBG = findMatchGaussian(Point(j,i),r);

				if(isBG){
					temp.at<Vec3b>(i,j)[2] = 0;
					temp.at<Vec3b>(i,j)[1] = 0;
					temp.at<Vec3b>(i,j)[0] = 0;
				}
				else{
					temp.at<Vec3b>(i,j)[2] = 255;
					temp.at<Vec3b>(i,j)[1] = 255;
					temp.at<Vec3b>(i,j)[0] = 255;

					//Draw the red points inside the original frame that is part of foreground
					Point2f Singlept;
					Singlept.y = i;
					Singlept.x = j;
					//particle vect is an array containing foreground points
					particleVect.push_back(Singlept);
					circle( frame, Singlept, 2, Scalar(0,0,255), -1, 8);
					//getchar();
				}
			}
			else{
				temp.at<Vec3b>(i,j)[2] = 0;
				temp.at<Vec3b>(i,j)[1] = 0;
				temp.at<Vec3b>(i,j)[0] = 0;
			}
		}
	}

	velocityImage.release(); background.release();
	return temp;
}

bool mmGaussianModel::findMatchGaussian(Point p, int r){
	int index = p.y*mWidth + p.x;
	sortGD(index);

	bool result = false;
	bool isMatch = false;
	int indexI = 0;

	//iterate through all the gaussian distributions in the container and find the first match
	for (int i=0; i<numGaussian && !isMatch; i++){
		double dist = sqrt(pow((r-gaussian[index][i].meanValue),2));
		if (dist <= matchFactor*(sqrt(gaussian[index][i].variance))){
			isMatch = true;
			indexI = i;
		}
	}
	matchedIndex = indexI;

	if(isMatch){
		result = checkBackground(index);
		updateGaussian(index,indexI,r,lrvariance);
	}
	else{
		// get least probable distribution and replace that gaussian distribution such that mean = current pixel and highvariance
		int indexGauss = getLPGD(index);
		replaceGD(index, indexGauss, r, highVariance, lowWeigth);
	}
	// adjust weights
	for(int i=0; i<numGaussian; i++){
		//for matched gaussian distribution
		if(indexI == i && isMatch == true)
			adjustWeight(index,i,lrweight,true);
		//for unmatched gaussian distribution
		else
			adjustWeight(index,i,lrweight,false);
	}

	return result;
}

//sort according to weight/variance.
void mmGaussianModel::sortGD(int index){
	for (int i=0; i<numGaussian-1; i++)	{
		for (int j=0; j<numGaussian-1-i; j++){
			if (getWeightVarianceRatio(index,j+1) > getWeightVarianceRatio(index,j)){
				swap = gaussian[index][j];
				gaussian[index][j] = gaussian[index][j+1];
				gaussian[index][j+1] = swap;
			}
		}
	}
}

double mmGaussianModel::getWeightVarianceRatio(int index, int indexGauss){
	return (gaussian[index][indexGauss].weight/gaussian[index][indexGauss].variance);
}
//Use MixtureOfGaussian method to determine if is in background or foreground
bool mmGaussianModel::checkBackground(int index){
	int i;
	double sumWeights = 0;
	int backgroundIndex = 0;

	for(i=0; i<numGaussian; i++){
		sumWeights += gaussian[index][i].weight;
		if(sumWeights > tProva){
			backgroundIndex = i;
			break;
		}
	}
	if(matchedIndex <= backgroundIndex)
		BG = true;
	else
		BG = false;

	return BG;
}

void mmGaussianModel::updateGaussian(int index, int indexGauss, int r, double learningRate){
	gaussian[index][indexGauss].meanValue = ((1-learningRate)*gaussian[index][indexGauss].meanValue) + (learningRate*r);
	gaussian[index][indexGauss].variance = ((1-learningRate)*gaussian[index][indexGauss].variance)+(learningRate*pow((r-gaussian[index][indexGauss].meanValue),2));
}

int mmGaussianModel::getLPGD(int index){
	double value = getWeightVarianceRatio(index,0);
	int tempIndex = 0;

	for (int i=1;i<numGaussian;i++){
		double temp = getWeightVarianceRatio(index,i);
		if (temp < value){
			value = temp;
			tempIndex=i;
		}
	}
	return tempIndex;
}

void mmGaussianModel::replaceGD(int index, int indexGauss, double r, double variance, double weight){
	gaussian[index][indexGauss].variance = variance;
	gaussian[index][indexGauss].meanValue = r;
	gaussian[index][indexGauss].weight = weight;
}

void mmGaussianModel::adjustWeight(int index, int indexGauss, double learningRate, bool matched){
	if(matched)
		gaussian[index][indexGauss].weight = (1-learningRate)*gaussian[index][indexGauss].weight+learningRate;
	else
		gaussian[index][indexGauss].weight = (1-learningRate)*gaussian[index][indexGauss].weight;
}

int mmGaussianModel::findAnomaly(Mat frame){
	Rect r;
	int i = 0;
	int count;

	if (isAnomaly)
		countFrame--;

	int countFirst = countFrame;

	while(i<particleVect.size() && !particleVect.empty()){
		int tx = (particleVect[i].x-ceil(width/2)>=0)?((particleVect[i].x-ceil(width/2)<frame.cols)?particleVect[i].x-ceil(width/2):frame.cols-1):0;
		int ty = (particleVect[i].y-ceil(height/2)>=0)?((particleVect[i].y-ceil(height/2)<frame.rows)?particleVect[i].y-ceil(height/2):frame.rows-1):0;
		int bx = (particleVect[i].x+ceil(width/2)>=0)?((particleVect[i].x+ceil(width/2)<frame.cols)?particleVect[i].x+ceil(width/2):frame.cols-1):0;
		int by = (particleVect[i].y+ceil(height/2)>=0)?((particleVect[i].y+ceil(height/2)<frame.rows)?particleVect[i].y+ceil(height/2):frame.rows-1):0;
		r = Rect(tx,ty,(bx-tx),(by-tx));
		count = 0;

		for (int j=0; j<particleVect.size(); j++){
			if (particleVect[j].x > r.x && particleVect[j].x < r.x+r.width && particleVect[j].y > r.y && particleVect[j].y < r.y+r.height)
				count++;
		}
		if (count>thresh){
			if (isAnomaly)
				countFrame = numNormal;
			else
				countFrame++;
			i = particleVect.size();
		}

		i++;
	}

	if (isAnomaly){
		if (countFrame == 0)
			isAnomaly = false;
	}
	else{
		if (countFrame >= numAnomaly){
			isAnomaly = true;
			countFrame = numNormal;
		}
		else if (countFirst == countFrame)
			countFrame = 0;
	}

	frame.release(); particleVect.clear();
	return count;
}


void mmGaussianModel::findAnomalyInsideRect(Mat frame,Rect anomalyRect){
	Rect r;
	int i = 0;
	int count = 0;

	if (isAnomaly)
		countFrame--;

	int countFirst = countFrame;

	while(i<particleVect.size() && !particleVect.empty()){
		if(mm_isPointContRect(anomalyRect,particleVect[i]))
			count++;

		if (count>thresh){
			if (isAnomaly)
				countFrame = numNormal;
			else
				countFrame++;

			i = particleVect.size();
		}

		i++;
	}

	if (isAnomaly){
		if (countFrame == 0)
			isAnomaly = false;
	}
	else{
		if (countFrame >= numAnomaly){
			isAnomaly = true;
			countFrame = numNormal;
		}
		else if (countFirst == countFrame)
			countFrame = 0;
	}

	frame.release(); particleVect.clear();
}

bool mmGaussianModel::getAnomaly(){
	return isAnomaly;
}
}
}
