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
#ifndef CV_DRAWRECTANGLE_INCLUDED
#define CV_DRAWRECTANGLE_INCLUDED

#include "cv.h"
#include "cvaux.h"
#include "cxcore.h"
#include "highgui.h"
#define _USE_MATH_DEFINES
#include <math.h>

#include "cvcreateaffine.h"
#include "cvrect32f.h"

//CVAPI(void) 
//cvDrawRectangle( IplImage* img, 
//                 CvRect32f rect32f = cvRect32f(0,0,1,1,0),
//                 CvPoint2D32f shear = cvPoint2D32f(0,0), 
//                 CvScalar color = CV_RGB(255, 255, 255), 
//                 int thickness = 1, 
//                 int line_type = 8,
//                int shift = 0 );
//CV_INLINE void 
//cvShowImageAndRectangle( const char* w_name, const IplImage* img, 
//                         CvRect32f rect32f = cvRect32f(0,0,1,1,0),
//                         CvPoint2D32f shear = cvPoint2D32f(0,0),
//                         CvScalar color = CV_RGB(255, 255, 0), 
//                         int thickness = 1, int line_type = 8, 
//                         int shift = 0 );

/**
 * Draw an rotated and sheared rectangle
 *
 * If you want define rotation center as the center of the rectangle,
 * use CvBox32f and use cvRect32fBox32( box32f ) for the rect32f argument. 
 *
 * @param img             The image to be drawn rectangle
 * @param rect32f         The rectangle (x,y,width,height) to be shown and
 *                        the rotation angle in degree where the rotation center is (x,y)
 * @param shear           The shear deformation parameter shx and shy
 * @param color           Line color (RGB) or brightness (grayscale image). 
 * @param thickness       Thickness of lines that make up the rectangle. 
 *                        Negative values, e.g. CV_FILLED, make the function 
 *                        to draw a filled rectangle. 
 * @param line_type       Type of the line, see cvLine description. 
 * @param shift           Number of fractional bits in the point coordinates. 
 *
 * @return CVAPI(void)
 * @uses cvRectangle
 */
CVAPI(void) 
cvDrawRectangle( IplImage* img, 
                 CvRect32f rect32f,
                 CvPoint2D32f shear,
                 CvScalar color,
                 int thickness ,
                 int line_type ,
                 int shift );

/**
 * Show Image and Rectangle
 *
 * @param w_name          Window name
 * @param img             Image to be shown
 * @param rect32f         The rectangle (x,y,width,height) to be shown and
 *                        the rotation angle in degree where the rotation center is (x,y)
 * @param shear           The shear deformation parameter shx and shy
 * @param color           Line color (RGB) or brightness (grayscale image). 
 * @param thickness       Thickness of lines that make up the rectangle. 
 *                        Negative values, e.g. CV_FILLED, make the function 
 *                        to draw a filled rectangle. 
 * @param line_type       Type of the line, see cvLine description. 
 * @param shift           Number of fractional bits in the point coordinates. 
 * @return CVAPI(void)
 * @uses cvDrawRectangle
 */
CV_INLINE void 
cvShowImageAndRectangle( const char* w_name, const IplImage* img, 
                         CvRect32f rect32f CV_DEFAULT(cvRect32f(0,0,1,1,0)),
                         CvPoint2D32f shear CV_DEFAULT(cvPoint2D32f(0,0)),
                         CvScalar color CV_DEFAULT(CV_RGB(255, 255, 0)), 
                         int thickness CV_DEFAULT(1), 
                         int line_type CV_DEFAULT(8), 
                         int shift CV_DEFAULT(0) )
{
    CvRect rect  = cvRectFromRect32f( rect32f );
    if( rect.width <= 0 || rect.height <= 0 ) { cvShowImage( w_name, img ); return; }
    IplImage* clone = cvCloneImage( img );
    cvDrawRectangle( clone, rect32f, shear, color, thickness, line_type, shift );
    cvShowImage( w_name, clone );
    cvReleaseImage( &clone );
}


#endif
