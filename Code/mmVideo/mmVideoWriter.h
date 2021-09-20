/*+++++++++++++++++++++++++
  +mmLib 
  +
  +mmVideoWriter.h
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/

#ifndef MMVIDEOWRITER_H_
#define MMVIDEOWRITER_H_

namespace mmLib {
namespace mmVideo{


class mmVideoWriter {
public:
	mmVideoWriter();
	mmVideoWriter(std::string outputFileName, int fps);
	mmVideoWriter(std::string outputFileName, int fps, int codec, bool bgrFrame);
	void addFrame(cv::Mat frame);
	virtual ~mmVideoWriter();
private:
	std::string _outputFileName;
	int _fps;
	int _codec;
	cv::VideoWriter _videoWriter;
	bool _init;
	bool _bgrFrame;
};

}
}
#endif /* MMVIDEOWRITER_H_ */
