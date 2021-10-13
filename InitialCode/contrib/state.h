/** @file
 * Rotated rectangle state particle filter +
 * 1nd order AR dynamics model ( in fact, next = current + noise )
 *
 * Use this file as a template of definitions of states and 
 * state transition model for particle filter
 *
 * Currently cvparticle.h supports only linear combination of states transition
 * model only. you may create another cvParticleTransition to support more complex
 * non-linear state transition model. Most of other functions still should be 
 * available modifications
 *
 * The MIT License
 * 
 * Copyright (c) 2008, Naotoshi Seo <sonots(at)sonots.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef STATE_H_
#define STATE_H_

#include "opencvx/cvparticle.h"
#include "opencvx/cvdrawrectangle.h"
#include "opencvx/cvcropimageroi.h"
#include "opencvx/cvrect32f.h"
#include <float.h>
//using namespace std;

/********************** Definition of a particle *****************************/



// Definition of meanings of 5 states.
// This kinds of structures is not necessary to be defined, 
// but I recommend to define them because it makes clear meanings of states
typedef struct CvParticleState {
    double x;        // center coord of a rectangle
    double y;        // center coord of a rectangle
    double width;    // width of a rectangle
    double height;   // height of a rectangle
    double angle;    // rotation around center. degree
} CvParticleState;



/********************** Function Prototypes *********************************/

// Functions for CvParticleState structure ( constructor, getter, setter )
inline CvParticleState cvParticleState( double x, 
                                        double y, 
                                        double width, 
                                        double height, 
                                        double angle = 0 );
CvParticleState cvParticleStateFromMat( const CvMat* state );
void cvParticleStateToMat( const CvParticleState &state, CvMat* state_mat );
CvParticleState cvParticleStateGet( const CvParticle* p, int p_id );
void cvParticleStateSet( CvParticle* p, int p_id, const CvParticleState &state );

// Particle Filter configuration
void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std );
void cvParticleStateAdditionalBound( CvParticle* p, CvSize imsize );

// Utility Functions
void cvParticleStateDisplay( const CvParticleState& state, IplImage* frame, CvScalar color );
void cvParticleStatePrint( const CvParticleState& state );

/****************** Functions for CvParticleState structure ******************/

// This kinds of state definitions are not necessary, 
// but helps readability of codes for sure.

/**
 * Constructor
 */
inline CvParticleState cvParticleState( double x, 
                                        double y, 
                                        double width, 
                                        double height, 
                                        double angle )
{
    CvParticleState state = { x, y, width, height, angle }; 
    return state;
}

/**
 * Convert a matrix state representation to a state structure
 *
 * @param state     num_states x 1 matrix
// */
//CvParticleState cvParticleStateFromMat( const CvMat* state );

/**
 * Convert a state structure to CvMat
 *
 * @param state        A CvParticleState structure
 * @param state_mat    num_states x 1 matrix
 * @return void
 */
//void cvParticleStateToMat( const CvParticleState& state, CvMat* state_mat );

/**
 * Get a state from a particle filter structure
 *
 * @param p         particle filter struct
 * @param p_id      particle id
 */
//CvParticleState cvParticleStateGet( const CvParticle* p, int p_id );

/**
 * Set a state to a particle filter structure
 *
 * @param state     A CvParticleState structure
 * @param p         particle filter struct
 * @param p_id      particle id
 * @return void
 */
//void cvParticleStateSet( CvParticle* p, int p_id, const CvParticleState& state );

/*************************** Particle Filter Configuration *********************************/

/**
 * Configuration of Particle filter
 */
//void cvParticleStateConfig( CvParticle* p, CvSize imsize, CvParticleState& std );

/**
 * @
 * CvParticle does not support this type of bounding currently
 * Call after transition
 */
//void cvParticleStateAdditionalBound( CvParticle* p, CvSize imsize );

/***************************** Utility Functions ****************************************/

//void cvParticleStateDisplay( const CvParticleState& state, IplImage* img, CvScalar color );
//
//void cvParticleStatePrint( const CvParticleState& state );

#endif
