/*
 * cvcropimageroi.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvcropimageroi.h"
CVAPI(void)
cvCropImageROI( const IplImage* img, IplImage* dst,
                CvRect32f rect32f CV_DEFAULT(cvRect32f(0,0,1,1,0)),
                CvPoint2D32f shear CV_DEFAULT(cvPoint2D32f(0,0)),
                int interpolation CV_DEFAULT(CV_INTER_LINEAR) )
{
    CvRect rect = cvRectFromRect32f( rect32f );
    float angle = rect32f.angle;
    CV_FUNCNAME( "cvCropImageROI" );
    __CV_BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );
    CV_ASSERT( dst->width == rect.width );
    CV_ASSERT( dst->height == rect.height );

    if( angle == 0 && shear.x == 0 && shear.y == 0 &&
        rect.x >= 0 && rect.y >= 0 &&
        rect.x + rect.width < img->width && rect.y + rect.height < img->height )
    {
        CvMat subimg;
        cvGetSubRect( img, &subimg, rect );
        cvConvert( &subimg, dst );
    }
    else if( shear.x == 0 && shear.y == 0 )
    {
        int x, y;
        float xp, yp;
        double c = cos( -M_PI / 180 * angle );
        double s = sin( -M_PI / 180 * angle );
        CvScalar color;
        /*CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), angle, 1.0, R );
        double c = cvmGet( R, 0, 0 );
        double s = cvmGet( R, 1, 0 );
        cvReleaseMat( &R );*/
        cvZero( dst );

        for( x = 0; x < rect.width; x++ )
        {
            for( y = 0; y < rect.height; y++ )
            {
                xp = ( c * x + -s * y ) + rect.x;
                yp = ( s * x + c * y ) + rect.y;
                if( xp <= -0.5 || xp >= img->width - 0.5 || yp <= -0.5 || yp >= img->height - 0.5 ) continue;
                color = cvGet2DInter( img, yp, xp, interpolation );
                cvSet2D( dst, y, x, color );
                //xp = cvRound( c * x + -s * y ) + rect.x;
                //yp = cvRound( s * x + c * y ) + rect.y;
                //if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                //for( ch = 0; ch < img->nChannels; ch++ )
                //{
                //    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                //        = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
                //}
            }
        }
    }
    else
    {
        int x, y;
        float xp, yp;
        CvScalar color;
        CvMat* affine = cvCreateMat( 2, 3, CV_32FC1 );
        CvMat* xy     = cvCreateMat( 3, 1, CV_32FC1 );
        CvMat* xyp    = cvCreateMat( 2, 1, CV_32FC1 );
        cvCreateAffine( affine, rect32f, shear );
        cvmSet( xy, 2, 0, 1.0 );
        cvZero( dst );

        for( x = 0; x < rect.width; x++ )
        {
            cvmSet( xy, 0, 0, x / rect32f.width );
            for( y = 0; y < rect.height; y++ )
            {
                cvmSet( xy, 1, 0, y / rect32f.height );
                cvMatMul( affine, xy, xyp );
                xp = cvmGet( xyp, 0, 0 );
                yp = cvmGet( xyp, 1, 0 );
                if( xp <= -0.5 || xp >= img->width - 0.5 || yp <= -0.5 || yp >= img->height - 0.5 ) continue;
                color = cvGet2DInter( img, yp, xp, interpolation );
                cvSet2D( dst, y, x, color );
                //xp = cvRound( cvmGet( xyp, 0, 0 ) );
                //yp = cvRound( cvmGet( xyp, 1, 0 ) );
                //if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                //for( ch = 0; ch < img->nChannels; ch++ )
                //{
                //    dst->imageData[dst->widthStep * y + x * dst->nChannels + ch]
                //        = img->imageData[img->widthStep * yp + xp * img->nChannels + ch];
                //}
            }
        }
        cvReleaseMat( &affine );
        cvReleaseMat( &xy );
        cvReleaseMat( &xyp );
    }
    __CV_END__;
}
