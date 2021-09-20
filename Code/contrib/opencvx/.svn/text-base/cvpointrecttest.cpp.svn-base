/*
 * cvpointrecttest.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvpointrecttest.h"
CVAPI(double)
cvPointRect32fTest( const CvRect32f& rect, CvPoint2D32f pt,
                    int measure_dist CV_DEFAULT(0),
                    CvPoint2D32f shear CV_DEFAULT(cvPoint2D32f(0,0)) )
{
    CvPoint2D32f points[4];
    CvMat *contour = cvCreateMat( 1, 4, CV_32FC2 );

    cvRect32fPoints( rect, points, shear );
    for( int i = 0; i < 4; i++ )
    {
        CV_MAT_ELEM( *contour, CvPoint2D32f, 0, i ) = points[i];
    } // cvPointSeqFromMat

    /* // CV_32FC2 is not possible
    CvPoint point;
    CvSeq *contour = cvCreateSeq( CV_32SC2, sizeof( CvSeq ), sizeof( CvPoint2D32f ), cvCreateMemStorage( 0 ) );
    for( int i = 0; i < 4; i++ )
    {
        point = cvPointFrom32f( points[i] );
        cvSeqPush( contour, &point );
        }*/

    double test = cvPointPolygonTest( contour, pt, measure_dist );

    cvReleaseMat( &contour );
    //cvClearMemStorage( contour->storage );
    return test;
}
