/*
 * mmDatabase.h
 *
 *  Created on: Dec 3, 2010
 *      Author: paolo
 */

#ifndef MMDATABASE_H_
#define MMDATABASE_H_
using namespace mmLib::mmModules;

namespace mmLib {
namespace mmModules{
struct matchHelper{
	int blobListID;
	int dbID;
	float mse;
};

class mmDatabase {
public:
	mmDatabase();
	virtual ~mmDatabase();
	void checkItTwice(vector<mmBlob> &blobList, float threshold);
	void printBlobList();
	int size();

private:
	void addId(mmLib::mmModules::mmBlob &blob);
	matchHelper matchIt(vector<mmLib::mmModules::mmBlob> &blobList, int i);

	std::vector<mmModules::mmBlob> lastBlobList;
	std::vector<int> idList;
	int idMax;
};

struct KalmanIstance{
	cv::Point pt;
	int id;
	mmModules::KalmanFilter *kFilter;
	cv::Mat img;
	cv::Mat fgnd;
	//vector<int> bindedBlob;
	int isOccluded;
//	vector<int> listOfOccludingBlob;
};


class mmKalmanDatabase {
public:
	mmKalmanDatabase();
	~mmKalmanDatabase();
	int findCloser (KalmanIstance ist, std::vector<mmModules::mmBlob> blobList, double &distMin);

	std::vector<KalmanIstance> kList;
	int maxId;
	//int framecounter;

};
}
}

#endif /* MMDATABASE_H_ */


