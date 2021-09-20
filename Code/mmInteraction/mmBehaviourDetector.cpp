#include "../mmLib.h"

namespace mmLib{
namespace mmInteraction{
mmBehaviourDetectorTrainSettings::mmBehaviourDetectorTrainSettings(){
	clusterTermCritParam = TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10, 0.1);
	svmParametersParam.kernel_type = CvSVM::RBF;
	svmParametersParam.gamma = 1;
	svmParametersParam.svm_type = CvSVM::C_SVC;
	svmParametersParam.C = 10;
	svmParametersParam.term_crit = TermCriteria(CV_TERMCRIT_ITER, 100, 0.001);
	dimensionalityParam = 300;
	clusterBinsParam = 100;
	clusterAttemptsParam = 15;
}

mmBehaviourDetectorTestSettings::mmBehaviourDetectorTestSettings(){
	windowLengthParam = 100;
	frameStepParam = 2;
	numFrameAnomalyParam = 10;
}

mmBehaviourDetector::mmBehaviourDetector(){
	clusterTermCrit = TermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, 10, 0.1);
	svmParameters.kernel_type = CvSVM::RBF;
	svmParameters.gamma = 1;
	svmParameters.svm_type = CvSVM::C_SVC;
	svmParameters.C = 10;
	svmParameters.term_crit = TermCriteria(CV_TERMCRIT_ITER, 100, 0.001);
	dimensionality = 300;
	clusterBins = 100;
	clusterAttempts = 15;

	windowLength = 100;
	frameStep = 2;
	numFrameAnomaly = 10;

	gridGranularity = 8;
	sigma = 1;
	tau = 1;
	totalFrameProcessed = 0;
	countAnomaly = 0;
	needToTest = false;
	systemAnomaly = false;
}

mmBehaviourDetector::mmBehaviourDetector(mmBehaviourDetectorTrainSettings &bdTrain){
	trainFile = bdTrain.trainFileParam; //xml files
	bdTrain.trainFileParam.clear();
	videoFiles = bdTrain.videoFilesParam; //video files
	bdTrain.videoFilesParam.clear();
	clusterTermCrit = bdTrain.clusterTermCritParam; //termination criterion for kmeans alghorithm
	svmParameters = bdTrain.svmParametersParam; //svm parameters
	dimensionality = bdTrain.dimensionalityParam; //feature space dimensionality
	clusterBins = bdTrain.clusterBinsParam; //number of bins per cluster
	clusterAttempts = bdTrain.clusterAttemptsParam; //termination attempts for kmeans alghorithm
	gridGranularity = 8;
	sigma = 1;
	tau = 1;
}

mmBehaviourDetector::mmBehaviourDetector(mmBehaviourDetectorTestSettings &bdTest){
	xmlClusterCenters = bdTest.xmlClusterCentersParam; //xml cluster center file
	bdTest.xmlClusterCentersParam.release();
	svmModelString = bdTest.svmModelStringParam; //svm model
	windowLength = bdTest.windowLengthParam; //frame windows length
	frameStep = bdTest.frameStepParam; //the system process a frame each frameStep frames
	numFrameAnomaly = bdTest.numFrameAnomalyParam;//number of frames for alarm to be raised
	totalFrameProcessed = 0;
	countAnomaly = 0;
	needToTest = false;
	systemAnomaly = false;
}

mmBehaviourDetector::~mmBehaviourDetector(){
	trainFile.clear();
	videoFiles.clear();
	xmlClusterCenters.release();
	particleInWindow.clear();
	frameInWindow.clear();
	frameNumber.clear();
	clusterCenterVect.clear();
}

