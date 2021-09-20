/*
 * cvpcadiffs.cpp
 *
 *  Created on: 03/mag/2010
 *      Author: armanini
 */
#include "cvpcadiffs.h"
/**
 * PCA Distance "in" and "from" feature space [1]
 *
 *   This computes a distance between a point to a PCA subspace as sum of
 *   distance-from-feature space (DFFS) and distance-in-feature-space
 *   (DIFS). The DFFS is essentially a reconstruction error and the
 *   DIFS measures (mahalanobis) distance between projected point and
 *   origin of PCA subpsace. DFFS and DIFS can be used to measure
 *   approximated likelihood Gaussian density distribution.
 *   See [1] for more details.
 *
 * @param samples             D x N sample vectors
 * @param avg                 D x 1 mean vector
 * @param eigenvalues         nEig x 1 eigen values
 * @param eigenvectors        M x D or D x M (automatically adjusted) eigen vectors
 * @param probs               1 x N computed likelihood probabilities
 * @param normalize           Compute normalization term or not
 *                            0 - nothing
 *                            1 - normalization term
 *                            2 - normalize so that sum becomes 1.0
 * @param logprob             Log probability or not
 * @see CVAPI(void) cvCalcPCA( const CvArr* data, CvArr* avg, CvArr* eigenvalues, CvArr* eigenvectors, int flags );
 * @see CVAPI(void) cvProjectPCA( const CvArr* data, const CvArr* avg, const CvArr* eigenvectors, CvArr* result );
 * @see CVAPI(void) cvBackProjectPCA( const CvArr* proj, const CvArr* avg,const CvArr* eigenvects, CvArr* result );
 *
 * References
 * @verbatim
 *   [1] @INPROCEEDINGS{Moghaddam95probabilisticvisual,
 *     author = {Baback Moghaddam and Alex Pentl},
 *     title = {Probabilistic visual learning for object detection},
 *     booktitle = {},
 *     year = {1995},
 *     pages = {786--793}
 *   }
 *   [2] @ARTICLE{Moghaddam02principalmanifolds,
 *     author = {Baback Moghaddam},
 *     title = {Principal manifolds and probabilistic subspaces for visual recognition},
 *     journal = {IEEE Transactions on Pattern Analysis and Machine Intelligence},
 *     year = {2002},
 *     volume = {24},
 *     pages = {780--788}
 * @endverbatim
 */
