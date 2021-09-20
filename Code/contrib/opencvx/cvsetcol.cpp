/*
 * cvsetcol.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */
#include "cvsetcol.h"

CVAPI(void) cvSetCols( const CvArr* src, CvArr* dst,
                       int start_col, int end_col )
{
    int coi;
    CvMat *srcmat = (CvMat*)src, srcmatstub;
    CvMat *dstmat = (CvMat*)dst, dstmatstub;
    CvMat *refmat, refmathdr;
    int cols;
    CV_FUNCNAME( "cvSetCols" );
    __CV_BEGIN__;
    if( !CV_IS_MAT(dstmat) )
    {
        CV_CALL( dstmat = cvGetMat( dstmat, &dstmatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(srcmat) )
    {
        CV_CALL( srcmat = cvGetMat( srcmat, &srcmatstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    cols = end_col - start_col;
    CV_ASSERT( srcmat->cols == cols || dstmat->cols == cols );
    if( srcmat->cols == cols )
    {
        refmat = cvGetCols( dstmat, &refmathdr, start_col, end_col );
        cvCopy( srcmat, refmat );
    }
    else
    {
        refmat = cvGetCols( srcmat, &refmathdr, start_col, end_col );
        cvCopy( refmat, dstmat );
    }
    __CV_END__;
}