//reads training parameters from xml files
void mmBehaviourDetector::TrainSvm(){
	Mat label = Mat::zeros(trainFile.size(),1,CV_32F);
	Mat	clusterCenters = Mat::zeros(dimensionality,clusterBins,CV_32F);
	Mat particleLabel;
	vector<float> stuffValue(clusterBins,0);
	vector<vector<float> > histValue(trainFile.size(),stuffValue);
	vector<int> totalLength;
	int matFeaturesCount = 0;
	int totalDimension = 0;

	//read info from the xml files
//	cout << "Init" << endl;
	for(int i=0; i<trainFile.size(); i++){
		FileStorage xmlInfo = trainFile[i];
		int dimension, isAnomaly;

		xmlInfo["numParticle"] >> dimension;
		totalDimension += dimension;
		totalLength.push_back(dimension);

		xmlInfo["isAnomaly"] >> isAnomaly;
		label.at<float>(i,0) = isAnomaly;

		xmlInfo["sigma"] >> sigma;
		xmlInfo["tau"] >> tau;
		xmlInfo["scaleX"] >> scaleX;
		xmlInfo["scaleY"] >> scaleY;
		xmlInfo["GridGranularity"] >> gridGranularity;
//		//TODO: togliere dopo
//		scaleX = 0.5;
//		scaleY = 0.5;

		xmlInfo.release();
	}

	Mat matFeatures = Mat::zeros(totalDimension,dimensionality,CV_32F);

	//compute the 3d-hog features for each train file
//	cout << "3d-hog" << endl;
	for(int i=0; i<trainFile.size(); i++){
		CreateParticle(trainFile[i]);

		string videoFile = videoFiles[i]+".avi";
		vector<vector<float> > hogValue = computeHugMod(videoFile,totalLength[i],dimensionality);

		for(int r=0; r<totalLength[i]; r++){
			for(int c=0; c<dimensionality; c++){
				matFeatures.at<float>(matFeaturesCount+r,c) = hogValue[r][c];
			}
		}

		matFeaturesCount+=totalLength[i];
		hogValue.clear();
		frameNumber.clear();
		particleInWindow.clear();
	}

	//clastering
//	cout << "clastering" << endl;
	double result = kmeans(matFeatures,clusterBins,particleLabel,clusterTermCrit,clusterAttempts,cv::KMEANS_PP_CENTERS,clusterCenters);

	//save claster centers
//	cout << "save centers" << endl;
	saveCenters(clusterCenters);

	//compute histograms
//	cout << "hist" << endl;
	histValue = createHistogram(histValue, particleLabel, totalLength, totalDimension);

	//normalization
//	cout << "norm" << endl;
	histValue = normalizeHistogram(histValue);

	//create svm model
	CreateModel(histValue,label);

	label.release(); clusterCenters.release(); matFeatures.release(); particleLabel.release();
	histValue.clear(); stuffValue.clear(); totalLength.clear();
}

void mmBehaviourDetector::TrainSvm(vector<trainParmeters> trainParamVect){
	Mat label = Mat::zeros(trainParamVect.size(),1,CV_32F);
	Mat	clusterCenters = Mat::zeros(dimensionality,clusterBins,CV_32F);
	Mat particleLabel;
	vector<float> stuffValue(clusterBins,0);
	vector<vector<float> > histValue(trainParamVect.size(),stuffValue);
	vector<int> totalLength;
	int matFeaturesCount = 0;
	int totalDimension = 0;

	//read info from the xml files
//	cout << "Init" << endl;
	for(int i=0; i<trainParamVect.size(); i++){
		totalDimension += trainParamVect[i].numParticle;
		totalLength.push_back(trainParamVect[i].numParticle);

		label.at<float>(i,0) = trainParamVect[i].isAnomaly;

		sigma = trainParamVect[i].sigma;
		tau = trainParamVect[i].tau;
		scaleX = trainParamVect[i].scaleX;
		scaleY = trainParamVect[i].scaleY;
		gridGranularity = trainParamVect[i].gridGranularity;
	}

	Mat matFeatures = Mat::zeros(totalDimension,dimensionality,CV_32F);

	//compute the 3d-hog features for each train file
//	cout << "3d-hog" << endl;
	for(int i=0; i<trainParamVect.size(); i++){
		CreateParticle(trainParamVect[i]);

		string videoFile = videoFiles[i]+".avi";
		vector<vector<float> > hogValue = computeHugMod(videoFile,totalLength[i],dimensionality);

		for(int r=0; r<totalLength[i]; r++){
			for(int c=0; c<dimensionality; c++){
				matFeatures.at<float>(matFeaturesCount+r,c) = hogValue[r][c];
			}
		}

		matFeaturesCount+=totalLength[i];
		hogValue.clear();
		frameNumber.clear();
		particleInWindow.clear();
	}

	//clastering
//	cout << "clastering" << endl;
	double result = kmeans(matFeatures,clusterBins,particleLabel,clusterTermCrit,clusterAttempts,cv::KMEANS_PP_CENTERS,clusterCenters);

	//save claster centers
//	cout << "save centers" << endl;
	saveCenters(clusterCenters);

	//compute histograms
//	cout << "hist" << endl;
	histValue = createHistogram(histValue, particleLabel, totalLength, totalDimension);

	//normalization
//	cout << "norm" << endl;
	histValue = normalizeHistogram(histValue);

	//create svm model
	CreateModel(histValue,label);

	label.release(); clusterCenters.release(); matFeatures.release(); particleLabel.release();
	histValue.clear(); stuffValue.clear(); totalLength.clear();
}

