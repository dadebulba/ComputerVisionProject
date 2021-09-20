/*
 * state.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */
#include "state.h"
int num_states = 5;
// Definition of dynamics model
// new_particle = cvMatMul( dynamics, particle ) + noise
// curr_x =: curr_x + noise
double dynamics[] = {
    1, 0, 0, 0, 0,
    0, 1, 0, 0, 0,
    0, 0, 1, 0, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 0, 1,
};
//typedef struct CvParticleState {
//    double x;        // center coord of a rectangle
//    double y;        // center coord of a rectangle
//    double width;    // width of a rectangle
//    double height;   // height of a rectangle
//    double angle;    // rotation around center. degree
//} CvParticleState;
CvParticleState cvParticleStateFromMat( const CvMat* state )
{
    CvParticleState s;
    s.x       = cvmGet( state, 0, 0 );
    s.y       = cvmGet( state, 1, 0 );
    s.width   = cvmGet( state, 2, 0 );
    s.height  = cvmGet( state, 3, 0 );
    s.angle   = cvmGet( state, 4, 0 );
    return s;
}

/**
 * Convert a state structure to CvMat
 *
 * @param state        A CvParticleState structure
 * @param state_mat    num_states x 1 matrix
 * @return void
 */
void cvParticleStateToMat( const CvParticleState& state, CvMat* state_mat )
{
    cvmSet( state_mat, 0, 0, state.x );
    cvmSet( state_mat, 1, 0, state.y );
    cvmSet( state_mat, 2, 0, state.width );
    cvmSet( state_mat, 3, 0, state.height );
    cvmSet( state_mat, 4, 0, state.angle );
}

/**
 * Get a state from a particle filter structure
 *
 * @param p         particle filter struct
 * @param p_id      particle id
 */
CvParticleState cvParticleStateGet( const CvParticle* p, int p_id )
{
    CvMat* state, hdr;
    state = cvGetCol( p->particles, &hdr, p_id );
    return cvParticleStateFromMat( state );
}

/**
 * Set a state to a particle filter structure
 *
 * @param state     A CvParticleState structure
 * @param p         particle filter struct
 * @param p_id      particle id
 * @return void
 */
void cvParticleStateSet( CvParticle* p, int p_id, const CvParticleState& state )
{
    CvMat* state_mat, hdr;
    state_mat = cvGetCol( p->particles, &hdr, p_id );
    cvParticleStateToMat( state, state_mat );
}

/*************************** Particle Filter Configuration *********************************/

/**
 * Configuration of Particle filter
 */
void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std )
{
    // config dynamics model
    CvMat dynamicsmat = cvMat( p->num_states, p->num_states, CV_64FC1, dynamics );

    // config random noise standard deviation
    CvRNG rng = cvRNG( time( NULL ) );
    double stdarr[] = {
        std.x,
        std.y,
        std.width,
        std.height,
        std.angle
    };
    CvMat stdmat = cvMat( p->num_states, 1, CV_64FC1, stdarr );

    // config minimum and maximum values of states
    // lowerbound, upperbound, circular flag (useful for degree)
    // lowerbound == upperbound to express no bounding
    double boundarr[] = {
        0, imsize.width - 1, false,
        0, imsize.height - 1, false,
        1, imsize.width, false,
        1, imsize.height, false,
        0, 360, true
    };
    CvMat boundmat = cvMat( p->num_states, 3, CV_64FC1, boundarr );
    cvParticleSetDynamics( p, &dynamicsmat );
    cvParticleSetNoise( p, rng, &stdmat );
    cvParticleSetBound( p, &boundmat );
}

/**
 * @
 * CvParticle does not support this type of bounding currently
 * Call after transition
 */
void cvParticleStateAdditionalBound( CvParticle* p, CvSize imsize )
{
    for( int np = 0; np < p->num_particles; np++ )
    {
        double x      = cvmGet( p->particles, 0, np );
        double y      = cvmGet( p->particles, 1, np );
        double width  = cvmGet( p->particles, 2, np );
        double height = cvmGet( p->particles, 3, np );
        width = MIN( width, imsize.width - x ); // another state x is used
        height = MIN( height, imsize.height - y ); // another state y is used
        cvmSet( p->particles, 2, np, width );
        cvmSet( p->particles, 3, np, height );
    }
}

/***************************** Utility Functions ****************************************/

void cvParticleStateDisplay( const CvParticleState& state, IplImage* img, CvScalar color )
{
    CvBox32f box32f = cvBox32f( state.x, state.y, state.width, state.height, state.angle );
    CvRect32f rect32f = cvRect32fFromBox32f( box32f );
    cvDrawRectangle( img, rect32f, cvPoint2D32f(0,0), color,1,8,0 );
}

void cvParticleStatePrint( const CvParticleState& state )
{
    printf( "x :%.2f ", state.x );
    printf( "y :%.2f ", state.y );
    printf( "width :%.2f ", state.width );
    printf( "height :%.2f ", state.height );
    printf( "angle :%.2f\n", state.angle );
    fflush( stdout );
}
