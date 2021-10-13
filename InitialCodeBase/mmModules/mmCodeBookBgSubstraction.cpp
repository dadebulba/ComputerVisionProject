#include "../mmLib.h"


namespace mmLib {
namespace mmModules{

int cb_threshold_value;
int tDelete,tCache,tAdd,tFrame,gammaP,tAssorb;

codeBookParameters::codeBookParameters(){
	cbMemory=200;
	alphaValue=0.8;
	betaValue=1.1;
	epsilon1Value=20;
	tCacheValue=20;
	tAddValue=150;
	tDeleteValue=3000;
	tAssorbValue=30;
}

mmCodeBookBgSubstraction::mmCodeBookBgSubstraction(){
	CodeBookMemory=200; //N
	cb_threshold_value=CodeBookMemory/2; //TM
	totalFrameProcessed=0;
	isInTraining=true;
	alpha=0.8;
	beta=1.1;
	epsilon1=20;
	firsTime=true;
	cbFind=false;
	tCache=20;
	tAdd=150;
	tDelete=3000;
	tAssorb=30;
	gammaP=0.3;
}
mmCodeBookBgSubstraction::~mmCodeBookBgSubstraction()
{
	if(!firsTime)
	{
		codeBook->clear();
		//codeBook->~vector();
		delete[] codeBook;
	}
}
mmCodeBookBgSubstraction::mmCodeBookBgSubstraction(int codeBookMemory,double alpha_value,double beta_value,double epsilon1_value,
		int tCache_value,int tAdd_value,int tDelete_value,int tAssorb_value){
	CodeBookMemory=codeBookMemory;
	cb_threshold_value=CodeBookMemory/2;
	totalFrameProcessed=0;
	isInTraining=true;
	alpha=alpha_value;
	beta=beta_value;
	epsilon1=epsilon1_value;
	firsTime=true;
	cbFind=false;
	tCache=tCache_value;
	tAdd=tAdd_value;
	tDelete=tDelete_value;
	tAssorb=tAssorb_value;
	gammaP=0.3;
}

mmCodeBookBgSubstraction::mmCodeBookBgSubstraction(codeBookParameters &cb){
	CodeBookMemory=cb.cbMemory;
	cb_threshold_value=CodeBookMemory/2;
	totalFrameProcessed=0;
	isInTraining=true;
	alpha=cb.alphaValue; 
	beta=cb.betaValue;
	epsilon1=cb.epsilon1Value;
	firsTime=true;
	cbFind=false;
	tCache=cb.tCacheValue;
	tAdd=cb.tAddValue;
	tDelete=cb.tDeleteValue;
	tAssorb=cb.tAssorbValue;
	gammaP=0.3;
}
//////////////////////////////////////
//mmCodebookBGParameters createMmCodebookBGModel(int codeBookMemory,double alpha_value,double beta_value,double epsilon1_value,int tCache_value,
//				int tAdd_value,int tDelete_value,int tAssorb_value){
//	mmCodebookBGParameters p;
//	p.codeBookMemory = codeBookMemory;
//	p.alpha_value = alpha_value;
//	p.beta_value = beta_value;
//	p.epsilon1_value = epsilon1_value;
//	p.tCache_value = tCache_value;
//	p.tAdd_value = tAdd_value;
//	p.tDelete_value = tDelete_value;
//	p.tAssorb_value = tAssorb_value;
//}
//////////////////////////////////////
cv::Mat mmCodeBookBgSubstraction::processFrame(cv::Mat I)
{
	cv::Mat result(I.rows,I.cols,CV_8UC3,cv::Scalar(0));
	cv::Mat bg(I.rows,I.cols,CV_8UC1,cv::Scalar(0));
	cv::Mat ret;
	if(firsTime)
	{ //iniziallizza il codebook
		codeBook = new std::vector<CodeBookElement>[I.rows*I.cols];
		firsTime=false;
	}
	maxIndex=0;
	times=0;
	//double t = (double)cv::getTickCount();
	//double IntensityMatrix[I.rows*I.cols];
	totalFrameProcessed++;
	int channels = I.channels();

	int nRows = I.rows * channels;
	int nCols = I.cols;

	if (I.isContinuous())
	{ //se I è un vettore
		nCols *= nRows;
		nRows = 1;
	}

	int i,j,z;
	uchar* p;
	uchar* r;
	//cout << "r: " << nRows << endl;
	//cout << "c: " << nCols << endl;
	for( i = 0; i < nRows; ++i){
		//puntatore alla riga i della matrice I
		p = I.ptr<uchar>(i);
		//puntatore alla riga i della matrice result
		r = result.ptr<uchar>(i);
		for ( j = 0,z=0; j < nCols; j=j+3,z++){
			//xt
			int Bp = (int)p[j];
			int Gp = (int)p[j+1];
			int Rp = (int)p[j+2];
			//            int Rpn = Rp/(Rp+Gp+Bp);
			//            int Gpn = Gp/(Rp+Gp+Bp);
			//            int Bpn = (Rp+Gp+Bp)/3;
			//r[z]=(uchar)122;
			//IntensityMatrix[z]=sqrt(Bp*Bp+Gp*Gp+Rp*Rp);
			//I^2
			double Ip=(Bp*Bp+Gp*Gp+Rp*Rp);
			int cbResult = UpdateCodebook(Bp,Gp,Rp,Ip,z);
			if(!isInTraining){
				switch(cbResult){
				case 0:
					//r[z]=(uchar)0;
					break;
				case 1:
					//r[j+2]=(uchar)255;
					bg.at<uchar>(i,z) = 255;
//					result.at<uchar>(i,j)   = I.at<uchar>(i,j);
//					result.at<uchar>(i,j+1) = I.at<uchar>(i,j+1);
//					result.at<uchar>(i,j+2) = I.at<uchar>(i,j+2);
//					r[j] = p[j];
//					r[j+1] = p[j+1];
//					r[j+2] = p[j+2];
					break;
				case 2:
					//r[j+1]=(uchar)255;
					bg.at<uchar>(i,z) = 255;
//					result.at<uchar>(i,j)   = I.at<uchar>(i,j);
//					result.at<uchar>(i,j+1) = I.at<uchar>(i,j+1);
//					result.at<uchar>(i,j+2) = I.at<uchar>(i,j+2);
//					r[j] = p[j];
//					r[j+1] = p[j+1];
//					r[j+2] = p[j+2];
					break;
				case 3:
					//r[j]=(uchar)255;
					bg.at<uchar>(i,z) = 255;
//					result.at<uchar>(i,j)   = I.at<uchar>(i,j);
//					result.at<uchar>(i,j+1) = I.at<uchar>(i,j+1);
//				 	result.at<uchar>(i,j+2) = I.at<uchar>(i,j+2);
//					r[j] = p[j];
//					r[j+1] = p[j+1];
//					r[j+2] = p[j+2];
					break;

				}
			}
		}
	}
	//t = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
	//std::cout <<  " training frame: " << totalFrameProcessed << " max index: " << maxIndex << " times:" << times << std::endl;
	if(totalFrameProcessed>=CodeBookMemory){
		isInTraining=false;
	}
//	if (isColor){
//		ret = result;
//	}
//	else{
		ret = bg;
//	}
	result.release(); bg.release(); I.release();
	return ret;
}
//Return
//0 is bg
//1 is non permanent bg
//2 fg in cache
//3 fg
//-1 error
//bool mmCodeBookBgSubstraction::IsInTraining()
int mmCodeBookBgSubstraction::UpdateCodebook(int Bp, int Gp, int Rp, double Ip, int index)
{
	int returnValue=3;
	cbFind=false;
	bool ccFind=false;
	int lastIndex;
	for(lastIndex=0;lastIndex<codeBook[index].size();lastIndex++)
	{
		double vect =pow((codeBook[index][lastIndex].Rx*Rp + codeBook[index][lastIndex].Gx*Gp + codeBook[index][lastIndex].Bx*Bp),2);
		double vvect = codeBook[index][lastIndex].Rx*codeBook[index][lastIndex].Rx+codeBook[index][lastIndex].Gx*codeBook[index][lastIndex].Gx+codeBook[index][lastIndex].Bx*codeBook[index][lastIndex].Bx;
		double p=vect/vvect;
		if(vect==0)
			p=0;
		double colordist = sqrt(Ip-p);
		//std::cout << "Colordist: " << colordist << " for pixel: " << Ip << " and frame: " << vvect << "\n";
		if(colordist<=epsilon1)
		{

			double iLow=alpha*codeBook[index][lastIndex].maxI;
			double iHigh=std::min(beta*codeBook[index][lastIndex].maxI,codeBook[index][lastIndex].minI/alpha);
			double Ii=sqrt(Ip);
			// file << "Colordist: " << colordist << " for pixel: " << index << " and frame: " << currentTrainingFrame << "\n";
			// file << "iLow: " << iLow << " iHigh: " << iHigh << " Ii: " << Ii << " for pixel: " << index << " and frame: " << currentTrainingFrame << "\n";
			if(Ii>=iLow*0.7 && Ii<=iHigh*1.5)
			{

				codeBook[index][lastIndex].Rx=(codeBook[index][lastIndex].Rx*codeBook[index][lastIndex].frequency+Rp)/(codeBook[index][lastIndex].frequency+1);
				codeBook[index][lastIndex].Gx=(codeBook[index][lastIndex].Gx*codeBook[index][lastIndex].frequency+Gp)/(codeBook[index][lastIndex].frequency+1);
				codeBook[index][lastIndex].Bx=(codeBook[index][lastIndex].Bx*codeBook[index][lastIndex].frequency+Bp)/(codeBook[index][lastIndex].frequency+1);
				//                codeBook[index][lastIndex].Rx=gammaP*Rp+(1-gammaP)*codeBook[index][lastIndex].Rx;
				//                codeBook[index][lastIndex].Gx=gammaP*Gp+(1-gammaP)*codeBook[index][lastIndex].Gx;
				//                codeBook[index][lastIndex].Bx=gammaP*Bp+(1-gammaP)*codeBook[index][lastIndex].Bx;
				codeBook[index][lastIndex].minI=std::min(Ii,codeBook[index][lastIndex].minI);
				codeBook[index][lastIndex].maxI=std::max(Ii,codeBook[index][lastIndex].maxI);
				codeBook[index][lastIndex].frequency=codeBook[index][lastIndex].frequency+1;
				codeBook[index][lastIndex].maximumNegativeRunLength=std::max(codeBook[index][lastIndex].maximumNegativeRunLength,totalFrameProcessed-codeBook[index][lastIndex].lastAccessTime);
				codeBook[index][lastIndex].lastAccessTime=totalFrameProcessed;
				if(codeBook[index][lastIndex].permanentOrCache==0)
				{
					cbFind=true;
					if(codeBook[index][lastIndex].firstAccessTime<=(totalFrameProcessed-tAssorb) || codeBook[index][lastIndex].firstAccessTime<=CodeBookMemory)
						returnValue=0;
					else
					{
						returnValue=1;
					}
					break;

				}
				else
				{
					ccFind=true;
					returnValue=2;
				}

			}
		}

		if(totalFrameProcessed==CodeBookMemory)
		{
			//update MNLR
			codeBook[index][lastIndex].maximumNegativeRunLength=std::max(codeBook[index][lastIndex].maximumNegativeRunLength,(CodeBookMemory-codeBook[index][lastIndex].lastAccessTime+codeBook[index][lastIndex].firstAccessTime-1));
		}
		if(!isInTraining)
			if(codeBook[index][lastIndex].permanentOrCache==1)
			{
				if((totalFrameProcessed-codeBook[index][lastIndex].firstAccessTime)>=tAdd)
				{
					//std::cout << "step: " << totalFrameProcessed-codeBook[index][lastIndex].firstAccessTime << std::endl;
					codeBook[index][lastIndex].permanentOrCache=0;
				}
			}
	}

	if(totalFrameProcessed==CodeBookMemory)
	{
		codeBook[index].erase(std::remove_if(codeBook[index].begin(),codeBook[index].end(),cleanCodebook),codeBook[index].end());

	}
	// std::cout << "fine loop" << std::endl;
	//std::cout << codeBook[0].Bx << std::endl;
	//if(!cbFind && isInTraining)
	if(!cbFind && !ccFind)
	{
		maxIndex=codeBook[index].size();
		//std::cout << "Add element in position: " << lastIndex << std::endl;
		CodeBookElement toAdd;
		toAdd.Rx=(double)Rp;
		toAdd.Gx=(double)Gp;
		toAdd.Bx=(double)Bp;
		toAdd.minI=sqrt(Ip);
		toAdd.maxI=sqrt(Ip);
		toAdd.frequency=1;
		toAdd.maximumNegativeRunLength=totalFrameProcessed-1;
		toAdd.firstAccessTime=totalFrameProcessed;
		toAdd.lastAccessTime=totalFrameProcessed;
		if(isInTraining)
			toAdd.permanentOrCache=0;
		else
			toAdd.permanentOrCache=1;
		// codeBook[index][lastIndex]=toAdd;
		//if(isInTraining)
			codeBook[index].push_back(toAdd);
			//file << "Low: " << sqrt(Ip) << " for pixel: " << index << " and frame: " << currentTrainingFrame << "\n";
			//if(!cbFind)
			returnValue=3;
	}
	tFrame=totalFrameProcessed;
	if(!isInTraining)
	{
		// std::cout << "Start size: " << codeBook[index].size() << std::endl;
		codeBook[index].erase(std::remove_if(codeBook[index].begin(),codeBook[index].end(),cleanCacheBGCodebook),codeBook[index].end());
		// std::cout << "End size: " << codeBook[index].size() << std::endl;
	}
	return returnValue;


}
bool mmCodeBookBgSubstraction::cleanCodebook(CodeBookElement element)
{
	return (element.maximumNegativeRunLength>cb_threshold_value);
}
bool mmCodeBookBgSubstraction::cleanCacheBGCodebook(CodeBookElement element)
{
	if(element.permanentOrCache==0)
		return ((tFrame-element.lastAccessTime)>=tDelete);
	else
		return((tFrame-element.lastAccessTime)>=tCache);
}
}
}
