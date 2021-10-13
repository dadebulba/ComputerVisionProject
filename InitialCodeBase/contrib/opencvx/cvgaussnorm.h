/** @file */
/* The MIT License
 * 
 * Copyright (c) 2008, Naotoshi Seo <sonots(at)sonots.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef CV_GAUSSNORM_INCLUDED
#define CV_GAUSSNORM_INCLUDED


#include "cv.h"
#include "cvaux.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvmatelemcn.h"

// @
// CVAPI(void) cvMatGaussNorm( const CvMat* samples, CvMat* dst );
// #define cvGaussNorm( sample, dst ) cvMatGaussNorm( sample, dst )
// CVAPI(void) cvImgGaussNorm( const IplImage* img, IplImage* normed ) {
//    IplImage* sample, samplehdr;
//    IplImage* dst, dsthdr;
//    sample = cvReshape( img, &samplehdr, 1, img->nChannels );
//    dst = cvReshape( normed, &dsthdr, 1, dst->nChannels );
//    // make sure how it is reshaped
//    cvMatGaussNorm( sample, dst );
// }

/**
 * Zero mean and unit covariance normalization of an image
 * Each channel is processed independently
 *
 * @param src       input image
 * @param dst       normalized image. 32F or 64F should be preferred.
*/
CVAPI(void) cvGaussNormImage( const CvArr* src, CvArr* dst );


#endif
