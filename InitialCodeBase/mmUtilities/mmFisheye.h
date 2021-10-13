/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmFisheye.h
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/


#ifndef mmFISHEYE_H_
#define mmFISHEYE_H_
namespace mmLib{
namespace mmUtilities{
class mmFisheye {
public:
	mmFisheye();
	mmFisheye(std::string intrinsicFileName, std::string distortionFileName);
	virtual ~mmFisheye();
	//Signal
	/**
	* New OpenCv Frame signal, accept cv::Mat class
	*/
	typedef boost::signals2::signal<void(cv::Mat)> signalT;
	/**
	* New OpenCV Frame Event
	* @param subscriber Subscriber function, must have cv::Mat class for input
	*/
	boost::signals2::connection newOpenCVUndistortedFrameEvent(const signalT::slot_type &subscriber)
	{
		return _newOpenCVUndistortedFrameSignal.connect(subscriber);
	}
	void newOpenCVDistortedFrame(cv::Mat distortedFrame);
	int CreateConfigurationsFiles(std::string imgDir, std::string extension, std::string cameraName, cv::Size2i boardSize);

private:
	signalT _newOpenCVUndistortedFrameSignal;
	cv::Mat _distortionMatrix;
	cv::Mat _intrinsicMatrix;
	CvMat *_intrinsic;
	CvMat *_distortion;
	bool _cropImage;
	bool _canDoConversion;
};
}
}
#endif /* MMFISHEYE_H_ */
