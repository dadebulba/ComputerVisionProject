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
#ifndef CV_GET2D_INTER_INCLUDED
#define CV_GET2D_LINEAR_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"

/**
 * Nearest Neighbor Interpolation
 *
 * Get a color at a floating position using nearest neighbor (round) method.
 *
 * @param  arr     array
 * @param  row
 * @param  col
 * @return color
 * @see cvInterLinear
 */
CVAPI(CvScalar) 
icvGet2DInterNn( const CvArr *arr, float row, float col );

/**
 * Bilinear Interpolation
 *
 * Get a color at a floating position using bilinear interpolation method
 * Floating origin is the center of (0,0) pixel.
 *
 * @param  arr     array
 * @param  row
 * @param  col
 * @return color
 */
CVAPI(CvScalar)
icvGet2DInterLinear( const CvArr *arr, float row, float col );

/**
 * Get an element of array with interpolation method
 *
 * Get a color at a floating position using an interpolation method.
 * Floating origin is the center of (0,0) pixel.
 *
 * @param  arr     array
 * @param  row     floating row position
 * @param  col     floating col position
 * @param  inter   The interpolation method. 
 *   - CV_INTER_NN - Nearest Neighborhood method
 *   - CV_INTER_LINEAR - Bilinear interpolation
 * @return color
 * @see cvResize has "interpolation" method, 
 *      but it was not reusable. 
 */
CVAPI(CvScalar)
cvGet2DInter( const CvArr *arr, float row, float col,
              int inter);

#endif
