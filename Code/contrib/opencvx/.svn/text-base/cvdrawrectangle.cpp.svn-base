/*
 * cvdrawrectangle.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */

#include "cvdrawrectangle.h"
CVAPI(void)
cvDrawRectangle( IplImage* img,
                 CvRect32f rect32f CV_DEFAULT(cvRect32f(0,0,1,1,0)),
                 CvPoint2D32f shear CV_DEFAULT(cvPoint2D32f(0,0)),
                 CvScalar color CV_DEFAULT(CV_RGB(255, 255, 255)),
                 int thickness CV_DEFAULT(1),
                 int line_type CV_DEFAULT(8),
                 int shift CV_DEFAULT(0) )
{
    CvRect rect = cvRectFromRect32f( rect32f );
    float angle = rect32f.angle;
    CV_FUNCNAME( "cvDrawRectangle" );
    __CV_BEGIN__;
    CV_ASSERT( rect.width > 0 && rect.height > 0 );

    if( angle == 0 && shear.x == 0 && shear.y == 0 )
    {
        CvPoint pt1 = cvPoint( rect.x, rect.y );
        CvPoint pt2 = cvPoint( rect.x + rect.width - 1, rect.y + rect.height - 1 );
        cvRectangle( img, pt1, pt2, color, thickness, line_type, shift );
    }
    else if( shear.x == 0 && shear.y == 0 )
    {
        int x, y, ch, xp, yp;
        double c = cos( -M_PI / 180 * angle );
        double s = sin( -M_PI / 180 * angle );
        /*CvMat* R = cvCreateMat( 2, 3, CV_32FC1 );
        cv2DRotationMatrix( cvPoint2D32f( 0, 0 ), angle, 1.0, R );
        double c = cvmGet( R, 0, 0 );
        double s = cvmGet( R, 1, 0 );
        cvReleaseMat( &R );*/

        for( x = 0; x < rect.width; x++ )
        {
            for( y = 0; y < rect.height; y += MAX(1, rect.height - 1) )
            {
                xp = cvRound( c * x + -s * y ) + rect.x;
                yp = cvRound( s * x + c * y ) + rect.y;
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }

        for( y = 0; y < rect.height; y++ )
        {
            for( x = 0; x < rect.width; x += MAX( 1, rect.width - 1) )
            {
                xp = cvRound( c * x + -s * y ) + rect.x;
                yp = cvRound( s * x + c * y ) + rect.y;
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }
    }
    else
    {
        int x, y, ch, xp, yp;
        CvMat* affine = cvCreateMat( 2, 3, CV_32FC1 );
        CvMat* xy     = cvCreateMat( 3, 1, CV_32FC1 );
        CvMat* xyp    = cvCreateMat( 2, 1, CV_32FC1 );
        cvmSet( xy, 2, 0, 1.0 );
        cvCreateAffine( affine, rect32f, shear );

        for( x = 0; x < rect.width; x++ )
        {
            cvmSet( xy, 0, 0, x / rect32f.width );
            for( y = 0; y < rect.height; y += MAX(1, rect.height - 1) )
            {
                cvmSet( xy, 1, 0, y / rect32f.height );
                cvMatMul( affine, xy, xyp );
                xp = cvRound( cvmGet( xyp, 0, 0 ) );
                yp = cvRound( cvmGet( xyp, 1, 0 ) );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }
        for( y = 0; y < rect.height; y++ )
        {
            cvmSet( xy, 1, 0, y / rect32f.height );
            for( x = 0; x < rect.width; x += MAX( 1, rect.width - 1) )
            {
                cvmSet( xy, 0, 0, x / rect32f.width );
                cvMatMul( affine, xy, xyp );
                xp = cvRound( cvmGet( xyp, 0, 0 ) );
                yp = cvRound( cvmGet( xyp, 1, 0 ) );
                if( xp < 0 || xp >= img->width || yp < 0 || yp >= img->height ) continue;
                for( ch = 0; ch < img->nChannels; ch++ )
                {
                    img->imageData[img->widthStep * yp + xp * img->nChannels + ch] = (char)color.val[ch];
                }
            }
        }
        cvReleaseMat( &affine );
        cvReleaseMat( &xy );
        cvReleaseMat( &xyp );
    }
    __CV_END__;
}
