/*
 * cvinvaffine.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */
#include "cvinvaffine.h"

CVAPI(void) cvInvAffine( const CvMat* affine, CvMat* invaffine )
{
    CvMat* Affine;
    CvMat* InvAffine;
    CV_FUNCNAME( "cvCreateAffine" );
    __CV_BEGIN__;
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );
    CV_ASSERT( invaffine->rows == 2 && invaffine->cols == 3 );
    CV_ASSERT( affine->type == invaffine->type );

    Affine = cvCreateMat( 3, 3, affine->type );
    InvAffine = cvCreateMat( 3, 3, affine->type );
    cvSetIdentity( Affine );
    cvSetRows( affine, Affine, 0, 2 ,1);
    cvInv( Affine, InvAffine );
    cvSetRows( InvAffine, invaffine, 0, 2 ,1);
    cvReleaseMat( &Affine );
    cvReleaseMat( &InvAffine );
    __CV_END__;
}

