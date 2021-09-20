#ifndef MMAPP_MODEL_H_
#define MMAPP_MODEL_H_

using namespace cv;

static const double pi = 3.14159265358979323846;

namespace mmLib{
	namespace mmModules{

		class appSettings{
		public:
			appSettings();
			double appAlpha;
			double appLambda;
			double appSigma;
			double appThresh;
		};

		class appModel{
		public:
			appModel();
			appModel(appSettings &a);
			~appModel();

			void appFrame();

			mmLib::mmModules::mmBlob newAppModel(Mat frame, Mat foreground, mmLib::mmModules::mmBlob blob);
			mmLib::mmModules::mmBlob stdAppModel(Mat frame, Mat foreground, mmLib::mmModules::mmBlob blobNew, mmLib::mmModules::mmBlob blobOld);
			mmLib::mmModules::mmBlob mergeAppModel(Mat frame, Mat foreground, mmLib::mmModules::mmBlob blobNew, mmLib::mmModules::mmBlob blobOld);
			mmLib::mmModules::mmBlob nonMergeAppModel(Mat frame, Mat foreground, mmLib::mmModules::mmBlob blobNew, vector<mmLib::mmModules::mmBlob> blobOld);

			Mat getAppMatrix(Mat frame, mmLib::mmModules::mmBlob blob);
			Mat getProbMatrix(Mat frame, mmLib::mmModules::mmBlob blob);

			mmLib::mmModules::mmBlob propagateObject(Mat frame, Mat foreground, mmLib::mmModules::mmBlob track);

			vector<mmLib::mmModules::mmBlob> SM(Mat frame, Mat foreground, mmBlob blobU, vector<mmLib::mmModules::mmBlob> tracks);
		private:
			double alpha;
			double lambda;
			double sigma;
			double thresh;
			int totalFrameProcessed;

			double getPrgb(Mat frame, mmLib::mmModules::mmBlob track, Point p, Point pOld);
			Rect propagatedRect(Mat frame, mmLib::mmModules::mmBlob blob);
//			double getDistFromCluster(Mat frame, Point p, vector<Point> clusterP);
			vector<mmLib::mmModules::mmBlob> assignAmbPoints(vector<mmLib::mmModules::mmBlob> vectBlob,vector<Point> vectPoint);
		};
		bool isFg(Mat foreground,Point p);
	}
}
///////////////////////
#endif /* MMAPP_MODEL_H_ */


