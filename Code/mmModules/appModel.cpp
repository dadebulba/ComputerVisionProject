/*
 * mmBlobDetector.cpp
 *
 *  Created on: Mar 4, 2013
 *      Author: matteo
 */

#include "../mmLib.h"

namespace mmLib {
namespace mmModules{
appSettings::appSettings(){
	appAlpha = 0.95;
	appLambda = 0.95;
	appSigma = 0.5;
	appThresh = 0.03;
}

appModel::appModel(){
	alpha = 0.95;
	lambda = 0.95;
	sigma = 0.5;
	thresh = 0.03;
	totalFrameProcessed =-1;
}

appModel::appModel(appSettings &a){
	alpha = a.appAlpha;
	lambda = a.appLambda;
	sigma = a.appSigma;
	thresh = a.appThresh;
	totalFrameProcessed = -1;
}

//	appModel::~appModel(){
//
//	}

void appModel::appFrame(){
	totalFrameProcessed++;
}

mmBlob appModel::newAppModel(Mat frame,Mat foreground,mmBlob blob){
	uchar* pr = frame.ptr<uchar>(0);

	Mat M(frame.rows,frame.cols,CV_32FC3,CV_RGB(0,0,0));
	Mat prob(frame.rows,frame.cols,CV_32F,cv::Scalar(0));

	for (int i=blob.getTL().x; i<=blob.getBR().x; i++){
		for (int j=blob.getTL().y; j<=blob.getBR().y; j++){
			Point p = Point(i,j);
			int index = (j*frame.cols+i);

			float Bi = (float)pr[3*index];
			float Gi = (float)pr[3*index+1];
			float Ri = (float)pr[3*index+2];

			if (isFg(foreground,p)){
				M.at<Vec3f>(j,i)[0] = Bi;
				M.at<Vec3f>(j,i)[1] = Gi;
				M.at<Vec3f>(j,i)[2] = Ri;
				prob.at<float>(j,i) = 0.4;
			}
		}
	}
	blob.setM(M);
	blob.setP(prob);

	M.release(); prob.release(); frame.release(); foreground.release();

	return blob;
}

mmBlob appModel::stdAppModel(Mat frame, Mat foreground, mmBlob blobNew, mmBlob blobOld){
	uchar* pr = frame.ptr<uchar>(0);

	Mat M(frame.rows,frame.cols,CV_32FC3,CV_RGB(0,0,0));
	Mat prob(frame.rows,frame.cols,CV_32F,cv::Scalar(0));

	for (int i=blobNew.getTL().x; i<=blobNew.getBR().x; i++){
		for (int j=blobNew.getTL().y; j<=blobNew.getBR().y; j++){
			Point p = Point(i,j);
			int index = (j*frame.cols+i);

			float Bi = (float)pr[3*index];
			float Gi = (float)pr[3*index+1];
			float Ri = (float)pr[3*index+2];

			if (isFg(foreground,p)){
				M.at<Vec3f>(j,i)[0] = alpha*blobOld.getM().at<Vec3f>(j,i)[0] + (1-alpha)*Bi;
				M.at<Vec3f>(j,i)[1] = alpha*blobOld.getM().at<Vec3f>(j,i)[1] + (1-alpha)*Gi;
				M.at<Vec3f>(j,i)[2] = alpha*blobOld.getM().at<Vec3f>(j,i)[2] + (1-alpha)*Ri;
				prob.at<float>(j,i) = lambda*blobOld.getP().at<float>(j,i) + (1-lambda);
			}
			else{
				prob.at<float>(j,i) = lambda*blobOld.getP().at<float>(j,i);
			}
		}
	}
	blobNew.setM(M);
	blobNew.setP(prob);

	M.release(); prob.release(); frame.release(); foreground.release();

	return blobNew;
}

mmBlob appModel::mergeAppModel(Mat frame, Mat foreground, mmBlob blobNew, mmBlob blobOld){
	uchar* pr = frame.ptr<uchar>(0);

	Mat M(frame.rows,frame.cols,CV_32FC3,CV_RGB(0,0,0));
	Mat prob(frame.rows,frame.cols,CV_32F,cv::Scalar(0));

	vector<Point> vectPoint = blobNew.getPointList();
	for (int i=0; i<vectPoint.size(); i++){
		Point p = Point(vectPoint[i].x,vectPoint[i].y);
		int index = (p.y*frame.cols+p.x);

		float Bi = (float)pr[3*index];
		float Gi = (float)pr[3*index+1];
		float Ri = (float)pr[3*index+2];

		if (isFg(foreground,p)){
			M.at<Vec3f>(p.y,p.x)[0] = alpha*blobOld.getM().at<Vec3f>(p.y,p.x)[0] + (1-alpha)*Bi;
			M.at<Vec3f>(p.y,p.x)[1] = alpha*blobOld.getM().at<Vec3f>(p.y,p.x)[1] + (1-alpha)*Gi;
			M.at<Vec3f>(p.y,p.x)[2] = alpha*blobOld.getM().at<Vec3f>(p.y,p.x)[2] + (1-alpha)*Ri;
			prob.at<float>(p.y,p.x) = lambda*blobOld.getP().at<float>(p.y,p.x) + (1-lambda);
		}
		else{
			prob.at<float>(p.y,p.x) = lambda*blobOld.getP().at<float>(p.y,p.x);
		}
	}
	blobNew.setM(M);
	blobNew.setP(prob);

	M.release(); prob.release(); frame.release(); foreground.release();

	return blobNew;
}

mmBlob appModel::nonMergeAppModel(Mat frame, Mat foreground, mmBlob blobNew, vector<mmBlob> blobOld){
	uchar* pr = frame.ptr<uchar>(0);

	Mat M(frame.rows,frame.cols,CV_32FC3,CV_RGB(0,0,0));
	Mat prob(frame.rows,frame.cols,CV_32F,cv::Scalar(0));

	for (int i=blobNew.getTL().x; i<=blobNew.getBR().x; i++){
		for (int j=blobNew.getTL().y; j<=blobNew.getBR().y; j++){
			Point p = Point(i,j);
			int index = (j*frame.cols+i);

			float Bi = (float)pr[3*index];
			float Gi = (float)pr[3*index+1];
			float Ri = (float)pr[3*index+2];

			for(int k=0; k<blobOld.size(); k++){
				prob.at<float>(j,i) = prob.at<float>(j,i) + lambda*blobOld[k].getP().at<float>(j,i);
				if (isFg(foreground,p)){
					M.at<Vec3f>(j,i)[0] = M.at<Vec3f>(j,i)[0] + alpha*blobOld[k].getM().at<Vec3f>(j,i)[0];
					M.at<Vec3f>(j,i)[1] = M.at<Vec3f>(j,i)[1] + alpha*blobOld[k].getM().at<Vec3f>(j,i)[1];
					M.at<Vec3f>(j,i)[2] = M.at<Vec3f>(j,i)[2] + alpha*blobOld[k].getM().at<Vec3f>(j,i)[2];
				}
			}
			if (isFg(foreground,p)){
				M.at<Vec3f>(j,i)[0] = M.at<Vec3f>(j,i)[0] + (1-alpha)*Bi;
				M.at<Vec3f>(j,i)[1] = M.at<Vec3f>(j,i)[1] + (1-alpha)*Gi;
				M.at<Vec3f>(j,i)[2] = M.at<Vec3f>(j,i)[2] + (1-alpha)*Ri;
				prob.at<float>(j,i) = prob.at<float>(j,i)  + (1-lambda);
			}
		}
	}
	blobNew.setM(M);
	blobNew.setP(prob);

	M.release(); prob.release(); frame.release(); foreground.release(); blobOld.clear();

	return blobNew;
}

Mat appModel::getAppMatrix(Mat frame, mmBlob blob){
	int nRows = frame.rows;
	int nCols = frame.cols;

	Mat ret(nRows,nCols,CV_8UC3,cv::Scalar(0));

	for (int i=blob.getTL().x; i<blob.getBR().x; i++){
		for (int j = blob.getTL().y; j < blob.getBR().y; j++){
			ret.at<uchar>(j,3*i)   = (uchar)blob.getM().at<Vec3f>(j,i)[0];
			ret.at<uchar>(j,3*i+1) = (uchar)blob.getM().at<Vec3f>(j,i)[1];
			ret.at<uchar>(j,3*i+2) = (uchar)blob.getM().at<Vec3f>(j,i)[2];
		}
	}

	frame.release();
	return ret;
}

Mat appModel::getProbMatrix(Mat frame, mmBlob blob){
	int nRows = frame.rows;
	int nCols = frame.cols;

	Mat ret(nRows,nCols,CV_8U,cv::Scalar(0));

	for (int i=blob.getTL().x; i<blob.getBR().x; i++){
		for (int j = blob.getTL().y; j < blob.getBR().y; j++){
			ret.at<uchar>(j,i) = blob.getP().at<float>(j,i)*255;
		}
	}

	frame.release();
	return ret;
}

Rect appModel::propagatedRect(Mat frame, mmBlob blob){
	int x = (blob.getTL().x+blob.getDispVect().x>=0)?((blob.getTL().x+blob.getDispVect().x<frame.cols-1)?blob.getTL().x+blob.getDispVect().x:frame.cols-1):0;
	int y = (blob.getTL().y+blob.getDispVect().y>=0)?((blob.getTL().y+blob.getDispVect().y<frame.rows-1)?blob.getTL().y+blob.getDispVect().y:frame.rows-1):0;
	int w = (x+blob.getWidth()<frame.cols-1)?((x+blob.getWidth()>=0)?blob.getWidth():0):frame.cols-1-x;
	int h = (y+blob.getHeight()<frame.rows-1)?((y+blob.getHeight()>=0)?blob.getHeight():0):frame.rows-1-y;

	Rect rect = Rect(x,y,w,h);

	frame.release();
	return rect;
}


mmBlob appModel::propagateObject(Mat frame, Mat foreground, mmBlob track){
	mmBlob result;
	vector <Point> pRes;
	double Pci, pRGB, pi;
	Rect r = propagatedRect(frame, track);

	for (int i=r.x; i<=(r.x+r.width); i++){
		for (int j=r.y; j<=(r.y+r.height); j++){
			Point p = Point(i,j);

			if (isFg(foreground,p)){
				Point pOld;
				int x = (i-track.getDispVect().x>=0)?((i-track.getDispVect().x<frame.cols)?i-track.getDispVect().x:frame.cols-1):0;
				int y = (j-track.getDispVect().y>=0)?((j-track.getDispVect().y<frame.rows)?j-track.getDispVect().y:frame.rows-1):0;
				pOld = Point(x,y);
				if (track.getP().empty())
					Pci = 0;
				else
					Pci = (double)track.getP().at<float>(pOld.y,pOld.x);

				if (Pci != 0){
					pRGB = getPrgb(frame, track, p, pOld);
					pi = pRGB*Pci;
				}
				else{
					pi = 0;
				}

				if (pi>thresh){
					pRes.push_back(p);
				}
			}
		}
	}
	if (pRes.empty()){
		result = mmBlob();
	}
	else{
		result = mmBlob(frame,r.x,r.y,r.width,r.height);
	}

	frame.release(); foreground.release(); pRes.clear();

	return result;
}

vector<mmBlob> appModel::SM(Mat frame, Mat foreground, mmBlob blobU, vector<mmBlob> blobVect){
	vector<mmBlob> result, res;
	vector<Point> pAmb;
	vector<double> pi;
	vector<int> depth, isContained, dispNumPoint;

	Point p, pOld, maxIdxPi, minIdxDepth, maxIdxDP;
	double Pci, pRGB, maxPi, maxDepth, maxDP;
	bool isAssigned = true;
	int bSize = blobVect.size();
	int count;
	int tx = blobU.getTL().x;
	int ty = blobU.getTL().y;
	int bx = blobU.getBR().x;
	int by = blobU.getBR().y;

	Mat fgAnd = Mat::zeros(frame.rows,frame.cols,CV_8U);
	Mat fgAmb = Mat::zeros(frame.rows,frame.cols,CV_8U);
	Mat maskAmb = Mat::zeros(frame.rows, frame.cols, CV_8U);
	Mat maskAnd = Mat::zeros(frame.rows, frame.cols, CV_8U);

	for (int i=0; i<bSize; i++){
		Rect r = propagatedRect(frame, blobVect[i]);
		tx = (r.x<tx)?r.x:tx;
		ty = (r.y<ty)?r.y:ty;
		bx = (r.x+r.width>bx)?r.x+r.width:bx;
		by = (r.y+r.height>by)?r.y+r.height:by;

		result.push_back(mmBlob(frame,r.x,r.y,r.width,r.height));
		dispNumPoint.push_back(0);
	}
	rectangle(maskAmb,Rect(tx,ty,bx-tx,by-ty),Scalar(255,255,255),CV_FILLED);

	for (int i=0; i<bSize; i++){
		Mat mask;
		Rect rect1 = result[i].boundingBoxProcessed();

		rectangle(maskAmb,rect1,Scalar(0,0,0),CV_FILLED);
		rectangle(mask,rect1,Scalar(255,255,255),CV_FILLED);
		for (int j=0; j<bSize; j++){
			Rect rect2 = result[j].boundingBoxProcessed();
			Rect intersezione = rect1&rect2;
			if (i!=j && intersezione != Rect(0,0,0,0)){

				rectangle(mask,intersezione,Scalar(0,0,0),CV_FILLED);
				rectangle(maskAnd,intersezione,Scalar(255,255,255),CV_FILLED);
			}
		}
		Mat fg1;
		foreground.copyTo(fg1,mask);
		result[i].setForeground(fg1);
		fg1.release(); mask.release();
	}
	foreground.copyTo(fgAmb,maskAmb);
	foreground.copyTo(fgAnd,maskAnd);
	maskAnd.release(); maskAmb.release();

	for (int i=tx; i<=bx; i++){
		for (int j=ty; j<=by; j++){
			p = Point(i,j);
			if (isFg(foreground,p)){
				pi.clear();
				isContained.clear();
				depth.clear();
				if (isFg(fgAnd,p)){
					for (int k=0; k<bSize; k++){
						int xOld = (p.x-blobVect[k].getDispVect().x>0)?(p.x-blobVect[k].getDispVect().x<frame.cols)?p.x-blobVect[k].getDispVect().x:frame.cols-1:0;
						int yOld = (p.y-blobVect[k].getDispVect().y>0)?(p.y-blobVect[k].getDispVect().y<frame.rows)?p.y-blobVect[k].getDispVect().y:frame.rows-1:0;
						pOld = Point(xOld,yOld);
						if (blobVect[k].getP().empty())
							Pci = 0;
						else
							Pci = blobVect[k].getP().at<float>(pOld.y,pOld.x);

						if (Pci != 0){
							pRGB = getPrgb(frame, blobVect[k], p, pOld);
							pi.push_back(pRGB*Pci);
							isContained.push_back(k);
							depth.push_back(blobVect[k].getDepth());
						}
					}

					if (isContained.size() == 0){
						pAmb.push_back(p);
					}
					else if (isContained.size() == 1){
						result[isContained[0]].pushPoint(p);
					}
					else{
						//FirstMerge
						if (countNonZero(depth) != depth.size()){
							minMaxLoc(pi,0,&maxPi,0,&maxIdxPi);
							result[isContained[maxIdxPi.x]].pushPoint(p);
							for (int k=0; k<pi.size(); k++){
								dispNumPoint[isContained[k]] = (dispNumPoint[isContained[k]] == 0)?1:dispNumPoint[isContained[k]];
							}
							dispNumPoint[isContained[maxIdxPi.x]] = dispNumPoint[isContained[maxIdxPi.x]] + 1;
						}
						//Other iterations
						else{
							count = 0;

							while(count < pi.size()){
								minMaxLoc(depth,0,&maxDepth,&minIdxDepth,0);

								if (pi[minIdxDepth.x] >= thresh){
									result[isContained[minIdxDepth.x]].pushPoint(p);
									count = pi.size();
								}
								else if (pi[minIdxDepth.x] < thresh && count == pi.size()-1){
									pAmb.push_back(p);
									count = pi.size();
								}
								else{
									depth[minIdxDepth.x] = maxDepth+1;
									count++;
								}
							}
						}
					}
				}
				else if (!isFg(fgAnd,p) && !isFg(fgAmb,p)){
					for (int k=0; k<bSize; k++){
						if (result[k].isConteined(p))
							result[k].pushPoint(p);
					}
				}
				else if (!isFg(fgAnd,p) && isFg(fgAmb,p))
					pAmb.push_back(p);
			}
		}
	}

	fgAnd.release(); fgAmb.release();

	result = assignAmbPoints(result,pAmb);
	pAmb.clear();

	//res
	for (int i=0; i<bSize; i++){
		if (result[i].getPointList().empty()){
			res.push_back(mmBlob());
		}
		else{
			res.push_back(mmBlob(frame,result[i].getPointList()));
		}
		res[i].setPointList(result[i].getPointList());
	}

	//res.depth
	count = 1;
	while(isAssigned){
		minMaxLoc(dispNumPoint,0,&maxDP,0,&maxIdxDP);

		if (maxDP == 0){
			res[maxIdxDP.x].setDepth(blobVect[maxIdxDP.x].getDepth());
		}
		else if (maxDP == -1){
			isAssigned = false;
		}
		else{
			res[maxIdxDP.x].setDepth(count);
			count++;
		}
		dispNumPoint[maxIdxDP.x] = -1;
	}

	blobVect.clear(); result.clear(); pi.clear(); dispNumPoint.clear(); depth.clear(); isContained.clear();
	frame.release(); foreground.release();

	return res;
}

//	double appModel::getDistFromCluster(Mat frame, Point p, vector<Point> clusterP){
//		double result = frame.rows*frame.cols;
//		int s = clusterP.size();
//
//		for (int i=0; i<s; i++){
//			Point point = clusterP[i];
//			double dist = sqrt((p.x-point.x)*(p.x-point.x) + (p.y-point.y)*(p.y-point.y));
//			result = (dist<result)?dist:result;
//		}
//		return result;
//	}

vector<mmBlob> appModel::assignAmbPoints(vector<mmBlob> vectBlob,vector<Point> vectPoint){
	vector<Point> blobP;
	double minDist;
	double dist;
	int index;

	for (int i=0; i<vectPoint.size(); i++){
		Point p = vectPoint[i];
		index = 0;
		minDist = 1000;
		for (int j=0; j<vectBlob.size(); j++){
			blobP = vectBlob[j].getPointList();

			for (int k=0; k<blobP.size(); k++){
				dist = sqrt((p.x-blobP[k].x)*(p.x-blobP[k].x)+(p.y-blobP[k].y)*(p.y-blobP[k].y));
				index = (dist<minDist)?j:index;
				minDist = (dist<minDist)?dist:minDist;
			}
		}
		vectBlob[index].pushPoint(vectPoint[i]);
	}

	blobP.clear(); vectPoint.clear();

	return vectBlob;
}

//funziona solo con frame continui
double appModel::getPrgb(Mat frame, mmBlob track, Point p, Point pOld){
	int in = (p.y*frame.cols + p.x);
	uchar* pr = frame.ptr<uchar>(0);

	int Bi = (int)pr[3*in];
	int Gi = (int)pr[3*in+1];
	int Ri = (int)pr[3*in+2];

	double Mb = (double)track.getM().at<Vec3f>(pOld.y,pOld.x)[0];
	double Mg = (double)track.getM().at<Vec3f>(pOld.y,pOld.x)[1];
	double Mr = (double)track.getM().at<Vec3f>(pOld.y,pOld.x)[2];

	double diffB = abs(Bi-Mb)/255;
	double diffG = abs(Gi-Mg)/255;
	double diffR = abs(Ri-Mr)/255;

	//		double weight = sqrt(1/((2*pi*sigma*sigma)*(2*pi*sigma*sigma)*(2*pi*sigma*sigma)));
	double weight = sqrt(1/pow(2*pi*sigma*sigma,3));
	double gauss  = exp(-(pow(diffB,2)+pow(diffG,2)+pow(diffR,2))/(2*pow(sigma,2)));

	double pRgb = weight*gauss;
	return pRgb;

	frame.release();
}

bool isFg(Mat foreground,Point p){
	bool ret = false;

	if (foreground.at<uchar>(p.y,p.x) != (uchar)0){
		ret = true;
	}
	foreground.release();
	return ret;
}

}
}


