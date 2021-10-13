/*
 * cvclosing.cpp
 *
 *  Created on: 27/apr/2010
 *      Author: armanini
 */
#include "cvclosing.h"
CVAPI(void) cvClosing( const CvArr* src, CvArr* dst,
                       IplConvKernel* element CV_DEFAULT(NULL),
                       int iterations CV_DEFAULT(1) )
{
    cvDilate( dst, dst, element, iterations );
    cvErode( src, dst, element, iterations );
}
