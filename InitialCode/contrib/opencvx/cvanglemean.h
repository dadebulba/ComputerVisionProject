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
#ifndef CVANGLEMEAN_H_
#define CVANGLEMEAN_H_

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

#include <float.h>
#define _USE_MATH_DEFINES
#include <math.h>

/**
 * Compute mean of angle elements of an array (each channel independently). 
 *
 * There is a fact that 0 degrees and 360 degrees are identical angles, 
 * so that for example 180 degrees is not a sensible mean of 2 degrees and 
 * 358 degrees, but 0 degree is the mean. 
 * Algorithm works as 1. compute means of cosine and sine 
 * 2. take arc tangent of the mean consine and sine. 
 *
 * @param  arr     array
 * @param  weight  Weight to compute mean. The deafult is 1/num (uniform).
 *                 The size must be same with arr. 
 * @param  wrap    The unit of wrapping around.
 *                 The defeault is 360 as angle.
 * @return angle mean for each channel
 */
CVAPI(CvScalar) cvAngleMean( const CvArr*,
                             const CvArr*,
                             double);

#endif
