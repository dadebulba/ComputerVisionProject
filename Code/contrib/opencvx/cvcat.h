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
#ifndef CVCAT_H_
#define CVCAT_H_


#include "cv.h"
#include "cvaux.h"
#include "cvsetrow.h"
#include "cvsetcol.h"

/**
 * Concatinate arrays horizontally
 *
 * @param src1 Input array 1
 * @param src2 Input array 2
 * @param dst Target array
 * @see cvCat, cvVcat
 */
#define cvHcat( src1, src2, dst ) cvCat( (src1), (src2), (dst), 0 )

/**
 * Concatinate arrays vertically
 *
 * @param src1 Input array 1
 * @param src2 Input array 2
 * @param dst Target array
 * @see cvCat, cvHcat
 */
#define cvVcat( src1, src2, dst ) cvCat( (src1), (src2), (dst), 1 )

/**
* Concatinate arrays
*
* Example)
* @code
*    IplImage* img    = cvCreateImage( cvSize(4,4), IPL_DEPTH_8U, 1 ); // width(cols), height(rows)
*    IplImage* subimg = cvCreateImage( cvSize(1,4), IPL_DEPTH_8U, 1 );
*    IplImage* catimg = cvCreateImage( cvSize(5,4), IPL_DEPTH_8U, 1 );
*    cvSet( img, cvScalar(1) );
*    cvSet( subimg, cvScalar(0) );
*    cvHcat( img, subimg, catimg ); // 4x4 + 4x1 = 4x5
*    cvMatPrint( catimg );
* @endcode
*
* @param src1 Input array 1
* @param src2 Input array 2
* @param dst Target array
* @param dim 0 horizontally, 1 vertically, -1 flexibly
* @see cvHcat( src1, src2, dst ) // cvCat( src1, src2, dst, 0 )
* @see cvVcat( src1, src2, dst ) // cvCat( src1, src2, dst, 1 )
*/
CVAPI(void) cvCat( const CvArr*, const CvArr*, CvArr* , int );

#endif

