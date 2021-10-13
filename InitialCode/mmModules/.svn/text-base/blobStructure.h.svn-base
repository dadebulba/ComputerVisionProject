#ifndef MMBLOB_STRUCTURE_H_
#define MMBLOB_STRUCTURE_H_

using namespace cv;
using namespace std;

namespace mmLib{
	namespace mmModules{

		struct blobListModule{
			Point center;
			int index;
		};

		struct polygons{
			vector<vector<Point> > contourPoints;
		};

		class harrisDetectorSettings{
		public:
			harrisDetectorSettings();
			int blockSizeParam;
			int apertureSizeParam;
			double kParam;
			int threshParam;
			int maxFrameParam;
			Mat maskParam;
		};

		class harrisDetector{
		public:
			harrisDetector();
			harrisDetector(bool fF, bool sB, int neighborhoodParam, int apertureParam, int c, double freeParam, int th, int mF);
			harrisDetector(harrisDetectorSettings & hS);
			~harrisDetector();

			vector<Point> getInitailConf();
			vector<Point> getCurrentConf();

			void InitHarris();
			void UpdateHarris(Mat frame);

			bool isHarrisChanged();
		private:
			vector<Point> initialConf;
			vector<Point> currentConf;

			void setCurrentConf(Point currConf);
			void setInitailConf(Point initConf);

			bool isSamePoints();

			bool firstFrame;
			bool sameBef;
			int blockSize;
			int apertureSize;
			int count;
			double k;
			int totalFrameProcessed;
			int thresh;
			int maxFrame;
			Mat mask;
		};

		class blobStructure{
		public:
			blobStructure();
			blobStructure(bool fF, bool fT, int tFp, int nP);
			~blobStructure();


			void InitBlobStructure(int numPolygons);
			void setPolygons(vector<vector<Point> > contours,int numPoly);

			void UpdateLists(vector<mmLib::mmModules::mmBlob> blobList);

			vector<mmLib::mmModules::mmBlob> getBlobIn(vector<mmLib::mmModules::mmBlob> blobList);
			vector<mmLib::mmModules::mmBlob> getBlobOut(vector<mmLib::mmModules::mmBlob> blobList);

			vector<mmLib::mmModules::blobListModule> *getBIn();
			vector<mmLib::mmModules::blobListModule> *getBOut();
		private:
			vector<mmLib::mmModules::blobListModule> *blobIn;
			vector<mmLib::mmModules::blobListModule> *blobOut;

			vector<mmLib::mmModules::polygons> *poly;

			bool firstFrame;
			bool firstTime;
			int totalFrameProcessed;
			int numPolygons;

			void setBlobIn(mmLib::mmModules::blobListModule listIn);
			void setBlobOut(mmLib::mmModules::blobListModule listOut);
		};
		Mat DrawList(Mat image, vector<mmLib::mmModules::blobListModule> *bIn, vector<mmLib::mmModules::blobListModule> *bOut);
	}
}

#endif /* MMBLOB_STRUCTURE_H_ */


