/*
 * cvsetrow.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvsetrow.h"

CVAPI(void) cvSetRows( const CvArr* src, CvArr* dst,
                       int start_row, int end_row, int delta_row CV_DEFAULT(1) )
{
    int coi;
    CvMat *srcmat = (CvMat*)src, srcmatstub;
    CvMat *dstmat = (CvMat*)dst, dstmatstub;
    CvMat *refmat, refmathdr;
    int rows;
    CV_FUNCNAME( "cvSetRows" );
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
    rows = cvFloor( ( end_row - start_row ) / delta_row );
    CV_ASSERT( srcmat->rows == rows || dstmat->rows == rows );
    if( srcmat->rows == rows )
    {
        refmat = cvGetRows( dstmat, &refmathdr, start_row, end_row, delta_row );
        cvCopy( srcmat, refmat );
    }
    else
    {
        refmat = cvGetRows( srcmat, &refmathdr, start_row, end_row, delta_row );
        cvCopy( refmat, dstmat );
    }
    __CV_END__;
}
