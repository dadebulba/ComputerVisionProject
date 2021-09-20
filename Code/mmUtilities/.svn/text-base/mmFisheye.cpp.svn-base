/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmFisheye.cpp
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/

#include "../mmLib.h"
namespace fs = boost::filesystem;
namespace mmLib{
namespace mmUtilities{
mmFisheye::mmFisheye() {
	_canDoConversion=false;

}
mmFisheye::mmFisheye(std::string intrinsicFileName, std::string distortionFileName){

	_canDoConversion=true;
	//_cropImage = cropImage;
	_intrinsic = (CvMat*)cvLoad(intrinsicFileName.c_str());
	_distortion = (CvMat*)cvLoad(distortionFileName.c_str());
	_distortionMatrix = _distortion;
	_intrinsicMatrix = _intrinsic;
}

int mmFisheye::CreateConfigurationsFiles(std::string imgDir, std::string extension, std::string cameraName, cv::Size2i boardSize){
	//Path e lista dei file
	unsigned long file_count = 0;
	std::list<std::string> fileList;
	fs::path full_path( fs::initial_path<fs::path>() );
	full_path = fs::system_complete( fs::path( imgDir ) );
	if ( !fs::exists( full_path ) )
	  {
	    std::cerr << "\nNot found: " << full_path.string() << std::endl;
	    return -1;
	  }
	if ( fs::is_directory( full_path ) ) {
		fs::directory_iterator end_iter;
		for ( fs::directory_iterator dir_itr( full_path ); dir_itr != end_iter; ++dir_itr ) {
			if ( fs::is_regular_file( dir_itr->status() ) ) {
				std::string fullPath=dir_itr->path().filename().string();
				size_t found;
				found=fullPath.find(extension);
				if(found!=std::string::npos)
				{
					fileList.push_back(dir_itr->path().string());
					std::cerr << dir_itr->path().filename() << "\n";
					++file_count;
				}
			}
		}
	}
	std::cerr << file_count << std::endl;
	//Immagini e strutture
	cv::Mat nowImageGray, nowImage;
	cv::Mat intrinsicMatrix = cv::Mat(3,3,CV_32FC1);
	cv::Mat distortionCoeffs = cv::Mat(5,1,CV_32FC1);

	std::list<std::string>::iterator nowPath;
	std::vector<std::vector<cv::Point2f> > listCorner;
	std::vector<std::vector<cv::Point3f> > listPoint;
	std::vector<cv::Point3f> points;
	for(int i=0;i<boardSize.area();i++)
	{
		cv::Point3f tmpP = cv::Point3f(i/boardSize.width,i%boardSize.width,0);
		points.push_back(tmpP);
	}
	for(nowPath=fileList.begin();nowPath!=fileList.end();nowPath++)
	{
		std::vector<cv::Point2f> corners;
	//	std::string path = im
		nowImageGray = cv::imread(*nowPath,0);
		nowImage = cv::imread(*nowPath);
		bool find = cv::findChessboardCorners(nowImage,boardSize,corners,CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if(find){
			cv::cornerSubPix(nowImageGray, corners,cv::Size(11,11),cv::Size(-1,-1),cv::TermCriteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
			listCorner.push_back(corners);
			listPoint.push_back(points);
			//std::cerr << "MA CAZZO!" << std::endl;
			//std::cerr << points.size() << " - " << std::endl;
		}

	}
	//Calibrazione

	std::vector<cv::Mat> v1;
	std::vector<cv::Mat> v2;
	cv::calibrateCamera(listPoint,listCorner,nowImage.size(),intrinsicMatrix,distortionCoeffs,v1,v2,0);
	//save
	CvMat cvIntrinsic=intrinsicMatrix;
	CvMat cvDistortion=distortionCoeffs;
	std::string intrinsicFileName =  cameraName+".intrinsic.xml";
	std::string distortionFileName =  cameraName+".distortion.xml";
	cvSave(intrinsicFileName.c_str(),&cvIntrinsic);
	cvSave(distortionFileName.c_str(),&cvDistortion);
	return 1;
}
void mmFisheye::newOpenCVDistortedFrame(cv::Mat distortedFrame){
	if(_canDoConversion)
	{
		cv::Mat undistortedFrame;
		cv::undistort(distortedFrame,undistortedFrame,_intrinsicMatrix,_distortionMatrix);
		_newOpenCVUndistortedFrameSignal(undistortedFrame);
	}
}
mmFisheye::~mmFisheye() {
}
}
}
