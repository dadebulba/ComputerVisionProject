/*
 * observetemplate.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */
#include "observetemplate.h"
#include "state.h"
#include <iostream>
using namespace std;
int num_observes = 1;
CvMat *eigenvalues;
CvMat *eigenvectors;
CvMat *eigenavg;
CvSize feature_size;
void cvParticleObserveInitialize( CvSize featsize )
{
    feature_size = featsize;
}
/**
 * Finalize
 */
void cvParticleObserveFinalize( )
{
}

/**
 * Measure and weight particles.
 *
 * The proposal function q is set p(xt|xt-1) in SIR/Condensation, and it results
 * that "weights" are set to be proportional to the likelihood probability
 * (Normalize later).
 * Rewrite here if you want to use a different proposal function q.
 *
 * CvParticleState s must have s.x, s.y, s.width, s.height, s.angle
 *
 * @param particle
 * @param frame
 * @param reference
 */
void cvParticleObserveMeasure( CvParticle* p, IplImage* frame, IplImage *reference )
{
    int i;
    double likeli;
    IplImage *patch;
    IplImage *resize;
    resize = cvCreateImage( feature_size, frame->depth, frame->nChannels );
    for( i = 0; i < p->num_particles; i++ )
    {
        CvParticleState s = cvParticleStateGet( p, i );
        CvBox32f box32f = cvBox32f( s.x, s.y, s.width, s.height, s.angle );
        CvRect32f rect32f = cvRect32fFromBox32f( box32f );
        CvRect rect = cvRectFromRect32f( rect32f );

        patch = cvCreateImage( cvSize(rect.width,rect.height), frame->depth, frame->nChannels );
        cvCropImageROI( frame, patch, rect32f,cvPoint2D32f(0,0),CV_INTER_LINEAR );
        cvResize( patch, resize );

        // log likeli. kinds of Gaussian model
        // exp( -d^2 / sigma^2 )
        // sigma can be omitted because common param does not affect ML estimate
        likeli = -cvNorm( resize, reference, CV_L2 );
        cvmSet( p->weights, 0, i, likeli );

        cvReleaseImage( &patch );
    }
    cvReleaseImage( &resize );
}


/**
 * Initialization
 */
void cvParticlePCAObserveInitialize( const char* data_pcaval,
                                  const char* data_pcavec,
                                  const char* data_pcaavg,
                                  CvSize feat_size)
{
    if( (eigenvalues = (CvMat*)cvLoad( data_pcaval )) == NULL ) {
        cerr << data_pcaval << " is not loadable." << endl << flush;
        exit( 1 );
    }
    if( (eigenvectors = (CvMat*)cvLoad( data_pcavec )) == NULL ) {
        cerr << data_pcavec << " is not loadable." << endl << flush;
        exit( 1 );
    }
    if( (eigenavg = (CvMat*)cvLoad( data_pcaavg )) == NULL ) {
        cerr << data_pcaavg << " is not loadable." << endl << flush;
        exit( 1 );
    }
    if( feat_size.width == 0 && feat_size.height == 0 ) {
        feature_size = cvSize( (int)sqrt((double)eigenavg->rows), (int)sqrt((double)eigenavg->rows) );
    } else {
        feature_size = feat_size;
    }
    if( eigenavg->rows != feature_size.width * feature_size.height ) {
        cerr << " Feature size is wrong " << feature_size.width << " x " << feature_size.height << endl << flush;
        exit( 1 );
    }
}

/**
 * Finalization
 */
void cvParticlePCAObserveFinalize()
{
    cvReleaseMat( &eigenvalues );
    cvReleaseMat( &eigenvectors );
    cvReleaseMat( &eigenavg );
}

/**
 * Preprocess as did in training PCA subspace
 */
void icvPreprocess( const IplImage* patch, CvMat *mat )
{
    IplImage *gry;
    if( patch->nChannels != 1 ) {
        gry = cvCreateImage( cvGetSize(patch), patch->depth, 1 );
        cvCvtColor( patch, gry, CV_BGR2GRAY );
    } else {
        gry = (IplImage*)patch;
    }
    IplImage *resize = cvCreateImage( cvSize(mat->rows, mat->cols), patch->depth, 1 );

    cvResize( gry, resize );
    cvConvert( resize, mat );
    cvGaussNormImage( mat, mat );

    cvReleaseImage( &resize );
    if( gry != patch )
        cvReleaseImage( &gry );
}

/**
 * Get observation features
 *
 * CvParticleState must have x, y, width, height, angle
 */
void icvGetFeatures( const CvParticle* p, const IplImage* frame, CvMat* features )
{
    int feature_height = feature_size.height;
    int feature_width  = feature_size.width;
    //cvNamedWindow( "patch" );
    CvMat* normed = cvCreateMat( feature_height, feature_width, CV_64FC1 );
    CvMat* normedT = cvCreateMat( feature_width, feature_height, CV_64FC1 );
    CvMat* feature, featurehdr;
    IplImage *patch;
    for( int n = 0; n < p->num_particles; n++ ) {
        CvParticleState s = cvParticleStateGet( p, n );
        CvBox32f box32f = cvBox32f( s.x, s.y, s.width, s.height, s.angle );
        CvRect32f rect32f = cvRect32fFromBox32f( box32f );

        // get image patch and preprocess
        patch = cvCreateImage( cvSize( cvRound( s.width ), cvRound( s.height ) ),
                               frame->depth, frame->nChannels );
        cvCropImageROI( (IplImage*)frame, patch, rect32f,cvPoint2D32f(0,0),CV_INTER_LINEAR );
        //cvShowImage( "patch", patch );
        //cvWaitKey( 10 );
        icvPreprocess( patch, normed );
        cvReleaseImage( &patch );

        // vectorize
        cvT( normed, normedT ); // transpose to make the same with matlab's reshape
        feature = cvReshape( normedT, &featurehdr, 1, feature_height * feature_width );

        cvSetCol( feature, features, n );
    }
    cvReleaseMat( &normedT );
    cvReleaseMat( &normed );
}

/**
 * Measure and weight particles.
 *
 * The proposal function q is set p(xt|xt-1) in SIR/Condensation, and it results
 * that "weights" are set to be proportional to the likelihood probability
 * (Normalize later).
 * Rewrite here if you want to use a different proposal function q.
 *
 * CvParticleState s must have s.x, s.y, s.width, s.height, s.angle
 *
 * @param particle
 * @param frame
 * @param reference
 */
void cvParticlePCAObserveMeasure( CvParticle* p, IplImage* frame )
{
    int feature_height = feature_size.height;
    int feature_width  = feature_size.width;

    // extract features from particle states
    CvMat* features = cvCreateMat( feature_height*feature_width, p->num_particles, CV_64FC1 );
    icvGetFeatures( p, frame, features );

    // Likelihood measurments
    cvMatPcaDiffs( features, eigenavg, eigenvalues, eigenvectors, p->weights, 0, true );

    cvReleaseMat( &features );
}

