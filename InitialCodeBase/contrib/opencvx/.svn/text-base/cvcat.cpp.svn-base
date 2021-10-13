/*
 * cvcat.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */
#include "cvcat.h"
CVAPI(void) cvCat( const CvArr* src1arr, const CvArr* src2arr, CvArr* dstarr, int dim CV_DEFAULT(-1) )
{
    int coi = 0;
    CvMat *src1 = (CvMat*)src1arr, src1stub;
    CvMat *src2 = (CvMat*)src2arr, src2stub;
    CvMat *dst  = (CvMat*)dstarr, dststub;
    CV_FUNCNAME( "cvCat" );
    __CV_BEGIN__;
    if( !CV_IS_MAT(src1) )
    {
        src1 = cvGetMat( src1, &src1stub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(src2) )
    {
        src2 = cvGetMat( src2, &src2stub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( !CV_IS_MAT(dst) )
    {
        dst = cvGetMat( dst, &dststub, &coi );
        if (coi != 0) CV_ERROR_FROM_CODE(CV_BadCOI);
    }
    if( dim == -1 )
    {
        if( src1->rows == src2->rows && src1->rows == dst->rows )
        {
            dim = 0;
        }
        else if( src1->cols == src2->cols && src1->cols == dst->cols )
        {
            dim = 1;
        }
        else
        {
            CV_ERROR( CV_StsBadArg, "The size of matrices does not fit to concatinate." );
        }
    }
    if( dim == 0 ) // horizontal cat
    {
        CV_ASSERT( src1->rows == src2->rows && src1->rows == dst->rows );
        CV_ASSERT( src1->cols + src2->cols == dst->cols );
    }
    else if( dim == 1 ) // vertical cat
    {
        CV_ASSERT( src1->cols == src2->cols && src1->cols == dst->cols );
        CV_ASSERT( src1->rows + src2->rows == dst->rows );
    }
    else
    {
        CV_ERROR( CV_StsBadArg, "The dim is 0 (horizontal) or 1 (vertical) or -1 (flexible)." );
    }

    if( dim == 0 ) // horizontal cat
    {
        cvSetCols( src1, dst, 0, src1->cols );
        cvSetCols( src2, dst, src1->cols, src1->cols + src2->cols );
    }
    else // vertical cat
    {
        cvSetRows( src1, dst, 0, src1->rows ,1);
        cvSetRows( src2, dst, src1->rows, src1->rows + src2->rows ,1);
    }
    __CV_END__;
}
