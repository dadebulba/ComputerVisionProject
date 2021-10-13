/*
 * cvcreateaffineimage.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */
#include "cvcreateaffineimage.h"
IplImage*
cvCreateAffineImage( const IplImage* src, const CvMat* affine,
                     int flags = CV_AFFINE_SAME, CvPoint* origin = NULL,
                     CvScalar bgcolor = CV_RGB(0,0,0) )
{
    IplImage* dst;
    int minx = INT_MAX;
    int miny = INT_MAX;
    int maxx = INT_MIN;
    int maxy = INT_MIN;
    int i, x, y, xx, yy, xp, yp;
    int ch, width, height;
    CvPoint pt[4];
    CvMat* invaffine;
    CV_FUNCNAME( "cvAffineImage" );
    __CV_BEGIN__;
    CV_ASSERT( src->depth == IPL_DEPTH_8U );
    CV_ASSERT( affine->rows == 2 && affine->cols == 3 );

    // cvBoxPoints supports only rotation (no shear deform)
    // original 4 corner
    pt[0].x = 0;              pt[0].y = 0;
    pt[1].x = src->width - 1; pt[1].y = 0;
    pt[2].x = 0;              pt[2].y = src->height - 1;
    pt[3].x = src->width - 1; pt[3].y = src->height - 1;
    // 4 corner after transformed
    for( i = 0; i < 4; i++ )
    {
        x = cvRound( pt[i].x * cvmGet( affine, 0, 0 ) +
                     pt[i].y * cvmGet( affine, 0, 1 ) +
                     cvmGet( affine, 0, 2 ) );
        y = cvRound( pt[i].x * cvmGet( affine, 1, 0 ) +
                     pt[i].y * cvmGet( affine, 1, 1 ) +
                     cvmGet( affine, 1, 2 ) );
        pt[i].x = x; pt[i].y = y;
    }
    // min, max
    for( i = 0; i < 4; i++ )
    {
        minx = MIN( pt[i].x, minx );
        miny = MIN( pt[i].y, miny );
        maxx = MAX( pt[i].x, maxx );
        maxy = MAX( pt[i].y, maxy );
    }
    // target image width and height
    if( flags == CV_AFFINE_FULL )
    {
        width = maxx - minx + 1;
        height = maxy - miny + 1;
    }
    else if( flags == CV_AFFINE_SAME )
    {
        width = src->width;
        height = src->height;
        minx = miny = 0;
        maxx = src->width - 1;
        maxy = src->height - 1;
    }
    //cvPrintMat( affine );
    //printf( "%d %d %d %d\n", minx, miny, maxx, maxy );
    if( origin != NULL )
    {
        origin->x = minx;
        origin->y = miny;
    }
    dst = cvCreateImage( cvSize(width, height), src->depth, src->nChannels );
    cvSet( dst, bgcolor );

    // inverse affine
    invaffine = cvCreateMat( 2, 3, affine->type );
    cvInvAffine( affine, invaffine );

    // loop based on image coordinates of transformed image
    for( x = 0; x < width; x++ )
    {
        xx = x + minx;
        for( y = 0; y < height; y++ )
        {
            yy = y + miny;
            xp = cvRound( xx * cvmGet( invaffine, 0, 0 ) +
                              yy * cvmGet( invaffine, 0, 1 ) +
                              cvmGet( invaffine, 0, 2 ) );
            yp = cvRound( xx * cvmGet( invaffine, 1, 0 ) +
                              yy * cvmGet( invaffine, 1, 1 ) +
                              cvmGet( invaffine, 1, 2 ) );
            if( xp < 0 || xp >= src->width || yp < 0 || yp >= src->height ) continue;
            for( ch = 0; ch < src->nChannels; ch++ )
            {
                dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                    = src->imageData[src->widthStep * yp + xp * src->nChannels + ch];
            }
        }
    }
    cvReleaseMat( &invaffine );
    __CV_END__;
    return dst;
}

/**
 * Create a mask image for cvCreateAffineImage
 *
 * @param src       Image. Used to get image size.
 * @param affine    2 x 3 Affine transform matrix
 * @param flags     CV_AFFINE_SAME - Outside image coordinates are cut off
 *                  CV_AFFINE_FULL - Fully contain the original image pixel values
 * @param origin    The coordinate of origin (the coordinate in original image respective to
 *                  the transformed image origin).
 *                  Useful when CV_AFFINE_FULL is used.
 */
CV_INLINE IplImage*
cvCreateAffineMask( const IplImage* src, const CvMat* affine,
                    int flags = CV_AFFINE_SAME, CvPoint* origin = NULL )
{
    IplImage* orig = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1 );
    cvSet( orig, cvScalar(1) );
    IplImage* mask = cvCreateAffineImage( orig, affine, flags, origin, cvScalar(0) );
    cvReleaseImage( &orig );
    return mask;
}
