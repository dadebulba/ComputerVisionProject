#include "../mmLib.h"

namespace mmLib{
namespace mmAnomaly{
mmParticleEnergySettings::mmParticleEnergySettings(){
	surveilanceRectDimParam = Size(40,40);
	filterSizeParam = Size(31,31);
	surroundingAreaParam = Size(75,60);
	sigmaParam = 0.05;
	threshHighParam = 20;
	threshLowParam = 20;
	valuePeriodParam = 1000;
	profilePeriodParam = 1000;
	profileFrameStartParam = 0;
	profileLengthParam = 1000;
	trackingPeriodParam = 50;
}

mmParticleEnergy::mmParticleEnergy(){
	surveilanceRectDim = Size(40,40);
	filterSize = Size(31,31);
	surroundingArea = Size(75,60);
	sigma = 0.05;
	threshHigh = 20;
	threshLow = 20;
	valuePeriod = 1000;
	profilePeriod = 1000;
	profileFrameStart = 0;
	profileLength = 1000;
	trackingPeriod = 50;
	countFrame = 0;
	totalFrameProcessed = -1;
	profileEnIndex = 0;
	firstFrame = true;
}

mmParticleEnergy::mmParticleEnergy(mmParticleEnergySettings &peSet){
	profileEnVect = peSet.profileEnVectParam; //refernce enrgy vector
	surveilanceRect = peSet.surveilanceRectParam; //surveillance area
	surveilanceRectDim = peSet.surveilanceRectDimParam; //dimension of the rectangles' surveillance area needed if surroundingAreaParam is not set
	filterSize = peSet.filterSizeParam; //dimension of the gaussian filter
	surroundingArea = peSet.surroundingAreaParam; //area around a particle to store energy
	sigma = peSet.sigmaParam; //variance value for the energy function
	threshHigh = peSet.threshHighParam; //high threshold for anomaly detection
	threshLow = peSet.threshLowParam; //low threshold for anomaly detection
	valuePeriod = peSet.valuePeriodParam; //length of the evaluation period
	profilePeriod = peSet.profilePeriodParam; //length of the reference period
	profileFrameStart = peSet.profileFrameStartParam; //frame number to start storing the reference energy
	profileLength = peSet.profileLengthParam; //frame numbers of the reference period
	trackingPeriod = peSet.trackingPeriodParam; //tracking period
	countFrame = 0;
	totalFrameProcessed = -1;
	profileEnIndex = 0;
	firstFrame = true;
	peSet.profileEnVectParam.clear();
	peSet.surveilanceRectParam.clear();
}

mmParticleEnergy::~mmParticleEnergy(){
	matProfile.release();
	matValue.release();
	profileEnVect.clear();
	valueEnVect.clear();
	surveilanceRect.clear();
}

//initialize module
void mmParticleEnergy::setUpModule(Mat frame){
	if (firstFrame){
		firstFrame = false;
		needToRec = (profileEnVect.size()!=0)?false:true;

		if (surveilanceRect.size() == 0){
			for (int i=0; i<frame.cols; i=i+surveilanceRectDim.width){
				for (int j=0; j<frame.rows; j=j+surveilanceRectDim.height){
					surveilanceRect.push_back(Rect(i,j,surveilanceRectDim.width,surveilanceRectDim.height));
				}
			}
		}
	}


	if(needToRec && totalFrameProcessed%profilePeriod == 0){
		matProfile = Mat::zeros(frame.rows,frame.cols,CV_32FC3);
	}
	if (!needToRec && totalFrameProcessed%valuePeriod == 0){
		matValue = Mat::zeros(frame.rows,frame.cols,CV_32FC3);
		valueEnVect.clear();
	}
	totalFrameProcessed++;
}

//store the reference energy
void mmParticleEnergy::setProfileEnVect(vector<vector<float> > energyVect){
	profileEnVect = energyVect;
}

//clear variables after the rec phase
void mmParticleEnergy::clearVariables(){
	countFrame = 0;
	totalFrameProcessed = -1;
}

//each frame is processed
void mmParticleEnergy::processFrame(Mat frame, vector<Point2f> particles){
	Mat matStuff;
	int period;
	vector<float> energyVectStuff;
	if(needToRec){
		matStuff = matProfile;
		period = profilePeriod;
	}
	else{
		matStuff = matValue;
		period = valuePeriod;
	}
	countFrame++;

	matStuff = computeParticleEnergy(matStuff,particles);
	if (countFrame != 0 && countFrame%(period/trackingPeriod) == 0){
		matStuff = matStuff/countFrame;
		GaussianBlur(matStuff,matStuff,filterSize,0,0, BORDER_DEFAULT);
		energyVectStuff = computeEnergyPerArea(matStuff);
		countFrame = 0;

		if(needToRec){
			matProfile = matStuff;
			profileEnVect.push_back(energyVectStuff);
			needToRec = (totalFrameProcessed==profileFrameStart+profileLength-1)?false:true;
		}
		else{
			matValue = matStuff;
			valueEnVect = energyVectStuff;
		}
	}
	matStuff.release(); energyVectStuff.clear();
}

//compute anomaly
vector<bool> mmParticleEnergy::getAnomaly(){
	vector<bool> anomalyVect;

	for (int i=0; i<valueEnVect.size(); i++){
		if(profileEnVect.size() == 1){
			if (valueEnVect[i]-profileEnVect[0][i]>threshHigh || valueEnVect[i]-profileEnVect[0][i]<-threshLow)
				anomalyVect.push_back(true);
			else
				anomalyVect.push_back(false);
		}
		else{
			if (valueEnVect[i]-profileEnVect[profileEnIndex][i]>threshHigh || valueEnVect[i]-profileEnVect[profileEnIndex][i]<-threshLow)
				anomalyVect.push_back(true);
			else
				anomalyVect.push_back(false);
			profileEnIndex = (profileEnIndex==profileEnVect.size()-1)?0:profileEnIndex+1;
		}
	}
	return anomalyVect;
}

//compute the type of anomaly
vector<mmLib::mmAnomaly::ANOMALY_TYPE> mmParticleEnergy::getAnomalyType(){
	vector<ANOMALY_TYPE> anomalyVect;
	bool oneTime = true;

	for (int i=0; i<valueEnVect.size(); i++){
		if(profileEnVect.size() == 1){
			if (valueEnVect[i]-profileEnVect[0][i]>threshHigh )
				anomalyVect.push_back(HIGHANOMALY);
			else if (valueEnVect[i]-profileEnVect[0][i]<-threshLow)
				anomalyVect.push_back(LOWANOMALY);
			else
				anomalyVect.push_back(STANDARD);
		}
		else{
			if (valueEnVect[i]-profileEnVect[profileEnIndex][i]>threshHigh)
				anomalyVect.push_back(HIGHANOMALY);
			else if (valueEnVect[i]-profileEnVect[profileEnIndex][i]<-threshLow)
				anomalyVect.push_back(LOWANOMALY);
			else
				anomalyVect.push_back(STANDARD);
		}
	}
	if (profileEnVect.size() > 1 && oneTime){
		profileEnIndex = (profileEnIndex==profileEnVect.size()-1)?0:profileEnIndex+1;
		oneTime = false;
	}
	return anomalyVect;
}

//compute the particle energy matrix as a summation of the previous particle energy matrix
Mat mmParticleEnergy::computeParticleEnergy(Mat energyMatrix, vector<Point2f> particles){
	int pSize = particles.size();
	Mat energyResult = energyMatrix;

	for(int i=0; i<pSize; i++){
		int tx = (particles[i].x-ceil(surroundingArea.width/2)>=0)?((particles[i].x-ceil(surroundingArea.width/2)<energyMatrix.cols)?particles[i].x-ceil(surroundingArea.width/2):energyMatrix.cols-1):0;
		int ty = (particles[i].y-ceil(surroundingArea.height/2)>=0)?((particles[i].y-ceil(surroundingArea.height/2)<energyMatrix.rows)?particles[i].y-ceil(surroundingArea.height/2):energyMatrix.rows-1):0;
		int bx = (particles[i].x+ceil(surroundingArea.width/2)<energyMatrix.cols)?((particles[i].x+ceil(surroundingArea.width/2)>0)?particles[i].x+ceil(surroundingArea.width/2):0):energyMatrix.cols-1;
		int by = (particles[i].y+ceil(surroundingArea.height/2)<energyMatrix.rows)?((particles[i].y+ceil(surroundingArea.height/2)>0)?particles[i].y+ceil(surroundingArea.height/2):0):energyMatrix.rows-1;
		Rect cont = Rect(tx,ty,(bx-tx),(by-ty));
		double energy = 0;

		for(int j=0; j<pSize; j++){
			if (mmLib::mmModules::isPointContRect(cont,particles[j])){
				double dist = pow(particles[i].x-particles[j].x,2) + pow(particles[i].y-particles[j].y,2);
				energy = energy + exp(-dist/(2*pow(sigma,2)));
			}
		}

		energyResult.at<Vec3f>(particles[i].y,particles[i].x)[1] = energyResult.at<Vec3f>(particles[i].y,particles[i].x)[1] + (float)energy;
	}

	return energyResult;
}

//compute energy in each single rectangle
vector<float> mmParticleEnergy::computeEnergyPerArea(Mat energyMatrix){
	Mat mask, stuff;
	vector<float> energyVector;

	for (int l=0; l<surveilanceRect.size(); l++){
		mask = Mat::zeros(energyMatrix.rows,energyMatrix.cols,CV_8U);
		rectangle(mask, surveilanceRect[l] ,Scalar(255,255,255),CV_FILLED);
		energyMatrix.copyTo(stuff,mask);
		mask.release();
		Scalar res= sum(stuff);
		energyVector.push_back(res.val[1]);
		stuff.release();
	}
	return energyVector;
}

vector<float> mmParticleEnergy::getProfileEnVect(int index){
	return profileEnVect[index];
}

vector<float> mmParticleEnergy::getValueEnVect(){
	return valueEnVect;
}

Mat mmParticleEnergy::getMatProfile(){
	return matProfile;
}

Mat mmParticleEnergy::getMatValue(){
	return matValue;
}

bool mmParticleEnergy::getRec(){
	return needToRec;
}
}
}
