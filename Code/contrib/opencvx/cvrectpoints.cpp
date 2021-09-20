/*
 * cvrectpoints.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvrectpoints.h"
CVAPI(void) cvRect32fPoints( CvRect32f rect, CvPoint2D32f pt[4],
                             CvPoint2D32f shear CV_DEFAULT(cvPoint2D32f(0,0)) )
{
    if( shear.x == 0 && shear.y == 0 )
    {
        CvBox2D box2d = cvBox2DFromRect32f( rect );
        cvBoxPoints( box2d, pt );
    }
    else
    {
        CvMat* A = cvCreateMat( 2, 3, CV_32FC1 );
        cvCreateAffine( A, rect, shear );
        pt[0].x = 0; pt[0].y = 0;
        pt[1].x = 1; pt[1].y = 0;
        pt[2].x = 1; pt[2].y = 1;
        pt[3].x = 0; pt[3].y = 1;
        CvPoint2D32f tmp;
        for( int i = 0; i < 4; i++ )
        {
            tmp.x = cvmGet( A, 0, 0 ) * pt[i].x + cvmGet( A, 0, 1 ) * pt[i].y + cvmGet( A, 0, 2 );
            tmp.y = cvmGet( A, 1, 0 ) * pt[i].x + cvmGet( A, 1, 1 ) * pt[i].y + cvmGet( A, 1, 2 );
            pt[i].x = tmp.x;
            pt[i].y = tmp.y;
        }
    }
}
