/*
 * kalmanFilter.cpp
 *
 *  Created on: 01/dic/2010
 *      Author: daldoss
 */

#include "../mmLib.h"
namespace mmLib{
namespace mmModules{

KalmanFilter::KalmanFilter()
{
	initialize();
}

KalmanFilter::KalmanFilter(const KalmanFilter &kalman)
{
	initialize();
}
KalmanFilter& KalmanFilter::operator=(const KalmanFilter &kalman)
{
	cvReleaseMat( &state );
	cvReleaseMat( &process_noise );
	cvReleaseMat( &measurement );
	cvReleaseMat( &measurement_noise );
	cvReleaseKalman( &m_pKalmanFilter );
	initialize();
	return *this;
}

KalmanFilter::~KalmanFilter()
{
	// Destroy state, process_noise, and measurement matricies
	cvReleaseMat( &state );
	cvReleaseMat( &process_noise );
	cvReleaseMat( &measurement );
	cvReleaseMat( &measurement_noise );
	cvReleaseKalman( &m_pKalmanFilter );

	// DeleteCriticalSection(&mutexPrediction);
}

bool KalmanFilter::isInitialized() const
{
	return init;
}

void KalmanFilter::initialize()
{
	init = false;
	m_timeLastMeasurement = clock();
	int dynam_params = 4;	// x,y,dx,dy//,width,height
	int measure_params = 2;
	m_pKalmanFilter = cvCreateKalman(dynam_params, measure_params);
	state = cvCreateMat( dynam_params, 1, CV_32FC1 );

	// random number generator stuff
	cvRandInit( &rng, 0, 1, -1, CV_RAND_UNI );
	cvRandSetRange( &rng, 0, 1, 0 );
	rng.disttype = CV_RAND_NORMAL;
	cvRand( &rng, state );

	process_noise = cvCreateMat( dynam_params, 1, CV_32FC1 ); // (w_k)
	measurement = cvCreateMat( measure_params, 1, CV_32FC1 ); // two parameters for x,y  (z_k)
	measurement_noise = cvCreateMat( measure_params, 1, CV_32FC1 ); // two parameters for x,y (v_k)
	cvZero(measurement);

	// F matrix data
	// F is transition matrix.  It relates how the states interact
	// For single input fixed velocity the new value
	// depends on the previous value and velocity- hence 1 0 1 0
	// on top line. New velocity is independent of previous
	// value, and only depends on previous velocity- hence 0 1 0 1 on second row
	double fps = 5;
	const double DEL_T = 1/fps;
	const float F[] = {
			1, 0, 1, 0,// 0, 0,	//x + dx
			0, 1, 0, 1,// 0, 0,	//y + dy
			0, 0, 1, 0,// 0, 0,	//dx = dx
			0, 0, 0, 1,// 0, 0,	//dy = dy
			//0, 0, 0, 0, 1, 0,	//width
			//0, 0, 0, 0, 0, 1,	//height
	};
	memcpy( m_pKalmanFilter->transition_matrix->data.fl, F, sizeof(F));
	cvSetIdentity( m_pKalmanFilter->measurement_matrix, cvRealScalar(1) ); // (H)

	// Questo Ã¨ il parametro da modificare: valore piÃ¹ alto, kalman + lento.
	cvSetIdentity( m_pKalmanFilter->measurement_noise_cov, cvRealScalar(1e-1) ); // Funzionante ma lento: 1 (R) ORIGINAL--> 1e-1

	cvSetIdentity( m_pKalmanFilter->process_noise_cov, cvRealScalar(1e-5) ); // Funzionante ma lento: 1e-5 (Q) ORIGINAL--> 1e-5
	cvSetIdentity( m_pKalmanFilter->error_cov_post, cvRealScalar(1));

	// choose random initial state
	cvRand( &rng, m_pKalmanFilter->state_post );

	//InitializeCriticalSection(&mutexPrediction);
}

void KalmanFilter::predictionBegin(float x , float y)
{
	init = true;
	// Initialize kalman variables (change in x, change in y,...)
	// m_pKalmanFilter->state_post is prior state

	m_pKalmanFilter->state_post->data.fl[0] = x;	//center x
	m_pKalmanFilter->state_post->data.fl[1] = y;	//center y
	m_pKalmanFilter->state_post->data.fl[2] = (float)0;	 //dx
	m_pKalmanFilter->state_post->data.fl[3] = (float)0;	 //dy
	//m_pKalmanFilter->state_post->data.fl[4] = (float)dWidth;	 //width
	//m_pKalmanFilter->state_post->data.fl[5] = (float)dHeight;	 //height

}

void KalmanFilter::predictionUpdate(float x, float y)
{
	//EnterCriticalSection(&mutexPrediction);

	m_timeLastMeasurement = clock();

	state->data.fl[0] = x;	//center x
	state->data.fl[1] = y;	//center y
	state->data.fl[2] = (float) 0;
	state->data.fl[3] = (float) 0;
	m_bMeasurement = true;
	//LeaveCriticalSection(&mutexPrediction);
}

void KalmanFilter::predictionReport(CvPoint &pnt)
{
	clock_t timeCurrent = clock();

	//EnterCriticalSection(&mutexPrediction);

	const CvMat* prediction = cvKalmanPredict( m_pKalmanFilter, 0 );
	pnt.x = prediction->data.fl[0];
	pnt.y = prediction->data.fl[1];
	//m_dWidth = prediction->data.fl[4];
	//m_dHeight = prediction->data.fl[5];

	// If we have received the real position recently, then use that position to correct
	// the kalman filter.  If we haven't received that position, then correct the kalman
	// filter with the predicted position
	if (m_bMeasurement) //(timeCurrent - m_timeLastMeasurement < TIME_OUT_LOCATION_UPDATE)	// update with the real position
	{
		m_bMeasurement = false;

	}
	// My kalman filter is running much faster than my measurements are coming in.  So this allows the filter to pick up from the last predicted state and continue in between measurements (helps the tracker look smoother)
	else	// update with the predicted position
	{
		state->data.fl[0] = pnt.x;
		state->data.fl[1] = pnt.y;
	}

	// generate measurement noise(z_k)
	cvRandSetRange( &rng, 0, sqrt(m_pKalmanFilter->measurement_noise_cov->data.fl[0]), 0 );
	cvRand( &rng, measurement_noise );

	// zk = Hk * xk + vk
	// measurement = measurement_error_matrix * current_state + measurement_noise
	cvMatMulAdd( m_pKalmanFilter->measurement_matrix, state, measurement_noise, measurement );

	// adjust Kalman filter state
	cvKalmanCorrect( m_pKalmanFilter, measurement );

	// generate process noise(w_k)
	cvRandSetRange( &rng, 0, sqrt(m_pKalmanFilter->process_noise_cov->data.fl[0]), 0 );
	cvRand( &rng, process_noise );

	// xk = F * xk-1 + B * uk + wk
	// state = transition_matrix * previous_state + control_matrix * control_input + noise
	cvMatMulAdd( m_pKalmanFilter->transition_matrix, state, process_noise, state );

	//LeaveCriticalSection(&mutexPrediction);
}
}
}

