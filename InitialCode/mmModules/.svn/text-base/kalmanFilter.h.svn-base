/*
 * kalmanFilter.h
 *
 *  Created on: 01/dic/2010
 *      Author: daldoss
 */

#ifndef KALMANFILTER_H_
#define KALMANFILTER_H_

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
namespace mmLib{
namespace mmModules{

class KalmanFilter
{
public:

	KalmanFilter(void);
	~KalmanFilter(void);
	KalmanFilter(const KalmanFilter &kalman);
	bool isInitialized() const;
	virtual KalmanFilter& operator=(const KalmanFilter &kalman);
	virtual void predictionBegin(float x, float y);
	virtual void predictionUpdate(float x, float y);
	virtual void predictionReport(CvPoint &pnt);

private:
	bool init;
	CvKalman *m_pKalmanFilter;
	CvRandState rng;
	CvMat* state;
	CvMat* process_noise;
	CvMat* measurement;
	CvMat* measurement_noise;
	bool m_bMeasurement;
	clock_t m_timeLastMeasurement;
	//CRITICAL_SECTION mutexPrediction;

	void initialize();
};

}
}
#endif /* KALMANFILTER_H_ */

