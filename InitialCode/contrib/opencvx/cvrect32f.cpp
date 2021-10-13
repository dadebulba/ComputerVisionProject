/*
 * cvrect32f.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvrect32f.h"
CvBox32f cvBox32fFromRect32f( CvRect32f rect )
{
    CvPoint2D32f c;
    // x + ( x + width - 1 ) / 2 = cx
    c.x = ( 2 * rect.x + rect.width - 1 ) / 2.0;
    c.y = ( 2 * rect.y + rect.height - 1 ) / 2.0;
    if( rect.angle != 0 )
    {
        CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix( cvPoint2D32f( rect.x, rect.y ), rect.angle, 1.0, R );
        c = cvPoint2D32f (
            cvmGet( R, 0, 0 ) * c.x + cvmGet( R, 0, 1 ) * c.y + cvmGet( R, 0, 2 ),
            cvmGet( R, 1, 0 ) * c.x + cvmGet( R, 1, 1 ) * c.y + cvmGet( R, 1, 2 ) );
        cvReleaseMat( &R );
    }
    return cvBox32f( c.x, c.y, rect.width, rect.height, rect.angle );
}

/**
 * cvRect32f from cvBox32f
 *
 * Convert center coordinate to upper-left coordinate of the rectangle
 */
CvRect32f cvRect32fFromBox32f( CvBox32f box )
{
    CvPoint2D32f l;
    // x + ( x + width - 1 ) / 2 = cx
    l.x = ( 2 * box.cx + 1 - box.width ) / 2.0;
    l.y = ( 2 * box.cy + 1 - box.height ) / 2.0;
    if( box.angle != 0.0 )
    {
        CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix( cvPoint2D32f( box.cx, box.cy ), box.angle, 1.0, R );
        l = cvPoint2D32f (
            cvmGet( R, 0, 0 ) * l.x + cvmGet( R, 0, 1 ) * l.y + cvmGet( R, 0, 2 ),
            cvmGet( R, 1, 0 ) * l.x + cvmGet( R, 1, 1 ) * l.y + cvmGet( R, 1, 2 ) );
        cvReleaseMat( &R );
    }
    return cvRect32f( l.x, l.y, box.width, box.height, box.angle );
}
