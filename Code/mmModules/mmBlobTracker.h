/*
 * mmBlobTracker.h
 *
 *  Created on: Nov 17, 2010
 *      Author: paolo
 */

#ifndef MMBLOBTRACKER_H_
#define MMBLOBTRACKER_H_

#include <cxcore.h>
#include <cvaux.h>
#include <cvaux.hpp>
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION < 2
#include <cxtypes.h>
#include <cvvidsurv.hpp>
#endif

#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 2
#include "opencv2/core/types_c.h"
#include "opencv2/legacy/blobtrack.hpp"
#endif

namespace mmLib {

namespace mmModules {

class mmBlobTracker: public CvBlobTracker {
public:
	mmBlobTracker();
	virtual ~mmBlobTracker();
};

}

}

#endif /* MMBLOBTRACKER_H_ */
