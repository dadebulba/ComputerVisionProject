/*
 * cvlogsum.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvlogsum.h"

CVAPI(CvScalar) cvLogSum( const CvArr *arr )
{
    IplImage* img = (IplImage*)arr, imgstub;
    IplImage *tmp, *tmp2;
    int ch;
    CvScalar sumval;
    CvScalar minval, maxval;
    CV_FUNCNAME( "cvLogSum" );
    __CV_BEGIN__;

    if( !CV_IS_IMAGE(img) )
    {
        CV_CALL( img = cvGetImage( img, &imgstub ) );
    }
    tmp = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );
    tmp2 = cvCreateImage( cvGetSize(img), img->depth, img->nChannels );

    // to avoid loss of precision caused by taking exp as much as possible
    // if this trick is not required, cvExp -> cvSum are enough
    for( ch = 0; ch < img->nChannels; ch++ )
    {
        cvSetImageCOI( img, ch + 1 );
        cvMinMaxLoc( img, &minval.val[ch], &maxval.val[ch] );
    }
    cvSetImageCOI( img, 0 );
    cvSubS( img, maxval, tmp );

    cvExp( tmp, tmp2 );
    sumval = cvSum( tmp2 );
    for( ch = 0; ch < img->nChannels; ch++ )
    {
        sumval.val[ch] = log( sumval.val[ch] ) + maxval.val[ch];
    }
    cvReleaseImage( &tmp );
    cvReleaseImage( &tmp2 );
    __CV_END__;
    return sumval;
}
