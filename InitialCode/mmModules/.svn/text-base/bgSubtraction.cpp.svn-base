/*
 * bgSubtraction.cpp
 *
 *  Created on: Nov 24, 2010
 *      Author: paolo
 */

#include "../mmLib.h"
#include <stdio.h>
#include <highgui.h>

#define 	MM_DEBUG		0

using namespace cv;

namespace mmLib {
namespace mmModules {

typedef struct MyCvGaussBGValues
{
    float match_sum;
    float weight;
    float mean[3];
    float variance[3];
}
MyCvGaussBGValues;

mmBGSubtractorMOG::mmBGSubtractorMOG(){
	cv::BackgroundSubtractorMOG();
}

void mmBGSubtractorMOG::operator()(const cv::Mat& image, cv::Mat& fgmask, double learningRate)
{

	bool needToInitialize = nframes == 0 || learningRate >= 1 || image.size() != frameSize || image.type() != frameType;

	if( needToInitialize )
		initialize(image.size(), image.type());

	CV_Assert( image.depth() == CV_8U );
	fgmask.create( image.size(), CV_8U );

	++nframes;
	learningRate = learningRate >= 0 && nframes > 1 ? learningRate : 1./cv::min( nframes, history );
	CV_Assert(learningRate >= 0);

	if( image.type() == CV_8UC1 )
		mmProcess8uC1( *this, image, fgmask, learningRate );
	else if( image.type() == CV_8UC3 )
		mmProcess8uC3( *this, image, fgmask, learningRate );
	else
		CV_Error( CV_StsUnsupportedFormat, "Only 1- and 3-channel 8-bit images are supported in BackgroundSubtractorMOG" );
}

template<typename VT> struct MixData
{
	float sortKey;
	float weight;
	VT mean;
	VT var;
};

void mmBGSubtractorMOG::mmProcess8uC1( mmBGSubtractorMOG& obj, const cv::Mat& image, cv::Mat& fgmask, double learningRate )
{
	printf("Remark: 1 color channel video \n");
	int x, y, k, k1, rows = image.rows, cols = image.cols;
	float alpha = (float)learningRate, TT = (float)obj.backgroundRatio, vT = (float)obj.varThreshold;
	int K = obj.nmixtures;
	MixData<float>* mptr = (MixData<float>*)obj.bgmodel.data;

	const float w0 = (float)CV_BGFG_MOG_WEIGHT_INIT;
	const float sk0 = (float)(w0/CV_BGFG_MOG_SIGMA_INIT);
	const float var0 = (float)(CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT);
	const float minVar = (float)(obj.noiseSigma*obj.noiseSigma);

	for( y = 0; y < rows; y++ )
	{
		const uchar* src = image.ptr<uchar>(y);
		uchar* dst = fgmask.ptr<uchar>(y);

		if( alpha > 0 )
		{
			for( x = 0; x < cols; x++, mptr += K )
			{
				float wsum = 0;
				float pix = src[x];
				int kHit = -1, kForeground = -1;

				for( k = 0; k < K; k++ )
				{
					float w = mptr[k].weight;
					wsum += w;
					if( w < FLT_EPSILON )
						break;
					float mu = mptr[k].mean;
					float var = mptr[k].var;
					float diff = pix - mu;
					float d2 = diff*diff;
					if( d2 < vT*var )
					{
						wsum -= w;
						float dw = alpha*(1.f - w);
						mptr[k].weight = w + dw;
						mptr[k].mean = mu + alpha*diff;
						var = cv::max(var + alpha*(d2 - var), minVar);
						mptr[k].var = var;
						mptr[k].sortKey = w/sqrt(var);

						for( k1 = k-1; k1 >= 0; k1-- )
						{
							if( mptr[k1].sortKey >= mptr[k1+1].sortKey )
								break;
							std::swap( mptr[k1], mptr[k1+1] );
						}

						kHit = k1+1;
						break;
					}
				}

				if( kHit < 0 ) // no appropriate gaussian mixture found at all, remove the weakest mixture and create a new one
						{
					kHit = k = cv::min(k, K-1);
					wsum += w0 - mptr[k].weight;
					mptr[k].weight = w0;
					mptr[k].mean = pix;
					mptr[k].var = var0;
					mptr[k].sortKey = sk0;
						}
				else
					for( ; k < K; k++ )
						wsum += mptr[k].weight;

				float wscale = 1.f/wsum;
				wsum = 0;
				for( k = 0; k < K; k++ )
				{
					wsum += mptr[k].weight *= wscale;
					mptr[k].sortKey *= wscale;
					if( wsum > TT && kForeground < 0 )
						kForeground = k+1;
				}

				dst[x] = (uchar)(-(kHit >= kForeground));
			}
		}
		else
		{
			for( x = 0; x < cols; x++, mptr += K )
			{
				float pix = src[x];
				int kHit = -1, kForeground = -1;

				for( k = 0; k < K; k++ )
				{
					if( mptr[k].weight < FLT_EPSILON )
						break;
					float mu = mptr[k].mean;
					float var = mptr[k].var;
					float diff = pix - mu;
					float d2 = diff*diff;
					if( d2 < vT*var )
					{
						kHit = k;
						break;
					}
				}

				if( kHit >= 0 )
				{
					float wsum = 0;
					for( k = 0; k < K; k++ )
					{
						wsum += mptr[k].weight;
						if( wsum > TT )
						{
							kForeground = k+1;
							break;
						}
					}
				}

				dst[x] = (uchar)(kHit < 0 || kHit >= kForeground ? 255 : 0);
			}
		}
	}

}

void mmBGSubtractorMOG::mmProcess8uC3( mmBGSubtractorMOG& obj, const Mat& image, Mat& fgmask, double learningRate )
{
	//TODO: Modificare la funzione in modo che la bgSubtraction sia selettiva, per ora e' una replica di Opencv.
	//printf("Entra in mmProcess8uC3 \n");
	//FILE * fileOut;
	//fileOut = fopen("output.txt", "a");
	int x, y, k, k1, rows = image.rows, cols = image.cols;
	float alpha = (float)learningRate;float TT = (float)obj.backgroundRatio; float vT = (float)obj.varThreshold;
	int K = obj.nmixtures;

	const float w0 = (float)CV_BGFG_MOG_WEIGHT_INIT;
	const float sk0 = (float)(w0/CV_BGFG_MOG_SIGMA_INIT*sqrt(3.));
	const float var0 = (float)(CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT);
	const float minVar = (float)(obj.noiseSigma*obj.noiseSigma);
	MixData<Vec3f>* mptr = (MixData<Vec3f>*)obj.bgmodel.data;

	for( y = 0; y < rows; y++ )
	{
		const uchar* src = image.ptr<uchar>(y);
		uchar* dst = fgmask.ptr<uchar>(y);

		if( alpha > 0 )
		{
			for( x = 0; x < cols; x++, mptr += K )
			{
				float wsum = 0;
				Vec3f pix(src[x*3], src[x*3+1], src[x*3+2]);
				//fprintf(fileOut,"%d/%d/%d ", int(pix[0]), int(pix[1]), int(pix[2]));
				int kHit = -1, kForeground = -1;

				for( k = 0; k < K; k++ )
				{
					float w = mptr[k].weight;
					wsum += w;
					if( w < FLT_EPSILON )
						break;
					Vec3f mu = mptr[k].mean;
					Vec3f var = mptr[k].var;
					Vec3f diff = pix - mu;
					float d2 = diff.dot(diff);
					if( d2 < vT*(var[0] + var[1] + var[2]) )
					{
						wsum -= w;
						float dw = alpha*(1.f - w);
						mptr[k].weight = w + dw;
						mptr[k].mean = mu + alpha*diff;
						var = Vec3f(max(var[0] + alpha*(diff[0]*diff[0] - var[0]), minVar),
								max(var[1] + alpha*(diff[1]*diff[1] - var[1]), minVar),
								max(var[2] + alpha*(diff[2]*diff[2] - var[2]), minVar));
						mptr[k].var = var;
						mptr[k].sortKey = w/sqrt(var[0] + var[1] + var[2]);

						for( k1 = k-1; k1 >= 0; k1-- )
						{
							if( mptr[k1].sortKey >= mptr[k1+1].sortKey )
								break;
							std::swap( mptr[k1], mptr[k1+1] );
						}

						kHit = k1+1;
						break;
					}
				}

				if( kHit < 0 ) // no appropriate gaussian mixture found at all, remove the weakest mixture and create a new one
						{
					kHit = k = min(k, K-1);
					wsum += w0 - mptr[k].weight;
					mptr[k].weight = w0;
					mptr[k].mean = pix;
					mptr[k].var = Vec3f(var0, var0, var0);
					mptr[k].sortKey = sk0;
						}
				else
					for( ; k < K; k++ )
						wsum += mptr[k].weight;

				float wscale = 1.f/wsum;
				wsum = 0;
				for( k = 0; k < K; k++ )
				{
					wsum += mptr[k].weight *= wscale;
					mptr[k].sortKey *= wscale;
					if( wsum > TT && kForeground < 0 )
						kForeground = k+1;
				}

				dst[x] = (uchar)(-(kHit >= kForeground));
			}
			//fprintf(fileOut,"\n");
		}
		else
		{
			for( x = 0; x < cols; x++, mptr += K )
			{
				Vec3f pix(src[x*3], src[x*3+1], src[x*3+2]);
				int kHit = -1, kForeground = -1;

				for( k = 0; k < K; k++ )
				{
					if( mptr[k].weight < FLT_EPSILON )
						break;
					Vec3f mu = mptr[k].mean;
					Vec3f var = mptr[k].var;
					Vec3f diff = pix - mu;
					float d2 = diff.dot(diff);
					if( d2 < vT*(var[0] + var[1] + var[2]) )
					{
						kHit = k;
						break;
					}
				}

				if( kHit >= 0 )
				{
					float wsum = 0;
					for( k = 0; k < K; k++ )
					{
						wsum += mptr[k].weight;
						if( wsum > TT )
						{
							kForeground = k+1;
							break;
						}
					}
				}

				dst[x] = (uchar)(kHit < 0 || kHit >= kForeground ? 255 : 0);
			}
		}
	}
	// To show the foreground image uncomment this:
	//	imshow("intern", fgmask);
}

// Metodi di Create, Release e Update del modulo - non si puo' iserire all'interno della classe perche' server la reference dell'istanza.
void mmReleaseGaussianBGModel( MmGaussBGModel** bg_model )
{

	//Release model
	if( !bg_model )
		CV_Error( CV_StsNullPtr, "" );

	if( *bg_model )
	{
		delete (cv::Mat*)((*bg_model)->g_point);
		cvReleaseImage( &(*bg_model)->background );
		cvReleaseImage( &(*bg_model)->foreground );
		cvReleaseMemStorage(&(*bg_model)->storage);
		memset( *bg_model, 0, sizeof(**bg_model) );
		delete *bg_model;
		*bg_model = 0;
	}
	//
}


int mmUpdateGaussianBGModel( IplImage* curr_frame, MmGaussBGModel*  bg_model)
{
	//printf("entro nella Update \n");
	int region_count = 0;

	cv::Mat image = cv::cvarrToMat(curr_frame), mask = cv::cvarrToMat(bg_model->foreground);

	mmBGSubtractorMOG mog;
	//mog.bgmodel = *(cv::Mat*)bg_model->g_point;
	mog.setBgModel(*(cv::Mat*)bg_model->g_point);
	//mog.frameSize = mog.bgmodel.data ? cv::Size(cvGetSize(curr_frame)) : cv::Size();
	mog.setSize(mog.getBgModel().data ? cv::Size(cvGetSize(curr_frame)) : cv::Size());
	//mog.frameType = image.type();
	mog.setFrameType(image.type());
	//mog.nframes = bg_model->countFrames;
	mog.setNFrames(bg_model->countFrames);
	//mog.history = bg_model->params.win_size;
	mog.setHistory(bg_model->params.win_size);
	//mog.nmixtures = bg_model->params.n_gauss;
	mog.setNMixtures(bg_model->params.n_gauss);
//	mog.varThreshold = bg_model->params.std_threshold;
	mog.setVarThreshold(bg_model->params.std_threshold);
	//mog.backgroundRatio = bg_model->params.bg_threshold;
    mog.setBackgroundRatio(bg_model->params.bg_threshold);
	//printf("Win Size: %d\n", mog.history);
	double lr = (double) mog.getHistory();
	lr = 1/lr;
	//printf("Learning Rate: %f\n", lr);
	mog(image, mask, lr);

	//bg_model->countFrames = mog.nframes;
	bg_model->countFrames = mog.getNFrames();
	if( ((cv::Mat*)bg_model->g_point)->data != mog.getBgModel().data )
		*((cv::Mat*)bg_model->g_point) = mog.getBgModel();

	//foreground filtering

	//filter small regions
	cvClearMemStorage(bg_model->storage);

	/////////////////// Perform Erosion on mask ///////////////////////////////
	Mat dst;
	mask.copyTo(dst);
	IplConvKernel* st_elem;
	//printf("qui arrivo\n");
	st_elem = cvCreateStructuringElementEx(4, 4, 1, 1, CV_SHAPE_ELLIPSE);
	IplImage* src_i = new IplImage( mask.operator _IplImage());
	IplImage* dst_i = new IplImage( dst.operator _IplImage());
	// Apertura
	cvErode(src_i, dst_i, st_elem, 1);
	cvDilate(dst_i, src_i, st_elem, 1);
	// Chiusura
	cvDilate(src_i, dst_i, st_elem, 1);
	cvErode(dst_i, src_i, st_elem, 1);
	//Mat m(dst_i);
	Mat m(src_i);
	//imshow("mask", mask);
#if MM_DEBUG
	imshow("morph", m);
#endif
	////////////////////////////////////////////////////////////////////////////
	CvMat _mask = m;
	cvCopy(&_mask, bg_model->foreground);

	// update background
	int K = bg_model->params.n_gauss;
	int nchannels = bg_model->background->nChannels;
	int height = bg_model->background->height;
	int width = bg_model->background->width;
	MyCvGaussBGValues *g_point = (MyCvGaussBGValues *) ((CvMat*)(bg_model->g_point))->data.ptr;
	MyCvGaussBGValues *mptr = g_point;
	for(int y=0; y<height; y++)
	{
		for (int x=0; x<width; x++, mptr+=K)
		{
			int pos = bg_model->background->widthStep*y + x*nchannels;
			float mean[3] = {0.0, 0.0, 0.0};

			for(int k=0; k<K; k++)
			{
				for(int m=0; m<nchannels; m++)
				{
					mean[m] += mptr[k].weight * mptr[k].mean[m];
				}
			}

			for(int m=0; m<nchannels; m++)
			{
				bg_model->background->imageData[pos+m] = (uchar) (mean[m]+0.5);
			}
		}
	}



	return region_count;
}

MmGaussBGModel* mmCreateGaussianBGModel( IplImage* first_frame, CvGaussBGStatModelParams parameters ){
	//printf("Entra in mmCreateGaussianBGModel\n");
	CvGaussBGStatModelParams params;

	CV_Assert( CV_IS_IMAGE(first_frame) );

	//init parameters
	if( parameters.win_size == NULL )
	{                        /* These constants are defined in cvaux/include/cvaux.h: */
		params.win_size      = CV_BGFG_MOG_WINDOW_SIZE;
		params.bg_threshold  = CV_BGFG_MOG_BACKGROUND_THRESHOLD;

		params.std_threshold = CV_BGFG_MOG_STD_THRESHOLD;
		params.weight_init   = CV_BGFG_MOG_WEIGHT_INIT;

		params.variance_init = CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT;
		params.minArea       = CV_BGFG_MOG_MINAREA;
		params.n_gauss       = CV_BGFG_MOG_NGAUSSIANS;
	}
	else
		params = parameters;

	MmGaussBGModel* bg_model = new MmGaussBGModel;
	memset( bg_model, 0, sizeof(*bg_model) );
	bg_model->type = CV_BG_MODEL_MOG;
	bg_model->release = (CvReleaseBGGaussModel)mmReleaseGaussianBGModel;
	bg_model->update = (CvUpdateBGGaussModel)mmUpdateGaussianBGModel;

	bg_model->params = params;

	//prepare storages
	bg_model->g_point = (CvGaussBGPoint*)new cv::Mat();

	bg_model->background = cvCreateImage(cvSize(first_frame->width,
			first_frame->height), IPL_DEPTH_8U, first_frame->nChannels);
	bg_model->foreground = cvCreateImage(cvSize(first_frame->width,
			first_frame->height), IPL_DEPTH_8U, 1);

	bg_model->storage = cvCreateMemStorage();

	bg_model->countFrames = 0;

	mmUpdateGaussianBGModel( first_frame, bg_model);

	//return (CvBGStatModel*)bg_model;
	return bg_model;
}

mmGaussianFGDetector::mmGaussianFGDetector(){

	mmParams.win_size      = CV_BGFG_MOG_WINDOW_SIZE;
	mmParams.bg_threshold  = CV_BGFG_MOG_BACKGROUND_THRESHOLD;

	mmParams.std_threshold = CV_BGFG_MOG_STD_THRESHOLD;
	mmParams.weight_init   = CV_BGFG_MOG_WEIGHT_INIT;

	mmParams.variance_init = CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT;
	mmParams.minArea       = CV_BGFG_MOG_MINAREA;
	mmParams.n_gauss       = CV_BGFG_MOG_NGAUSSIANS;

}

mmGaussianFGDetector::mmGaussianFGDetector(CvGaussBGStatModelParams params){
	mmpFG = NULL;
	mmParams = params;
//	printf("winsize!!: %d\n", mmParams.win_size);
//	fflush(stdout);
}

mmGaussianFGDetector::~mmGaussianFGDetector(){

}

IplImage* mmGaussianFGDetector::GetMask() {
	return mmpFG?mmpFG->foreground:NULL;
}

void   mmGaussianFGDetector::Process(IplImage* pImg) {

	if(mmpFG == NULL)
	{
	    //printf("Nuovo Model;\n");
	    //fflush(stdout);
		mmpFG = mmCreateGaussianBGModel(pImg, mmParams);
	}
	else
	{
	    //printf("Vecchio Model\n");
	    //fflush(stdout);
		// Calls the Update function set in mmCreateGaussianBGModel of mmpFG
		mmUpdateGaussianBGModel( pImg, mmpFG);
	}

}

void   mmGaussianFGDetector::Release() {

	// Calls the Release function set in mmCreateGaussianBGModel of mmpFG
	if(mmpFG)mmReleaseGaussianBGModel( &mmpFG );
}

void mmGaussianFGDetector::ChangeParams(CvGaussBGStatModelParams newParam){

	if (newParam.bg_threshold != mmParams.bg_threshold 		||
			newParam.minArea != mmParams.minArea			||
			newParam.n_gauss != mmParams.n_gauss			||
			newParam.std_threshold != mmParams.std_threshold ||
			newParam.variance_init != mmParams.variance_init ||
			newParam.weight_init != mmParams.weight_init	||
			newParam.win_size != mmParams.win_size)
	{
		mmpFG->params = newParam;
		printf("change Params\n");
		mmParams = newParam;
	}
	else
	{
		//printf("Tuttapposto!\n");
		//fflush(stdout);
	}

}

IplImage* mmGaussianFGDetector::GetBackground(){
	return mmpFG?mmpFG->background:NULL;

}

} // mmModules namespace

} // mmLib namespace

