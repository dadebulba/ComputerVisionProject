/*
 * cvanglemean.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */
#include "cvanglemean.h"
CVAPI(CvScalar) cvAngleMean( const CvArr *arr,
                             const CvArr *weight CV_DEFAULT(NULL),
                             double wrap CV_DEFAULT(360) )
{
    CvMat* mat, matstub;
    CvMat* wmat = NULL, wmatstub;
    CvScalar mean = cvScalar(0,0,0,0);
    int row, col, ch;
    int nChannels;
    CvScalar elem;
    CvScalar mean_cos = cvScalar(0,0,0,0);
    CvScalar mean_sin = cvScalar(0,0,0,0);
    CvScalar welem;
    CV_FUNCNAME( "cvAngleMean" );
    __CV_BEGIN__;

    mat = (CvMat*)arr;
    if( !CV_IS_MAT(mat) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub ) );
    }
    if( weight != NULL )
    {
        wmat = (CvMat*)weight;
        if( !CV_IS_MAT(wmat) )
        {
            CV_CALL( wmat = cvGetMat( wmat, &wmatstub ) );
        }
        CV_ASSERT(
            mat->rows == wmat->rows &&
            mat->cols == wmat->cols &&
            CV_MAT_CN(mat->type) == CV_MAT_CN(wmat->type)
        );
    }
    nChannels = CV_MAT_CN(mat->type);
    if( wmat == NULL ) // uniform
    {
        double w = 1.0 / (double)mat->rows * (double)mat->cols;
        welem = cvScalar( w, w, w, w );
    }
    for( row = 0; row < mat->rows; row++ )
    {
        for( col = 0; col < mat->cols; col++ )
        {
            elem = cvGet2D( mat, row, col );
            if( wmat != NULL ) welem = cvGet2D( wmat, row, col );
            for( ch = 0; ch < nChannels; ch++ )
            {
                mean_cos.val[ch] +=
                    cos( elem.val[ch] * 2*M_PI / wrap ) * welem.val[ch];
                mean_sin.val[ch] +=
                    sin( elem.val[ch] * 2*M_PI / wrap ) * welem.val[ch];
            }
        }
    }
    for( ch = 0; ch < nChannels; ch++ )
    {
        mean.val[ch] =
            atan( mean_sin.val[ch] / mean_cos.val[ch] ) * wrap / (2*M_PI);
    }
    __CV_END__;
    return mean;
}