void mmBehaviourDetector::CreateParticle(FileStorage xmlFileString){
	int numParticle, countParticle = 0;
	xmlFileString["numParticle"] >> numParticle;

	for(int i=0; i<numParticle; i++){
		int x, y, frame = 0;
		ostringstream particle;
		particle<<i;
		string particleString = "Particle" + particle.str();
		FileNode pa = xmlFileString[particleString];
		pa["x"] >> x;
		pa["y"] >> y;
		pa["frame"] >> frame;
		vector<Point> stuffPoint;
		stuffPoint.push_back(Point(x,y));
		particleInWindow.push_back(stuffPoint);
		stuffPoint.clear();
		frameNumber.push_back(frame);
	}
	xmlFileString.release();
}

void mmBehaviourDetector::CreateParticle(trainParmeters file){
	for(int i=0; i<file.numParticle; i++){
		vector<Point> stuffPoint;
		stuffPoint.push_back(file.particles[i]);
		particleInWindow.push_back(stuffPoint);
		stuffPoint.clear();
		frameNumber.push_back(file.frameNum[i]);
	}
}

void mmBehaviourDetector::saveCenters(Mat clusterCenters){
	vector<float> centerStuff;
	for(int i=0; i<clusterBins; i++){
		for(int j=0; j<dimensionality; j++){
			centerStuff.push_back(clusterCenters.at<float>(i,j));
		}
		clusterCenterVect.push_back(centerStuff);
		centerStuff.clear();
	}
}

void mmBehaviourDetector::CreateModel(vector<vector<float> > features, Mat labels){
	Mat matFeatures = Mat::zeros(features.size(),features[0].size(),CV_32F);
	for(int i=0; i<features.size(); i++)
		for(int j=0; j<features[0].size(); j++)
			matFeatures.at<float>(i,j) = features[i][j];

	mod.train(matFeatures,labels,Mat(),Mat(),svmParameters);
	matFeatures.release(); labels.release(); features.clear();
}

vector<vector<float> >  mmBehaviourDetector::createHistogram(vector<vector<float> > histValue, Mat particleLabel, vector<int> totalLength, int dimension){
	int idx=0;
	int peso=0;
	for(int i=0; i<dimension; i++){
		if((i-peso)>=totalLength[idx]){
			peso+=totalLength[idx];
			idx++;
		}

		int index = particleLabel.at<uchar>(i,0);
		histValue[idx][index]+=1;
	}

	return histValue;
}

vector<vector<float> >  mmBehaviourDetector::normalizeHistogram(vector<vector<float> > histValue){
	for(int i=0; i<histValue.size(); i++){
		vector<float> stuffVect = histValue[i];
		pow(stuffVect,2,stuffVect);
		sum(stuffVect);
		double sumHist = std::accumulate(stuffVect.begin(),stuffVect.end(),0.0);
		stuffVect.clear();
		if(sumHist != 0)
			divide(histValue[i],sqrt(sumHist),histValue[i]);
	}

	return histValue;
}

