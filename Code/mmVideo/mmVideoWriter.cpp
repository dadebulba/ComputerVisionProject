/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmVideoWriter.cpp
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/


#include "../mmLib.h"

namespace mmLib {
namespace mmVideo{
mmVideoWriter::mmVideoWriter() {
	_outputFileName = "out.avi";
	_fps = 25;
	_codec = CV_FOURCC('D','I','V','X');
	_init=false;
	_bgrFrame = true;

}
mmVideoWriter::mmVideoWriter(std::string outputFileName, int fps) {
	_outputFileName = outputFileName;
	_fps = fps;
	_codec = CV_FOURCC('D','I','V','X');
	_init=false;
	_bgrFrame = true;

}
mmVideoWriter::mmVideoWriter(std::string outputFileName, int fps, int codec, bool bgrFrame) {
	_outputFileName = outputFileName;
	_fps = fps;
	_codec = codec;
	_init=false;
	_bgrFrame = bgrFrame;

}
void mmVideoWriter::addFrame(cv::Mat frame)
{
	if(!_init)
	{
		bool colored=false;
		if(frame.channels()>1)
			colored=true;
		_videoWriter = cv::VideoWriter(_outputFileName,_codec,_fps,frame.size(),colored);
		_init=true;
	}
	if(_bgrFrame)
	{
		cv::Mat coloredFrame(frame.rows, frame.cols, CV_8UC3);
		int from_to[] = { 0,2,  1,1,  2,0};
		cv::mixChannels(&frame,1,&coloredFrame,1,from_to,3);
		_videoWriter << coloredFrame;
	}
	else
	{
		_videoWriter << frame;
	}

}

mmVideoWriter::~mmVideoWriter() {
	_videoWriter.~VideoWriter();
}

}
}
