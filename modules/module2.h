/*
 * module3.h
 *
 *  Created on: 7 ott 2021
 *      Author: davide
 */

#ifndef MODULES_MODULE2_H_
#define MODULES_MODULE2_H_

#include <string>
#include <vector>
#include "opencv/cxcore.h"

using namespace std;
using namespace cv;

struct module2Config {
	string videoPath;
    int threshold;
    int numAnomalyParam;
    int numNormalParam;
};

int module2(module2Config &config, bool verbose);


#endif /* MODULES_MODULE3_H_ */
