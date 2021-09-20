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
#ifndef CV_RECT32FPOINTS_INCLUDED
#define CV_RECT32FPOINTS_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include "cvrect32f.h"
#include "cvcreateaffine.h"

// CVAPI(void) cvRect32fPoints( CvRect32f rect, CvPoint2D32f pt[4], 
//                       CvPoint2D32f shear = cvPoint2D32f(0,0) );
// CV_INLINE void cvBox32fPoints( CvBox32f box, CvPoint2D32f pt[4], 
//                                CvPoint2D32f shear = cvPoint2D32f(0,0) );
// CV_INLINE void cvRectPoints( CvRect rect, CvPoint2D32f pt[4] );
// cvBoxPoints

/**
 * Find 4 corners of rectangle
 *
 * @code
 *     x y
 * [0] 0 0
 * [1] 1 0
 * [2] 1 1
 * [3] 0 1
 * @endcode
 *
 * @param rect       Rectangle parameters and rotation angle parameter 
 * @param pt[4]      Found 4 corners
 * @param shear      Shear deformation parameter if you want
 * @see cvBoxPoints, cvBox32fPoints, cvRectPoints, cvRect32fPoints
 */
CVAPI(void) cvRect32fPoints( CvRect32f rect, CvPoint2D32f pt[4],
		                       CvPoint2D32f shear);

/**
 * Find 4 corners of box
 *
 * @code
 *     x y
 * [0] 0 0
 * [1] 1 0
 * [2] 1 1
 * [3] 0 1
 * @endcode
 *
 * @param box    Box structure
 * @param pt[4]  Found 4 corners
 * @param shear  Shear deformation parameters if you want
 * @see cvBoxPoints, cvBox32fPoints, cvRectPoints, cvRect32fPoints
 */
CV_INLINE void cvBox32fPoints( CvBox32f box, CvPoint2D32f pt[4], 
                               CvPoint2D32f shear CV_DEFAULT(cvPoint2D32f(0,0)) )
{
    cvRect32fPoints( cvRect32fFromBox32f( box ), pt, shear );
}

/**
 * Find 4 corners of rectangle
 *
 * @code
 *     x y
 * [0] 0 0
 * [1] 1 0
 * [2] 1 1
 * [3] 0 1
 * @endcode
 *
 * @param rect   Rectangle parameter
 * @param pt[4]  Found 4 points
 * @see cvBoxPoints, cvBox32fPoints, cvRectPoints, cvRect32fPoints
 */
CV_INLINE void cvRectPoints( CvRect rect, CvPoint2D32f pt[4] )
{
    cvRect32fPoints( cvRect32fFromRect( rect ), pt, cvPoint2D32f(0,0) );
}

#endif
