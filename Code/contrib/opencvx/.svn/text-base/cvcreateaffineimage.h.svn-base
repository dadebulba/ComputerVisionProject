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
#ifndef CV_CREATEAFFINEIMAGE_INCLUDED
#define CV_CREATEAFFINEIMAGE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits.h>

#include "cvinvaffine.h"

#define CV_AFFINE_SAME 0
#define CV_AFFINE_FULL 1

//CV_INLINE IplImage* 
//cvCreateAffineMask( const IplImage* src, const CvMat* affine, 
//                    int flags = CV_AFFINE_SAME, CvPoint* origin = NULL );
//IplImage* 
//cvCreateAffineImage( const IplImage* src, const CvMat* affine, 
//                     int flags = CV_AFFINE_SAME, CvPoint* origin = NULL,
//                     CvScalar bgcolor = CV_RGB(0,0,0) );

/**
 * Affine transform of an image. 
 *
 * Memory is allocated. Do not forget cvReleaseImage( &ret );
 *
 * @param src       Image
 * @param affine    2 x 3 Affine transform matrix
 * @param flags     CV_AFFINE_SAME - Outside image coordinates are cut off. 
 *                  CV_AFFINE_FULL - Fully contain the original image pixel values
 * @param origin    The coordinate of origin is returned. The origin is the 
 *                  coordinate in original image respective to the transformed 
 *                  image origin. Useful when CV_AFFINE_FULL is used.
 * @param bgcolor   The color used when corresponding coordinate is not 
 *                  available in the original image. 
 * @return
 * @see cvWarpAffine
 *  this does not support CV_AFFINE_FULL, but supports
 *  everal interpolation methods and so on.
 */
IplImage* 
cvCreateAffineImage( const IplImage*, const CvMat* ,
                     int , CvPoint*,
                     CvScalar );

/**
 * Create a mask image for cvCreateAffineImage
 *
 * @param src       Image. Used to get image size.
 * @param affine    2 x 3 Affine transform matrix
 * @param flags     CV_AFFINE_SAME - Outside image coordinates are cut off
 *                  CV_AFFINE_FULL - Fully contain the original image pixel values
 * @param origin    The coordinate of origin (the coordinate in original image respective to 
 *                  the transformed image origin). 
 *                  Useful when CV_AFFINE_FULL is used.
 */
CV_INLINE IplImage* 
cvCreateAffineMask( const IplImage*, const CvMat* ,
                    int, CvPoint*);

#endif
