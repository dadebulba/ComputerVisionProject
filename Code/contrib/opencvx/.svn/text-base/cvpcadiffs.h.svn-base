/** @file */
/* Copyright (c) 2008, Naotoshi Seo. All rights reserved.
 *
 * The program is free to use for non-commercial academic purposes,
 * but for course works, you must understand what is going inside to 
 * use. The program can be used, modified, or re-distributed for any 
 * purposes only if you or one of your group understand not only 
 * programming codes but also theory and math behind (if any). 
 * Please contact the authors if you are interested in using the 
 * program without meeting the above conditions.
*/

#include "cv.h"
#include "cvaux.h"
#include <stdio.h>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef CV_PCADIFFS_INCLUDED
#define CV_PCADIFFS_INCLUDED

//#ifndef NO_DOXYGEN
CVAPI(void) 
cvMatPcaDiffs( const CvMat* samples, const CvMat* avg, const CvMat* eigenvalues, 
               const CvMat* eigenvectors, CvMat* probs, 
               int normalize, bool logprob );
//CV_INLINE double
//cvPcaDiffs( const CvMat* sample, const CvMat* avg, const CvMat* eigenvalues,
//            const CvMat* eigenvectors, int normalize, bool logprob );
//#endif

/**
 * PCA Distance "in" and "from" feature space
 *
 * @param sample              D x 1 feature vector
 * @param avg                 D x 1 mean vector
 * @param eigenvalues         nEig x 1 eigen values
 * @param eigenvectors        M x D eigen vectors
 * @param normalize           Compute normalization term or not
 *                            0 - nothing
 *                            1 - normalization term
 *                            2 - normalize so that sum becomes 1.0
 * @param logprob             Log probability or not
 * @return double likelihood
*/
CV_INLINE double
cvPcaDiffs( const CvMat* sample, const CvMat* avg, const CvMat* eigenvalues,
            const CvMat* eigenvectors,
            int normalize CV_DEFAULT(1), bool logprob CV_DEFAULT(false) )
{
    double prob;
    CvMat *_probs  = cvCreateMat( 1, 1, sample->type );

    cvMatPcaDiffs( sample, avg, eigenvalues, eigenvectors, _probs, normalize, logprob );
    prob = cvmGet(_probs, 0, 0);

    cvReleaseMat( &_probs );
    return prob;
}

#endif
