/*+++++++++++++++++++++++++
  +mmLib
  +
  +mmLib.h
  +
  +Authors: Alfredo Armanini (armanini@disi.unitn.it)
  +
  +Copyright 2010 mmLab - Disi University of Trento
  +				    http://disi.unitn.it/
  +++++++++++++++++++++++++*/

//#define WINDOWS 1

#ifndef MMLIB_H_
#define MMLIB_H_



#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/highgui.h>
#include <opencv/ml.h>

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <ctime>

//#include <gst/gst.h>
//#include <gst/video/video.h>
//#include <gst/app/gstappsink.h>
//#include <gst/app/gstappsrc.h>
//#include <gst/riff/riff-media.h>



//
//// boost libs
//#include <boost/signals2/signal.hpp>
//#include <boost/bind.hpp>
//#include <boost/thread/thread.hpp>
//#include <boost/thread/mutex.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/thread/xtime.hpp>

//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>
//// boost libs
//#include <boost/program_options.hpp>
//#include <boost/numeric/ublas/io.hpp>
//#include <boost/numeric/ublas/vector.hpp>
//#include <boost/numeric/ublas/matrix.hpp>
//#include <boost/lexical_cast.hpp>
//#include <boost/algorithm/string.hpp>
//#include <boost/format.hpp>
//#include <boost/filesystem.hpp>
//#include <boost/typeof/typeof.hpp>
//#include <boost/scoped_ptr.hpp>

//#include <gst/gst.h>
//#include <gst/video/video.h>
//#include <gst/app/gstappsink.h>
//#include <gst/app/gstappsrc.h>
//#include <gst/riff/riff-media.h>
// TODO: pensare a levare tutti gli include mm...

//mmLib include



//#include "mmKalmanDatabase.h"

//mmLib::mmModules include
//#include "mmModules/mmBlob.h"
//#include "mmModules/bgSubtraction.h"
//#include "mmModules/appModel.h"
//#include "mmModules/blobDetection.h"
//#include "mmModules/blobStructure.h"
//#include "mmModules/kalmanFilter.h"
//#include "mmModules/mmDatabase.h"
//#include "mmModules/mmCodeBookBgSubstraction.h"
//#include "mmModules/mmCrowdAnalysis.h"
//#include "mmModules/mmTracker.h"
//#include "mmModules/mmBlobDetection.h"

//mmLib::mmAnomaly include
#include "mmAnomaly/mmGaussianModel.h"
#include "mmAnomaly/mmParticleAccumulation.h"
#include "mmAnomaly/mmParticleEnergy.h"

//mmLib::mmInteraction include
//#include "mmInteraction/mmPedestrianDetector.h"
//#include "mmInteraction/mmBehaviourDetector.h"
//#include "mmInteraction/FastHog3DComputer.h"
//#include "mmInteraction/FastVideoGradientComputer.h"


//mmLib::mmUtilities include
//#include "mmUtilities/mmHomography.h"
//#include "mmUtilities/mmMaps.h"
//#include "mmUtilities/mmFisheye.h"

//mmLib::mmVideo include
//#include "mmVideo/mmGstreamer.h"
//#include "mmVideo/mmVideoWriter.h"

//contrib
//#include "contrib/tinyxml/tinystr.h"
//#include "contrib/tinyxml/tinyxml.h"


#endif /* MMLIB_H_ */