void mmBehaviourDetector::InitializeModule(){
	xmlClusterCenters["Dimensionality"] >> dimensionality;
	xmlClusterCenters["ClasterBins"] >> clusterBins;
	xmlClusterCenters["Sigma"] >> sigma;
	xmlClusterCenters["Tau"] >> tau;

	FileNode videoNode = xmlClusterCenters["Video"];
	videoNode["Height"] >> videoProp.height;
	videoNode["Width"] >> videoProp.width;
	videoNode["ScaleX"] >> videoProp.scaleW;
	scaleX = videoProp.scaleW;
	videoNode["ScaleY"] >> videoProp.scaleH;
	scaleY = videoProp.scaleH;
	videoNode["Character"] >> videoProp.character;
	videoNode["FrameRate"] >> videoProp.frameRate;
	videoNode["isColor"] >> videoProp.isColor;

	FileNode clusterNode = xmlClusterCenters["ClasterCenters"];
	for(int i=0; i<clusterBins; i++){
		ostringstream bins;
		bins<<i;
		vector<float> stuffVect;
		for(int j=0; j<dimensionality; j++){
			ostringstream dimen;
			dimen<<j;
			string centString = "center" + bins.str() +"x" + dimen.str();
			float value;
			clusterNode[centString] >> value;
			stuffVect.push_back(value);
		}
		clusterCenterVect.push_back(stuffVect);
		stuffVect.clear();
	}
}

bool mmBehaviourDetector::TestFrame(Mat frame, vector<Point> particleVect, string pathVideo){
	if(particleVect.size() != 0){
		needToTest = true;
		Mat imm = frame.clone();
		frameInWindow.push_back(imm);
		particleInWindow.push_back(particleVect);
		imm.release();

		//if the buffer is full-->start processing the frames
		if(frameInWindow.size() == windowLength){

			//process one frame each frameStep frames
			if(totalFrameProcessed%frameStep == 0){
				//create the window file
				int numParticle = CreateParticle(pathVideo);

				//compute behaviour
				bool isAnomaly = getAnomaly(numParticle);

				if(isAnomaly)
					countAnomaly++;
				else
					countAnomaly=0;

				if(countAnomaly>=numFrameAnomaly && isAnomaly)
					systemAnomaly = true;
				else if(countAnomaly!=0 && !isAnomaly){
					countAnomaly = 0;
					systemAnomaly = false;
				}
			}
			frameInWindow.erase(frameInWindow.begin());
			particleInWindow.erase(particleInWindow.begin());
		}
	}
	else if(particleVect.size() == 0 && needToTest){
		needToTest = false;
		systemAnomaly = false;
		frameInWindow.clear();
		particleInWindow.clear();
		countAnomaly=0;

	}
	totalFrameProcessed++;

	return systemAnomaly;
}


int mmBehaviourDetector::CreateParticle(string pathVideo){
	//video file
	VideoWriter video = VideoWriter(pathVideo, videoProp.character, videoProp.frameRate, Size(videoProp.width,videoProp.height), videoProp.isColor);
	int dimension = 0;

	for(int i=0; i<frameInWindow.size(); i++){
		if(video.isOpened())
			video << frameInWindow[i];

		frameNumber.push_back(i);
		dimension+=particleInWindow[i].size();
	}

	return dimension;
}

bool mmBehaviourDetector::getAnomaly(int numParticle){
	bool isAnomaly = false;

	//compute hogFeatures
	vector<vector<float> > hogFeat = computeHugMod("test.avi",numParticle,dimensionality);

	//compute histogram
	Mat histTest = Mat::zeros(1,clusterBins,CV_32F);
	for(int i=0; i<hogFeat.size(); i++){
		Mat dist = Mat::zeros(1,clusterBins,CV_32F);

		for(int j=0; j<clusterBins; j++){
			vector<float> diffVect;
			absdiff(hogFeat[i],clusterCenterVect[j],diffVect);
			pow(diffVect,2,diffVect);
			double sumDiffvect = std::accumulate(diffVect.begin(),diffVect.end(),0.0);
			dist.at<float>(0,j) = sumDiffvect;
			diffVect.clear();
		}
		double minVal;
		Point minValPoint;
		minMaxLoc(dist,&minVal,0,&minValPoint,0);
		histTest.at<float>(0,minValPoint.x) = histTest.at<float>(0,minValPoint.x) + 1;

		dist.release();
	}

	//normalization
	Mat histTestPower = Mat::zeros(histTest.rows,histTest.cols,CV_32F);
	pow(histTest,2,histTestPower);
	Scalar histSum = sum(histTestPower);
	histTest = histTest/sqrt(histSum[0]);
	histTestPower.release();

	//classification
	mod.load(svmModelString.c_str());
	isAnomaly = mod.predict(histTest,false);
	cout << "Frame= " << totalFrameProcessed << " -->risposta= " << isAnomaly << ":" << mod.predict(histTest,true) <<  endl;

	histTest.release(); hogFeat.clear();
	return isAnomaly;
}

