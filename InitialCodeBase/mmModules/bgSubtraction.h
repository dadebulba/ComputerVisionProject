/*
 * bgSubtraction.h
 *
 *  Created on: Nov 24, 2010
 *      Author: paolo
 */

#ifndef BGSUBTRACTION_H_
#define BGSUBTRACTION_H_

#include <opencv/cvaux.h>
#include <opencv/cvaux.hpp>
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION < 2
#include <opencv/cxtypes.h>
#include <opencv/cvvidsurv.hpp>
#endif

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2
#include "opencv2/core/types_c.h"
#include "opencv2/legacy/blobtrack.hpp"
#endif

#include <opencv/highgui.h>


namespace mmLib {
namespace mmModules{

typedef void (CV_CDECL * CvReleaseBGGaussModel)( struct MmGaussBGModel** bg_model );
typedef int (CV_CDECL * CvUpdateBGGaussModel)( IplImage* curr_frame, struct MmGaussBGModel* bg_model,
                                              double learningRate );

typedef struct MmGaussBGModel
{
	int             type; /*type of BG model*/                                      \
	CvReleaseBGGaussModel release;                                                   \
	CvUpdateBGGaussModel update;                                                     \
	IplImage*       background;   /*8UC3 reference background image*/               \
	IplImage*       foreground;   /*8UC1 foreground image*/                         \
	IplImage**      layers;       /*8UC3 reference background image, can be null */ \
	int             layer_count;  /* can be zero */                                 \
	CvMemStorage*   storage;      /*storage for foreground_regions*/                \
	CvSeq*          foreground_regions; /*foreground object contours*/
    CvGaussBGStatModelParams   params;
    CvGaussBGPoint*            g_point;
    int                        countFrames;
}MmGaussBGModel;

class mmGaussianFGDetector {
public:
	mmGaussianFGDetector();
	mmGaussianFGDetector(CvGaussBGStatModelParams params);
	~mmGaussianFGDetector();

	IplImage* GetMask();

	IplImage* GetBackground();
	// Process current image:
	void    Process(IplImage* pImg);
	// Release foreground detector:
	void    Release();

	void ChangeParams(CvGaussBGStatModelParams newParam);

	//CvGaussBGStatModelParams    mmParams;
protected:
	CvGaussBGStatModelParams    mmParams;
	MmGaussBGModel*  mmpFG;
};

class mmBGSubtractorMOG : public cv::BackgroundSubtractorMOG {
public:
	mmBGSubtractorMOG();
	void setBgModel(cv::Mat bg ){bgmodel=bg;}
	cv::Mat getBgModel(){return bgmodel;}
	void setSize(cv::Size sz){frameSize=sz;}
	void setFrameType(int type){frameType=type;}
	void setNFrames(int nf){nframes=nf;}
	int getNFrames(){return nframes;}
	void setHistory(int hs){history=hs;}
	int getHistory(){return history;}
	void setNMixtures(int nm){nmixtures=nm;}
	void setVarThreshold(double th){varThreshold=th;}
	void setBackgroundRatio(double br){backgroundRatio=br;}
	virtual void operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate=0);

private:
	void mmProcess8uC1( mmBGSubtractorMOG& obj, const cv::Mat& image, cv::Mat& fgmask, double learningRate );
	void mmProcess8uC3( mmBGSubtractorMOG& obj, const cv::Mat& image, cv::Mat& fgmask, double learningRate );

};

MmGaussBGModel* mmCreateGaussianBGModel( IplImage* first_frame, CvGaussBGStatModelParams parameters );
static int mmUpdateGaussianBGModel( IplImage* curr_frame, MmGaussBGModel*  bg_model);
static void mmReleaseGaussianBGModel( MmGaussBGModel** bg_model );




} //mmModules namespace

} //mmLib namespace

#endif /* BGSUBTRACTION_H_ */


