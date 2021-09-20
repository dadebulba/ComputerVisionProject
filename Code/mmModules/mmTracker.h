/*
 * mmTracker.h
 *
 *  Created on: Nov 24, 2010
 *      Author: paolo
 */

#ifndef MMTRACKER_H_
#define MMTRACKER_H_

#include <vector>
#include <cv.h>
#include <fstream>


namespace mmLib {
	namespace mmModules{
		// Background Subtraction
		enum BACKGROUND_MODULE {
			//	GAUSSIAN_BACKGROUND, 			// Mixture of Gaussian as implemented on OpenCV
			GAUSSIAN_BACKGROUND_PARAM,
			//CODEBOOK_PARAM,
		};

		// Blob Detection Module
		enum BLOB_DETECTION_MODULE {
			MORPHOLOGICAL_CLEANING,			// Same as CC_ON_FG_MASK + Morphological filters
			CLNF,							// Clnf Method paper: "An improved Deat-time Blob Detection for Visual Surveillance" T.B.Nguyen, S.T.Chung.
		};

		// Tracker Module
		enum TRACKING_MODULE {
			MM_DB_TRACKER,						//TODO: Pensare a come implementarlo, per ora database tracker direttamente in mmTracker.h
			MM_KALMAN_TRACKER,
		};

		class trackerSettings {
			public:
				trackerSettings();
				BACKGROUND_MODULE backgroundMod;
				BLOB_DETECTION_MODULE blobDetMod;
				TRACKING_MODULE trakingMod;
				bool printForeground;
				//////////////////////
				//bool findCenter;
				//bool firstFrame;
				CvGaussBGStatModelParams backgroundParam;

				// Blob Detector Settings
				vector<Rect> nonVisibleAreaParam;
				double toleranceH;
				double toleranceW;
				int sThresh;
				int rectArea;
				int minDetWhidth;
				int minDetHeigth;
				int incresedDetTL;
				int incresedDetBR;
				int windowSizeParam;
		};

		class mmTracker {
			public:
				mmTracker();
				mmTracker(trackerSettings &t);
				mmTracker(trackerSettings &t, appSettings &a);
				virtual ~mmTracker();

				Mat BackgroundSubtraction(Mat frame);
				Mat CodebookBackgroundSubtraction(Mat frame);

				vector<mmLib::mmModules::mmBlob> BlobDetection(Mat frame, Mat foreground, int minBlobWidth, int minBlobHeigth);
				vector<mmLib::mmModules::mmBlob> BlobDetectionFiltered(Mat frame, Mat foreground);
				vector<mmLib::mmModules::mmBlob> BlobDetectionApp(Mat frame, Mat foreground, vector<mmLib::mmModules::mmBlob> activeTracks);
				vector<mmLib::mmModules::mmBlob> BlobDetectionStd(Mat frame, Mat foreground, vector<mmLib::mmModules::mmBlob> activeTracks);

				vector<Point> getBlobParticle(Mat frame, vector<mmLib::mmModules::mmBlob> blobList, int gridGranularity);

				void DbTracker(vector<mmLib::mmModules::mmBlob>& blobList, vector<mmLib::mmModules::mmBlob>& oldList, float threshold);
				void kalmanTracker(Mat frame, vector<mmLib::mmModules::mmBlob> blobList, double threshold = 0);
				void Release();
				// Struttura di parametri del Tracker - i parametri bgsub si possono cambiare anche live.
				trackerSettings tParam;
				mmKalmanDatabase* kDb;

			private:
				// Modulo di background subtraction
				mmLib::mmModules::mmGaussianFGDetector* fDec;
				// Modulo di codebook bg subtraction
				mmLib::mmModules::mmCodeBookBgSubstraction* fDecC;
				// Modulo di blob detection
				CvBlobDetector* bDec;
				// Modulo di tracking
				mmDatabase* db; // or

				vector<Rect> nonVisibleArea;
				int framecounter;
				int dThresh;
				int centThresh;
				int nnVisibleThresh;
				int minBlobWidth;
				int minBlobHeigth;
				int incTL;
				int incBR;
				int totalFrameProcessed;
				int startAnomaly;
				int windowSize;
				bool needToTrain;

				//ObjectType objectType;
				mmLib::mmModules::appModel* aMod;
				vector<mmLib::mmModules::mmBlob> detectNewBlob(Mat frame, Mat foreground);
				bool isObjectOutOfRange(cv::Mat frame, mmLib::mmModules::mmBlob blob);
		};

		bool isPointContRect(Rect r, Point p);
		vector<mmLib::mmModules::mmBlob> deleteBlobFromVect(vector<mmLib::mmModules::mmBlob> stuff,int i);
		mmLib::mmModules::mmBlob setParameters(cv::Mat frame, mmLib::mmModules::mmBlob blobNew, mmLib::mmModules::mmBlob blobOld, bool isNew, bool isMwerge);
		vector<Point> deletePoint(vector<Point> vectInt, int pos);

		mmLib::mmModules::mmGaussianFGDetector* chooseBgModule(trackerSettings t);
		CvBlobDetector* chooseBDecModule(trackerSettings t);
		void chooseTracker(trackerSettings t);
		// Calcola la distanza tra due Point
		double PointDistance(Point a, Point b);
	}
}

#endif /* MMTRACKER_H_ */