vector<vector<float> > mmBehaviourDetector::computeHugMod(string videoFile, int lenght, int dimensionality){
	vector<float> stuffVect(dimensionality,0);
	vector<vector<float> > vecFeatures(lenght,stuffVect);
	stuffVect.clear();
	int countFeatures = -1;

	try {
		std::size_t xyNCells, tNCells, nPix;
		double normThreshold, cutZero;
		double sigmaSupport, tauSupport;
		std::string quantTypeStr;
		std::size_t polarBinsXY, polarBinsT;
		bool fullOrientation, gaussWeight, overlapCells, normGlobal, l1Norm;

		std::size_t xyNStride;
		std::size_t tNStride;
		double xyScaleFactor;
		double tScaleFactor;
		double scaleOverlap;
		std::size_t bufferLength;
		double imgScaleFactor;
		std::size_t seed;
		std::size_t nSubsample;

		po::options_description argOpt("command line arguments");
		argOpt.add_options()
																	("video-file", po::value<string>(), "video file to process");

		po::options_description generalOpt("general options");
		generalOpt.add_options()
																	("position-file,p", po::value<string>(), "position file for sampling features; each line represents one feature position and consists of 5 elements (they may be floating points) which are: '<x-position> <y-position> <frame-number> <xy-scale> <t-scale>'; lines beginning with '#' are ignored")
																	("track-file,t", po::value<string>(), "track file for sampling features; each line represents a bounding box at a given frame; a line consists of 5 elements (they may be floating points) which are: '<frame-number> <top-left-x-position> <top-left-y-position> <width> <height>'; lines beginning with '#' are ignored; for a given position file, features with a center point that lies outside the set of given bounding boxes are ignored; for dense sampling, the --xy-nstride and --t-nstride options will be relative to the track (length and bounding boxes)")
																	("force,f", "force computation of features, no suppression if features do not fit completely in the full video/buffer")
																	("n-subsample", po::value<std::size_t>(&nSubsample)->default_value(1), "subsample every n-th feautre (in average)")
																	("seed", po::value<std::size_t>(&seed)->default_value(time(0), "timestamp"), "seed for subsampling (default current time)");

		po::options_description descriptorOpt("descriptor options");
		descriptorOpt.add_options()
																	("xy-ncells", po::value<std::size_t>(&xyNCells)->default_value(2), "number of HOG3D cells in x and y direction")
																	("t-ncells", po::value<std::size_t>(&tNCells)->default_value(5), "number of HOG3D cells in time")
																	("npix", po::value<std::size_t>(&nPix)->default_value(4), "number of hyper pixel support for each HOG3D cell, i.e., the histogram of a cell is computed for SxSxS subblocks")
																	("norm-threshold,n", po::value<double>(&normThreshold)->default_value(0.1, "0.1"), "suppress features with a descriptor L2-norm lower than the given threshold")
																	("cut-zero", po::value<double>(&cutZero)->default_value(0.25, "0.25"), "descriptor vector is normalized, its values are limited to given value, and the vector is renormalized again")
																	("sigma-support", po::value<double>(&sigmaSupport)->default_value(24), "on the original scale, sigma determines the size of the region around a sampling point for which the descriptor is computed; for a different scale, this size is given by: <characteristic-scale>*<sigma-support>; sigma is the support in x- and y-direction")
																	("tau-support", po::value<double>(&tauSupport)->default_value(12), "similar to 'sigma-support'; tau is the support in time")
																	("quantization-type,P", po::value<string>(&quantTypeStr)->default_value("polar"), "method that shall be taken for 3D gradient quantization: 'icosahedron' (20 faces=bins), 'dodecahedron' (12 faces=bins), 'polar' (binning on polar coordinates, you can specify the binning)")
																	("polar-bins-xy", po::value<std::size_t>(&polarBinsXY)->default_value(5), "number of bins for the XY-plane orientation using polar coordinate quantization (has either full or half orientation)")
																	("polar-bins-t", po::value<std::size_t>(&polarBinsT)->default_value(3), "number of bins for the XT-plane orientation using polar coordinate quantization (has always half orientation")
																	("full-orientation,F", po::value<bool>(&fullOrientation)->default_value(false), "By default, the half orientation is used (thus resulting in half the number of bins); if this flag is set, only the full sphere is used for quantization, thus doubling the number of bins")
																	("gauss-weight,G", po::value<bool>(&gaussWeight)->default_value(false), "By default, each (hyper) pixel has a weight = 1; this flag enables Gaussian weighting similar to the SIFT descriptor")
																	("overlap-cells,O", po::value<bool>(&overlapCells)->default_value(false), "Given this flag, cells in the descriptor will be 50% overlapping")
																	("norm-global,N", po::value<bool>(&normGlobal)->default_value(false), "By default, each cell in the descriptor is normalized; given this flag, normalization is carried out on the complete descriptor")
																	("l1-norm", po::value<bool>(&l1Norm)->default_value(false), "Given this flag, the each cell desriptor (or the full descriptor if given '--norm-global') will be normalized with L1 norm; by default normalization is done using L2-norm");

		po::options_description denseOpt("dense sampling options");
		denseOpt.add_options()
																	("xy-nstride", po::value<std::size_t>(&xyNStride), "how many features are sampled in x/y direction on the smallest scale (specify either xy-nstride or xy-stride)")
																	("xy-stride", po::value<double>(), "specifies the stride in x/y direction (in pixel) on the smallest scale (specify either xy-nstride or xy-stride)")
																	("xy-max-stride", po::value<double>(), "specifies the maximum stride (and indirectly its scale) for x/y")
																	("xy-max-scale", po::value<double>(), "specifies the maximum scale for x/y")
																	("xy-scale", po::value<double>(&xyScaleFactor)->default_value(sqrt(2), "sqrt(2)"), "scale factor for different scales in x/y direction")
																	("t-nstride", po::value<std::size_t>(&tNStride), "how many features are sampled in time on the smallest scale (specify either t-nstride or t-stride)")
																	("t-stride", po::value<double>(), "specifies the stride in t direction (in frames) on the smalles scale (specify either t-nstride or t-stride)")
																	("t-max-stride", po::value<double>(), "specifies the maximum stride (and indirectly its scale) for t")
																	("t-max-scale", po::value<double>(), "specifies the maximum scale for t")
																	("t-scale", po::value<double>(&tScaleFactor)->default_value(sqrt(2), "sqrt(2)"), "scale factor for different scales in time")
																	("scale-overlap", po::value<double>(&scaleOverlap)->default_value(2, "2"), "controls overlap of adjacent features, scales size of 3D box; for a factor of 1, features will be adjacent, any factor greater than 1 will cause overlapping features; a factor of 2 will double the size of the box (along each dimension) and thus will result in an overlap of 50%");

		po::options_description videoOpt("video options");
		videoOpt.add_options()
																	("start-frame,S", po::value<std::size_t>(), "if given, feature extraction starts at given frame")
																	("end-frame,E", po::value<std::size_t>(), "if given, feature extraction ends at given frame (including this frame)")
																	("start-time,s", po::value<double>(), "if given, feature extraction starts at the first frame at or after the given time (in seconds)")
																	("end-time,e", po::value<double>(), "if given, feature extraction ends at the last frame before or at the given time (in seconds)")
																	("buffer-length,b", po::value<std::size_t>(&bufferLength)->default_value(100), "length of the internal video buffer .. controls the memory usage as well as the maximal scale in time for features")
																	("simg", po::value<double>(&imgScaleFactor)->default_value(1), "scale the input video by this given factor");

		po::options_description desc;
		desc.add(argOpt).add(generalOpt).add(descriptorOpt).add(denseOpt).add(videoOpt);
		po::positional_options_description p;
		p.add("video-file", 1);

		string stringZero = "/Debug/Up";
		string stringOne = "-p";
		string stringTwo = "tt.txt";
		string stringFour = videoFile;
		const char* argv[4];
		argv[0] = stringZero.c_str();
		argv[1] = stringOne.c_str();
		argv[2] = stringTwo.c_str();
		argv[3] = stringFour.c_str();
//		argv[1] = stringFour.c_str();

		po::variables_map vm;
		po::store(po::command_line_parser(4, argv).options(desc).positional(p).run(), vm);
		po::notify(vm);

		// check whether video file has been given
		if (!vm.count("video-file")) {
			cout << endl << "You need to specify a video file!" << endl << endl;
		}

		// get the correct type of orientation quantization
		FastHog3DComputer::QuantizationType quantizationType = FastHog3DComputer::Icosahedron;
		if (boost::algorithm::iequals(quantTypeStr, "icosahedron"))
			quantizationType = FastHog3DComputer::Icosahedron;
		else if (boost::algorithm::iequals(quantTypeStr, "dodecahedron"))
			quantizationType = FastHog3DComputer::Dodecahedron;
		else if (boost::algorithm::iequals(quantTypeStr, "polar"))
			quantizationType = FastHog3DComputer::PolarBinning;
		else
			throw std::runtime_error("unsupported platonic solid: " + quantTypeStr);
		boost::scoped_ptr<Video> video(new Video(vm["video-file"].as<string>(), scaleX, scaleY));
//		// init variables
//		if(frameInWindow.size() == 0)
//			boost::scoped_ptr<Video> video(new Video(vm["video-file"].as<string>(), scaleX, scaleY));
//		else
//			boost::scoped_ptr<Video> video(new Video(frameInWindow, scaleX, scaleY));

		boost::scoped_ptr<FastVideoGradientComputer> gradComputer(new FastVideoGradientComputer(video.get(), bufferLength, imgScaleFactor));

		FastHog3DComputer hogComputer(gradComputer.get(), quantizationType, polarBinsXY, polarBinsT,
				xyNCells, tNCells, nPix, normThreshold, cutZero, fullOrientation,
				gaussWeight, overlapCells, !normGlobal, l1Norm);

		// get start and end frame
		std::size_t startFrame = 0;
		std::size_t endFrame = video->numOfFrames() - 1;
		double frameRate = video->getFrameRate();
		if (0.1 > frameRate || 100.0 < frameRate) {
			cerr << "Error! Invalid frame rate: " << frameRate << endl;
			cerr << "Aborting!" << endl;
		}
		if (vm.count("start-frame")) {
			startFrame = vm["start-frame"].as<std::size_t>();
			cerr << "start frame: " << startFrame << endl;
		}
		if (vm.count("end-frame")) {
			endFrame = vm["end-frame"].as<std::size_t>();
			video->probeFrame(static_cast<Video::FrameIndex>(endFrame));
			cerr << "end frame: " << endFrame << endl;
		}
		if (vm.count("start-time")) {
			startFrame = static_cast<std::size_t>(std::max(0.0, round(vm["start-time"].as<double>() * frameRate)));
			cerr << "start time: " << vm["start-time"].as<double>() << " => frame " << startFrame << endl;
		}
		if (vm.count("end-time")) {
			video->probeFrame(vm["end-time"].as<double>());
			endFrame = static_cast<std::size_t>(std::max(0.0, round(vm["end-time"].as<double>() * frameRate)));
			cerr << "end time: " << vm["end-time"].as<double>() << " => frame " << endFrame << endl;
		}
		if (endFrame <= startFrame) {
			cerr << "The ending point should have a higher value than the starting point! Aborting!" << endl;
		}

		// init random seed
		srand(seed);

		// read in the track file if it is given
		std::multimap<int, Box<double> > track;

		// get dimensions of the video to process
		int width = gradComputer->getWidth();
		int height = gradComputer->getHeight();
		std::size_t length = endFrame - startFrame + 1;
		if (endFrame <= startFrame) {
			cerr << "Nothing to do!" << endl;
		}

		// read in line by line
		std::string line;
		std::vector<double> tmpVec(5);
		std::list<Position3D> positionList;

		for(int i=0; i<particleInWindow.size(); i++){
			for(int j=0; j<particleInWindow[i].size(); j++){
				tmpVec[0] = particleInWindow[i][j].x;
				tmpVec[1] = particleInWindow[i][j].y;
				tmpVec[2] = frameNumber[i];
				tmpVec[3] = sigma;
				tmpVec[4] = tau;
				positionList.push_back(Position3D(tmpVec[0], tmpVec[1], tmpVec[2], tmpVec[3], tmpVec[4]));
			}
		}
		tmpVec.clear();

		// sort the list according to time
		positionList.sort(Position3DTLess());

		// compute descriptors for all positions
		FastHog3DComputer::VectorType vec;
		std::list<Box<double> > bboxes; // list will contain all bounding boxes of a frame
		bboxes.push_back(Box<double>(0, 0, width, height)); // without a track, the full image is the only bounding box

		int count =0;
		for (BOOST_AUTO(iPos, positionList.begin()); iPos != positionList.end(); ++iPos) {
			// check whether this position lies in the interval given by start and end frame
			int frame = static_cast<int>(round(iPos->t));
			if (frame < startFrame || frame > endFrame)
				continue;

			// advance to the specific frame position
			gradComputer->gotoFrame(frame);

			// we might have multiple positions in the track (in case we have a loose collection of bounding boxes) therefore we need to iterate over
			// all bounding boxes at the current time instant
			for (BOOST_AUTO(iBox, bboxes.begin()); iBox != bboxes.end(); ++iBox) {
				// check whether this position lies in the track
				if (/*vm.count("track-file") && */!iBox->contains(iPos->x, iPos->y))
					continue;
				count++;
				// get the bounding box (either specified or from <x,y,t,sigma,tau> values)
				Box3D box;
				if (iPos->box3D)
					box = *(iPos->box3D);
				else {
					box.width = std::max(hogComputer.getMinWidth(), iPos->sigma * sigmaSupport);
					box.height = std::max(hogComputer.getMinHeight(), iPos->sigma * sigmaSupport);
					box.length = std::max(hogComputer.getMinLength(), iPos->tau * tauSupport);
					box.x = iPos->x - box.width * 0.5;
					box.y = iPos->y - box.height * 0.5;
					box.t = iPos->t - box.length * 0.5;
				}

				//cerr << "#   " << box.x << " " << box.y << " " << box.t << " " << box.width << " " << box.height << " " << box.length << endl;
				double xNormalized = std::min(0.999, (iPos->x - iBox->getLeft()) / iBox->getWidth());
				double yNormalized = std::min(0.999, (iPos->y - iBox->getTop()) / iBox->getHeight());

				// check whether box is inside video
				if (!gradComputer->isInBuffer(box) && !vm.count("force"))
					continue;

				// check for sampling
				if (nSubsample > 1 && (rand() % nSubsample) > 0)
					continue;

				vec = hogComputer.getHog3D(box);

				if (vec.size() > 0) {
					countFeatures++;
					for(int i=0; i<dimensionality; i++){
						vecFeatures[countFeatures][i] = vec[i];
					}
				}
			}
		}
		positionList.clear(); bboxes.clear(); track.clear();
	}
	catch(boost::bad_lexical_cast &e) {
		cerr << endl << "Problem parsing command line arguments: " << e.what() << endl << endl;
	}
	catch(std::exception& e) {
		cerr << endl << "Error during execution: " << e.what() << endl << endl;
	}

	return vecFeatures;
}

vector<vector<float> > mmBehaviourDetector::getClasterCenterVect(){
	return clusterCenterVect;
}

CvSVM mmBehaviourDetector::getSVMmodel(){
	return mod;
}

int mmBehaviourDetector::getSigma(){
	return sigma;
}

int mmBehaviourDetector::getTau(){
	return tau;
}

int mmBehaviourDetector::getGridGranularity(){
	return gridGranularity;
}
}
}