CVAPI(void)
cvMatPcaDiffs( const CvMat* samples, const CvMat* avg, const CvMat* eigenvalues,
               const CvMat* eigenvectors, CvMat* probs,
               int normalize CV_DEFAULT(1), bool logprob CV_DEFAULT(false) )
{
    int D = samples->rows;
    int N = samples->cols;
    int M = (eigenvectors->rows == D) ? eigenvectors->cols : eigenvectors->rows;
    int type = samples->type;
    int nEig = eigenvalues->rows;
    int d, n;
    double normterm = 0;
    CvMat *_eigenvectors; // cvProjectPCA requires M x D vec
    CvMat *proj = cvCreateMat( N, M, type );
    CvMat *subproj, subprojhdr;
    CvMat *sclsubproj = cvCreateMat( 1, M, type );
    CvMat *pLambda, pLambdaHdr;
    CvMat *rLambda, rLambdaHdr;
    CvMat *sqrt_pLambda = cvCreateMat( M, 1, type );
    CvMat *DIFS = cvCreateMat( 1, N, type );
    CvMat *DFFS = cvCreateMat( 1, N, type );
    CvMat *samples0 = cvCreateMat( D, N, type ); // mean subtracted samples
    CvMat *subsamples0, subsamples0hdr;
    CvScalar rho;
    CvMat *_proj;
    CV_FUNCNAME( "cvMatPcaDiffs" );
    __CV_BEGIN__;
    CV_ASSERT( CV_IS_MAT(samples) );
    CV_ASSERT( CV_IS_MAT(avg) );
    CV_ASSERT( CV_IS_MAT(eigenvalues) );
    CV_ASSERT( CV_IS_MAT(eigenvectors) );
    CV_ASSERT( D == eigenvectors->rows || D == eigenvectors->cols );
    CV_ASSERT( D == avg->rows && 1 == avg->cols );
    CV_ASSERT( 1 == probs->rows && N == probs->cols );

    cvZero( DIFS );
    cvZero( DFFS );

    if( D == eigenvectors->rows ) {
        _eigenvectors = cvCreateMat( M, D, type );
        cvT( eigenvectors, _eigenvectors );
    } else {
        _eigenvectors = (CvMat*)eigenvectors;
    }
    //cvProjectPCA( samples, avg, _eigenvectors, proj );
    for( n = 0; n < N; n++ ) { // want mean subtracted samples for laster too
        for( d = 0; d < D; d++ ) {
            cvmSet( samples0, d, n, cvmGet( samples, d, n ) - cvmGet( avg, d, 0 ) );
        }
    }
    _proj = cvCreateMat( M, N, type );
    cvMatMul( _eigenvectors, samples0, _proj );
    cvT( _proj, proj );
    cvReleaseMat( &_proj );

    // distance in feature space
    if( M > 0 ) {
        pLambda = cvGetRows( eigenvalues, &pLambdaHdr, 0, M );
        cvPow( pLambda, sqrt_pLambda, 0.5 );
        for( n = 0; n < N; n++ ) {
            subproj = cvGetRow( proj, &subprojhdr, n );
            for( d = 0; d < M; d++ ) {
                cvmSet( sclsubproj, 0, d, cvmGet( subproj, 0, d ) / cvmGet( sqrt_pLambda, d, 0 ) );
            }
            cvmSet( DIFS, 0, n, pow(cvNorm( sclsubproj, NULL, CV_L2 ), 2) );
        }
        if( normalize == 1 ) {
            //cvLog( sqrt_pLambda, sqrt_pLambda );
            //cvSum( sqrt_pLambda );
            for( d = 0; d < M; d++ ) {
                normterm += log( cvmGet( sqrt_pLambda, d, 0 ) );
            }
            normterm += log(2*M_PI)*(M/2.0);
        }
    }

    // distance from feature space
    if( nEig > M ) {
        rLambda = cvGetRows( eigenvalues, &rLambdaHdr, M, nEig );
        rho = cvAvg( rLambda );
        for( n = 0; n < N; n++ ) {
            subsamples0 = cvGetCol( samples0, &subsamples0hdr, n );
            subproj = cvGetRow( proj, &subprojhdr, n );
            cvmSet( DFFS, 0, n, pow(cvNorm( subsamples0, NULL, CV_L2 ),2) - pow(cvNorm( subproj, NULL, CV_L2 ),2) );
        }
        cvScale( DFFS, DFFS, 1.0/rho.val[0] );
        if( normalize == 1 ) {
            normterm += log(2*M_PI*rho.val[0]) * ((nEig - M)/2.0);
        }
    }

    // sum DIFS and DFFS in log form
    for( n = 0; n < N; n++ ) {
        cvmSet( probs, 0, n, cvmGet( DIFS, 0, n ) / (-2) + cvmGet( DFFS, 0, n) / (-2) - normterm );
    }

    // normalization and so on
    if( normalize == 2 ) {
        double minval, maxval;
        cvMinMaxLoc( probs, &minval, &maxval );
        cvSubS( probs, cvScalar( maxval ), probs );
    }
    if( !logprob || normalize == 2 ) {
        for( n = 0; n < N; n++ ) {
            cvmSet( probs, 0, n, exp(cvmGet( probs, 0, n )) );
        }
        if( normalize == 2 ) {
            CvScalar sumprob = cvSum( probs );
            cvScale( probs, probs, 1.0 / sumprob.val[0] );
        }
    }
    if( logprob && normalize == 2 ) {
        for( n = 0; n < N; n++ ) {
            cvmSet( probs, 0, n, log(cvmGet( probs, 0, n )) );
        }
    }
    cvReleaseMat( &proj );
    cvReleaseMat( &sclsubproj );
    cvReleaseMat( &sqrt_pLambda );
    cvReleaseMat( &DIFS );
    cvReleaseMat( &DFFS );
    cvReleaseMat( &samples0 );
    if( D == eigenvectors->rows ) {
        cvReleaseMat( &_eigenvectors );
    }
    __CV_END__;
}


