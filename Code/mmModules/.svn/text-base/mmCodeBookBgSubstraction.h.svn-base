#ifndef MMCODEBOOKBGSUBSTRACTION_H
#define MMCODEBOOKBGSUBSTRACTION_H
#include <vector>
#include <cv.h>
#include <fstream>
namespace mmLib {
namespace mmModules{
struct CodeBookElement
{
    double  Rx;
    double  Gx;
    double  Bx;
    double  minI;
    double  maxI;
    int     frequency;
    int     maximumNegativeRunLength;
    int     firstAccessTime;
    int     lastAccessTime;
    //0 is permanent - 1 is cache
    int     permanentOrCache;

    CodeBookElement() : Rx(-1),Gx(-1),Bx(-1),minI(-1),
        frequency(-1),maximumNegativeRunLength(-1),
        firstAccessTime(-1),lastAccessTime(-1),permanentOrCache(-1){}


};

class codeBookParameters{
public:
	codeBookParameters();
	int cbMemory;
	double alphaValue;
	double betaValue;
	double epsilon1Value;
	int tCacheValue;
	int tAddValue;
	int tDeleteValue;
	int tAssorbValue;
};

class mmCodeBookBgSubstraction
{
public:
    mmCodeBookBgSubstraction();
    ~mmCodeBookBgSubstraction();
    mmCodeBookBgSubstraction(int codeBookMemory,
    		double alpha_value,
    		double beta_value,
    		double epsilon1_value,
    		int tCache_value,
    		int tAdd_value,
    		int tDelete_value,
    		int tAssorb_value);
    mmCodeBookBgSubstraction(codeBookParameters &cb);
    int CodeBookMemory;
    //std::vector<CodeBookElement> *codeBook;
    //CodeBookElement **codeBook;
    std::vector<CodeBookElement> *codeBook;
    cv::Mat processFrame(cv::Mat frame);
     bool isInTraining;
private:
    int totalFrameProcessed;

    int UpdateCodebook(int Bp, int Gp, int Rp, double Ip, int index);
    static bool cleanCodebook(CodeBookElement element);
    static bool cleanCacheBGCodebook(CodeBookElement element);
    double alpha,beta;
    double epsilon1;
    bool firsTime;
    bool cbFind;
    int maxCodeBookLenght;
    std::ofstream file;
    int maxIndex;
    int times;

};
}}
#endif // MMCODEBOOKBGSUBSTRACTION_H
