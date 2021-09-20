#ifndef MMCROWDANALYSIS_H
#define MMCROWDANALYSIS_H
#include <vector>
#include <cv.h>
#include <highgui.h>
#include <fstream>
#include <algorithm>
//#include "gc/energy.h"
//#include "gc/GCoptimization.h"
//#include "gc/graph.h"
namespace mmLib {
namespace mmModules{
struct CrowdArea
{
    int label;
    cv::vector<cv::Point> AreaPoint;

//    CodeBookElement() : Rx(-1),Gx(-1),Bx(-1),minI(-1),
//        frequency(-1),maximumNegativeRunLength(-1),
//        firstAccessTime(-1),lastAccessTime(-1),permanentOrCache(-1){}


};
struct CrowdResult
{
    std::vector<cv::Mat> CrowdMat;
    std::vector<CrowdArea> CrowdAreas;
    std::vector<CrowdArea> CurrentCrows;

//    CodeBookElement() : Rx(-1),Gx(-1),Bx(-1),minI(-1),
//        frequency(-1),maximumNegativeRunLength(-1),
//        firstAccessTime(-1),lastAccessTime(-1),permanentOrCache(-1){}


};
class mmCrowdAnalysis
{
public:
    mmCrowdAnalysis();
    mmCrowdAnalysis(double alarm_threshold);
    int blockSize;
    int windowSize;
    int accWindowsComparasionSize;
    int numLabel;
    double area_alarm;
    CrowdResult processFrame(cv::Mat frame, cv::Mat bgFrame);
    std::vector< std::vector<int> > getPseudoColorPalette();
    cv::Mat getCrowdDirectionImage(int width_img, int height_img);
    std::vector< std::vector<int> > pseudoColorMatrix;
    int memory;
private:
    int width;
    int height;
    cv::Mat nowGrayFrame;
    cv::Mat prevGrayFrame;
    cv::Mat prevBGFrame;
    cv::Mat accMatrix;
    cv::Mat fastAccMatrix;
    cv::Mat gcMatrix;
    std::vector<double> computeCorrelationMatrix(int centerI, int centerJ);
    void buildLabelDistanceMatrix();
    void buildPseudoColorMatrix();
    std::vector< std::vector<double> > accuMulatorMatrix;
    std::vector< std::vector<double> > fastAccuMulatorMatrix;
    std::vector< std::vector<double> > prevAccuMulatorMatrix;
    std::vector< std::vector<double> > labelDistanceMatrix;
    //GCoptimizationGridGraph *gc;
    bool firstFrame;
    int frameNumber;
    int now_memory;
    bool memory_filled;
};
}}
#endif // MMCROWDANALYSIS_H


