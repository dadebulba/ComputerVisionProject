/*
 * module3.h
 *
 *  Created on: 7 ott 2021
 *      Author: davide
 */

#ifndef MODULES_MODULE3_H_
#define MODULES_MODULE3_H_

#include <string>
#include <vector>
#include "opencv/cxcore.h"

using namespace std;
using namespace cv;

struct module3Config {
	string videoPath;
	string outputVideoPath;
	int widthScale;
	int heightScale;
	int accumulationPhase_frameStart;
	int accumulationPhase_frameEnd;
	int thresholdHigh;
	int thresholdLow;
	vector<Rect> rectangles;
};

int module3(module3Config &config, bool verbose);


#endif /* MODULES_MODULE3_H_ */
