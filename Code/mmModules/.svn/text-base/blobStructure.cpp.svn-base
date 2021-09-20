#include "../mmLib.h"

namespace mmLib{
	namespace mmModules{

	harrisDetectorSettings::harrisDetectorSettings(){
		blockSizeParam = 2;
		apertureSizeParam = 3;
		kParam = 0.04;
		threshParam = 200;
		maxFrameParam = 70;
	}

	harrisDetector::harrisDetector(){
		firstFrame = true;
		sameBef = false;
		blockSize = 2;
		apertureSize = 3;
		count = 0;
		k = 0.04;
		thresh = 200;
		maxFrame = 70;
		totalFrameProcessed = -1;
		mask = imread("src/mmLib/hug.png");
	}

	harrisDetector::harrisDetector(bool fF, bool sB, int neighborhoodParam, int apertureParam, int c, double freeParam, int th, int mF){
		firstFrame = fF;
		sameBef = sB;
		blockSize = neighborhoodParam;
		apertureSize = apertureParam;
		count = c;
		k = freeParam;
		thresh = th;
		maxFrame = mF;
		totalFrameProcessed = -1;
		mask = imread("src/mmLib/hug.png");
	}

	harrisDetector::harrisDetector(harrisDetectorSettings & hS){
		firstFrame = true;
		sameBef = false;
		blockSize = hS.blockSizeParam;
		apertureSize = hS.apertureSizeParam;
		count = 0;
		k = hS.kParam;
		thresh = hS.threshParam;
		maxFrame = hS.maxFrameParam;
		totalFrameProcessed = -1;
		mask = hS.maskParam;
		hS.maskParam.release();
	}

	//inizializzo harris corner detector
	void  harrisDetector::InitHarris(){
		if (!firstFrame){
			currentConf.clear();
		}
		else{
			firstFrame = false;
		}
		totalFrameProcessed++;
	}

	//setto la configurazione iniziale dei corner
	void harrisDetector::setInitailConf(Point initConf){
		initialConf.push_back(initConf);
	}

	//setto la configurazione dei corner all'iterazione corrente
	void harrisDetector::setCurrentConf(Point currConf){
		currentConf.push_back(currConf);
	}

	vector<Point> harrisDetector::getInitailConf(){
		return initialConf;
	}

	vector<Point> harrisDetector::getCurrentConf(){
		return currentConf;
	}

	//faccio l'update dei corner points
	void harrisDetector::UpdateHarris(Mat frame){
		Mat imm, immGray, immHarris, immHarrisNorm;
		immHarris = Mat::zeros(frame.size(), CV_32FC1);
		Mat uni = Mat::zeros(mask.rows,mask.cols,CV_8U);
		frame.copyTo(imm,mask);

		//calcolo harris
		cvtColor(imm, immGray, CV_BGR2GRAY);

		cornerHarris(immGray, immHarris, blockSize, apertureSize, k, BORDER_DEFAULT);
		normalize(immHarris, immHarrisNorm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());

		//trovo i corner
		for(int j = 0; j<immHarrisNorm.rows ; j++ ){
			for(int i = 0; i<immHarrisNorm.cols; i++ ){
				if((int) immHarrisNorm.at<float>(j,i) > thresh){
					circle( frame, Point( i, j ), 1,  Scalar(0,0,255));
					if (totalFrameProcessed == 0){
						setInitailConf(Point(i,j));
					}
					else{
						setCurrentConf(Point(i,j));
					}
				}
			}
		}
//		imshow("M",frame);
		frame.release(); imm.release(); immGray.release(); immHarris.release(); immHarrisNorm.release(); uni.release();
	}

	//restituisce true se la condizione e' uguale al frame iniziale o siamo nella fase di stabilizzazione (aspetto maxFrame prima di abbassare l'allarme)
	bool harrisDetector::isHarrisChanged(){
		bool result = false;

		if (totalFrameProcessed>0){
			if (!isSamePoints()){
				result = true;
				sameBef = true;
				count = 0;
			}
			else if (isSamePoints() && sameBef){
				if (count < maxFrame){
					result = true;
				}
				else{
					sameBef = false;
				}
				count++;
			}
		}

		return result;
	}

	//se la configurazione corrente = alla configurazione iniziale, la funzione restituisce true
	bool harrisDetector::isSamePoints(){
		bool result = false;
		int count = 0;

		if (initialConf.size() == currentConf.size()){
			for (int i=0; i<currentConf.size(); i++){
				for (int j=0; j<initialConf.size(); j++){
					if (initialConf[j].x == currentConf[i].x && initialConf[j].y == currentConf[i].y){
						count++;
					}
				}
			}
			if (count == currentConf.size()){
				result = true;
			}
		}

		return result;
	}

	harrisDetector::~harrisDetector(){
		mask.release();
		initialConf.clear();
		currentConf.clear();
	}

	blobStructure::blobStructure(){
		firstFrame = true;
		firstTime = true;
		totalFrameProcessed = 0;
		numPolygons = 0;
	}

