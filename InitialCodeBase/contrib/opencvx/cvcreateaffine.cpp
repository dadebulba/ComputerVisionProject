/*
 * cvcreateaffine.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvcreateaffine.h"
CVAPI(void) cvCreateAffine( CvMat* affine,
                            CvRect32f rect CV_DEFAULT(cvRect32f(0,0,1,1,0)),
                            CvPoint2D32f shear CV_DEFAULT(cvPoint2D32f(0,0)) )
{
    double c, s;
    CvMat *R, *S, *A, hdr;
    CV_FUNCNAME( "cvCreateAffine" );
    __CV_BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );

    // affine = [ A T ]
    // A = [ a b; c d ]
    // Translation T = [ tx; ty ]
    // (1) A = Rotation * Shear(-phi) * [sx 0; 0 sy] * Shear(phi)
    // (2) A = Rotation * [sx shx; shy sy]
    // Use (2)

    // T = [tx; ty]
    cvmSet( affine, 0, 2, rect.x ); cvmSet( affine, 1, 2, rect.y );

    // R = [cos -sin; sin cos]
    R = cvCreateMat( 2, 2, affine->type );
    /*CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
      cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), rect.angle, 1.0, R ); */
    c = cos( -M_PI / 180 * rect.angle );
    s = sin( -M_PI / 180 * rect.angle );
    cvmSet( R, 0, 0, c ); cvmSet( R, 0, 1, -s );
    cvmSet( R, 1, 0, s ); cvmSet( R, 1, 1, c );

    // S = [sx shx; shy sy]
    S = cvCreateMat( 2, 2, affine->type );
    cvmSet( S, 0, 0, rect.width ); cvmSet( S, 0, 1, shear.x );
    cvmSet( S, 1, 0, shear.y );    cvmSet( S, 1, 1, rect.height );

    // A = R * S
    A = cvGetCols( affine, &hdr, 0, 2 );
    cvMatMul ( R, S, A );

    cvReleaseMat( &R );
    cvReleaseMat( &S );
    __CV_END__;
}
