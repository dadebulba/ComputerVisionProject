/*
 * cvprintmat.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvprintmat.h"
CVAPI(void) cvPrintMat( const CvArr* arr, bool transpose CV_DEFAULT(false) )
{
    CV_FUNCNAME( "cvPrintMat" );
    __CV_BEGIN__;
    int row, col, ch;
    int coi = 0;
    CvMat matstub, *mat = (CvMat*)arr;
    int depth, nChannels;
    CvScalar value;
    int rows, cols;
    if( !CV_IS_MAT( mat ) )
    {
        CV_CALL( mat = cvGetMat( mat, &matstub, &coi ) ); // i.e. IplImage to CvMat
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    depth = CV_MAT_DEPTH( mat->type );
    nChannels = CV_MAT_CN( mat->type );
    rows = !transpose ? mat->rows : mat->cols;
    cols = !transpose ? mat->cols : mat->rows;

    for( row = 0; row < rows; row++ )
    {
        for( col = 0; col < cols; col++ )
        {
            value = cvGet2D( mat, !transpose ? row : col, !transpose ? col : row );
            if( nChannels > 1 )
            {
                printf( "(%lf", value.val[0] );
                for( ch = 1; ch < nChannels; ch++ )
                {
                    printf( " %lf", value.val[ch] );
                }
                printf( ") " );
            }
            else
            {
                printf( "%lf ", value.val[0] );
            }
        }
        printf("\n");
    }
    fflush( stdout );
    __CV_END__;
}