	blobStructure::blobStructure(bool fF, bool fT, int tFp, int nP){
		firstFrame = fF;
		firstTime = fT;
		totalFrameProcessed = tFp;
		numPolygons = nP;
	}

	//inizializzo le strutture-->una volta solo per i poligoni, ad ogni iterazione i vettori di blob
	void blobStructure::InitBlobStructure(int numPolygons){
		if (firstFrame){
			poly = new vector<polygons>;
			firstFrame = false;
		}
		else{
			delete blobIn;
			delete blobOut;
		}
		blobIn = new vector<blobListModule>;
		blobOut = new vector<blobListModule>;
		totalFrameProcessed++;
	}

	//acquisice i poligoni e salva i punti di contorno
	void blobStructure::setPolygons(vector<vector<Point> > contours, int numPoly){
		if (firstTime){
			numPolygons++;
			polygons p;
			p.contourPoints = contours;
			poly[0].push_back(p);

			if (numPoly == numPolygons){
				firstTime = false;
			}
		}
		contours.clear();
//		std::cout << "Poly Size:: " << poly[0].size() << std::endl;
	}

	//controlla se il centro del blob è all'interno del poligono e assegna alla lista specifica
	void blobStructure::UpdateLists(vector<mmBlob> blobList){
		float dist;

		//scorro il vettore dei blob
		for (int i=0; i<blobList.size(); i++){
			bool isConteined = false;
			blobListModule bl;
			bl.center = blobList[i].centroid();
			bl.index = i;

			//scorro i vari poligoni
			for (int l=0; l<poly[0].size(); l++){
				//scorro tutti i conotni
				for (int j=0; j<poly[0][l].contourPoints.size(); j++){
					dist = pointPolygonTest(poly[0][l].contourPoints[j], bl.center, true);

					if(dist>=0){
						isConteined = true;
					}
				}
			}

			if(isConteined && ((blobList[i].getType() == NONVISIBLE) || (blobList[i].getType() == NEW))){
				setBlobIn(bl);
			}
			else{
				setBlobOut(bl);
			}
		}
		blobList.clear();
	}

	//crea la lista dei blob interni ai poligoni
	void blobStructure::setBlobIn(blobListModule listIn){
		blobIn[0].push_back(listIn);
	}

	//crea la lista dei blob esterni ai poligoni
	void blobStructure::setBlobOut(blobListModule listOut){
		blobOut[0].push_back(listOut);
	}

	//restituisce la lista dei blob interni
	vector<blobListModule> *blobStructure::getBIn(){
		return blobIn;
	}

	//restituisce la lista dei blob interni
	vector<blobListModule> *blobStructure::getBOut(){
		return blobOut;
	}

	//restituisce la lista dei blob interni-->solo x visulaizzazione
	vector<mmBlob> blobStructure::getBlobIn(vector<mmBlob> blobList){
		vector<mmBlob> result;
		mmBlob stuff;

		for (int i=0; i<blobIn[0].size(); i++){
			stuff = blobList[blobIn[0][i].index];
			stuff.setColor(Scalar(0,0,255));
			result.push_back(stuff);
		}
		blobList.clear();

		return result;
	}

	//restituisce la lista dei blob interni-->solo x visulaizzazione
	vector<mmBlob> blobStructure::getBlobOut(vector<mmBlob> blobList){
		vector<mmBlob> result;
		mmBlob stuff;

		for (int i=0; i<blobOut[0].size(); i++){
			stuff = blobList[blobOut[0][i].index];
			stuff.setColor(Scalar(0,255,0));
			result.push_back(stuff);
		}
		blobList.clear();

		return result;
	}

	Mat DrawList(Mat image, vector<blobListModule> *bIn, vector<blobListModule> *bOut){
		Mat result;
		image.copyTo(result);

		for (int i=0; i<bIn[0].size(); i++){
			circle(result,bIn[0][i].center,1,Scalar(0,0,255));
		}

		for (int i=0; i<bOut[0].size(); i++){
			circle(result,bOut[0][i].center,1,Scalar(0,255,0));
		}
		image.release();

		return result;
	}

	blobStructure::~blobStructure(){
		if (!firstFrame && poly != NULL){
			for(int i=0; i<poly[0].size(); i++){
				poly[0].erase(poly[0].begin(),poly[0].end());
			}
			poly->clear();
			delete poly;
			poly = NULL;
		}
		if (blobIn != NULL){
			for(int i=0; i<blobIn[0].size(); i++){
				blobIn[0].erase(blobIn[0].begin(),blobIn[0].end());
			}
			blobIn->clear();
			delete blobIn;
			blobIn = NULL;
		}

		if (blobOut != NULL){
			for(int i=0; i<blobOut[0].size(); i++){
				blobOut[0].erase(blobOut[0].begin(),blobOut[0].end());
			}
			blobOut->clear();
			delete blobOut;
			blobOut = NULL;
		}
	}

	}
}


