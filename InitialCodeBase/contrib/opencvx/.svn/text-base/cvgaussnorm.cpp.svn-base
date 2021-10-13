/*
 * cvgaussnorm.cpp
 *
 *  Created on: 03/mag/2010
 *      Author: armanini
 */
#include "cvgaussnorm.h"
CVAPI(void) cvGaussNormImage( const CvArr* src, CvArr* dst )
{
    CvMat instub, *in = (CvMat*)src;
    CvMat outstub, *out = (CvMat*)dst;
    int coi = 0;
    CvScalar mean, std;
    int rows, cols, nChannels;
    int ch, row, col;
    CvScalar inval;
    CvMat *tmp_in;
    CvMat *sub_in;
    CV_FUNCNAME( "cvGaussNormImage" );
    __CV_BEGIN__;
    if( !CV_IS_MAT(in) )
    {
        CV_CALL( in = cvGetMat( in, &instub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(out) )
    {
        CV_CALL( out = cvGetMat( out, &outstub, &coi ) );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    CV_ASSERT( in->rows == out->rows && in->cols == out->cols );
    CV_ASSERT( CV_MAT_CN(in->type) == CV_MAT_CN(out->type) );

    if( in->type != out->type ) {
        tmp_in = cvCreateMat( out->rows, out->cols, out->type );
        cvConvert( in, tmp_in );
    } else {
        tmp_in = in;
    }
    sub_in = cvCreateMat( out->rows, out->cols, out->type );

    cvAvgSdv( tmp_in, &mean, &std );
    cvSubS( tmp_in, mean, sub_in );
    //cvScale( sub_in, out, 1.0/std.val[0] ); // do channel
    rows = out->rows;
    cols = out->cols;
    nChannels = CV_MAT_CN(out->type);
    for( row = 0; row < rows; row++ ) {
        for( col = 0; col < cols; col++ ) {
            inval = cvGet2D( sub_in, row, col );
            for( ch = 0; ch < nChannels; ch++ ) {
                inval.val[ch] /= std.val[ch];
            }
            cvSet2D( out, row, col, inval );
        }
    }

    if( in->type != out->type ) {
        cvReleaseMat( &tmp_in );
    }
    cvReleaseMat( &sub_in );
    __CV_END__;
}
