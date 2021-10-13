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
#ifndef CVCREATEAFFINE_H_INCLUDED
#define CVCREATEAFFINE_H_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvrect32f.h"

/**
 * Create an affine transform matrix from parameters.
 *
 * @param affine    The 2 x 3 CV_32FC1|CV_64FC1 affine matrix to be created
 * @param rect      The translation (x, y) and scaling (width, height) and
 *                  rotation (angle) paramenter in degree
 * @param shear     The shear deformation parameter shx and shy
 * @return CVAPI(void)
 *
 * References)
 * @verbatim
 @Book{Hartley2004,
    author = "Hartley, R.~I. and Zisserman, A.",
    title = "Multiple View Geometry in Computer Vision",
    edition = "Second",
    year = "2004",
    publisher = "Cambridge University Press, ISBN: 0521540518"
 } @endverbatim
 */
CVAPI(void) cvCreateAffine( CvMat* ,
                            CvRect32f,
                            CvPoint2D32f);



#endif
