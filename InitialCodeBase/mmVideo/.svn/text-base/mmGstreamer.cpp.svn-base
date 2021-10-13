#include "../mmLib.h"
static bool isInited = false;
static bool static_qt_image=false;

namespace mmLib {
namespace mmVideo{
mmGstreamer::mmGstreamer(const std::string &uri,bool ffmpeg, bool realtime, bool sync, bool qtOutput, bool signal_emitter)
{
	_uri=uri;
	if(_uri.find("http://")==std::string::npos
			&& _uri.find("rtsp://")==std::string::npos
			&& _uri.find("file://")==std::string::npos)
		_uri="file://"+_uri;

    _signal_emitter = signal_emitter;

    _realtime=realtime;
    _sync=sync;
    _qtOoutput=qtOutput;
    static_qt_image=_qtOoutput;
    _isPlaying=false;
    test=1488;
    _ffmpeg=ffmpeg;
    _frameNumber=0;
    _lastGetFrameNumber=0;
    _resuming=false;
    _killThread=false;
    if(_ffmpeg)
    {
    	_threadMutex = new boost::mutex();
    	_threadToInit=true;
    	cap= new cv::VideoCapture(_uri);
    }
    else
    {
    	init();
    	_successOpen=open(1,_uri.c_str());
    }
    m_bEndOfStreamReached=false;


}

void mmGstreamer::init()
{
    pipeline=0;
    frame=0;
    buffer=0;
    frame=0;
    m_bEndOfStreamReached=false;

}
bool mmGstreamer::isEndOfStreamReached()
{
	return m_bEndOfStreamReached;
}
void mmGstreamer::on_new_buffer(GstElement * elt, void * data)
{
    mmGstreamer *parent=(mmGstreamer *)data;
    guint size;
    GstBuffer *buffer;
    //get the buffer from appsink
    buffer = gst_app_sink_pull_buffer (GST_APP_SINK (elt));


    IplImage *_frame;
    gint height, width;
    GstCaps *buff_caps = gst_buffer_get_caps(buffer);
    assert(gst_caps_get_size(buff_caps) == 1);
    GstStructure* structure = gst_caps_get_structure(buff_caps, 0);

    gst_structure_get_int(structure, "width", &width);
    gst_structure_get_int(structure, "height", &height);

    _frame = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
    gst_caps_unref(buff_caps);
    // no need to memcpy, just use gstreamer's buffer :-)
    _frame->imageData = (char *)GST_BUFFER_DATA(buffer);
    cv::Mat myMat(_frame);
    cv::Mat rgbFrame(myMat.rows, myMat.cols, CV_8UC3);
    if(parent->_qtOoutput)
    {
        int from_to[] = { 0,2,  1,1,  2,0};
        cv::mixChannels(&myMat,1,&rgbFrame,1,from_to,3);
        parent->_newOpenCVFrameSignal(rgbFrame);
        //return rgbFrame;
    }
    else
    	parent->_newOpenCVFrameSignal(myMat);
    gst_buffer_unref(buffer);
}

bool mmGstreamer::play()
{
	m_bEndOfStreamReached=false;
	_isPlaying=true;
	_streamError=false;
	if(_ffmpeg)
	{
		if(!_resuming){
		_lastAccessTime=(double)cv::getTickCount();
		_lastFrameTimeStamp=0;
		_firstTimeAccess=true;
		if(_realtime && _threadToInit)
		{
			_threadToInit=false;
			_frameThread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&mmLib::mmVideo::mmGstreamer::internalGrabber, this)));
		}
		}
		return true;

	}
	else
	{
		return setState(GST_STATE_PLAYING);
	}
}

bool mmGstreamer::pause()
{
	_isPlaying=false;
	_resuming=true;
	if(!_ffmpeg)
	{
		return setState(GST_STATE_PAUSED);
	}
	else
		return true;

}
bool mmGstreamer::stop()
{
	_resuming=false;
	_isPlaying=false;
	if(!_ffmpeg)
	{
		return setState(GST_STATE_READY);
	}
	else
	{
		_killThread=true;
		_frameNumber=0;
		_lastGetFrameNumber=0;
		_threadToInit=true;
		if(_realtime){
			assert(_frameThread);
			_frameThread->join();
			//_frameThread->
		}
		delete cap;
		cap= new cv::VideoCapture(_uri);
		return true;
	}
}
void mmGstreamer::internalGrabber()
{
	bool _canGo=true;
	_signalLocked=false;
	while(_canGo)
	{

		if(_isPlaying)
		{
			cv::Mat myMat;
			*cap >> myMat;
			if(myMat.empty()){
				m_bEndOfStreamReached = true;
				_streamError=true;
				return;}
			if(_firstTimeAccess)
			{
				_lastFrameTimeStamp = cap->get(CV_CAP_PROP_POS_MSEC);
				_timeStampOffset=_lastFrameTimeStamp;
				_firstTimeAccess=false;
			}
			double timeElapsedMs=(((double)getTickCount() - _lastAccessTime)/getTickFrequency())*1000;
			double frameTimeStamp = cap->get(CV_CAP_PROP_POS_MSEC);
			//std::cout << frameTimeStamp << " <- time: " << _isPlaying <<  std::endl;
			if(timeElapsedMs<(frameTimeStamp - _lastFrameTimeStamp))
			{
				double toWait = (frameTimeStamp - _lastFrameTimeStamp)-timeElapsedMs;
				usleep((unsigned long)(toWait*1000));
			}
			_lastFrameTimeStamp = cap->get(CV_CAP_PROP_POS_MSEC);
			_lastAccessTime=(double)cv::getTickCount();
			_threadMutex->lock();
			_lastFrame=myMat.clone();
			_frameNumber++;
			_threadMutex->unlock();
			if(_signal_emitter)
			{
				if(!_signalLocked)
					boost::shared_ptr<boost::thread> frameNew = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&mmLib::mmVideo::mmGstreamer::processNewOpenCvSignal, this)));

			}

		}
		else
		{
			if(_killThread)
			{
				_killThread=false;
				return;
			}
			usleep(2000);
		}
	}
}
void mmGstreamer::processNewOpenCvSignal()
{
	_signalLocked=true;
	_threadMutex->lock();
	cv::Mat myMat=_lastFrame.clone();
	_threadMutex->unlock();
	cv::Mat rgbFrame(myMat.rows, myMat.cols, CV_8UC3);
	if(_qtOoutput)
	{
		int from_to[] = { 0,2,  1,1,  2,0};
		cv::mixChannels(&myMat,1,&rgbFrame,1,from_to,3);
		_newOpenCVFrameSignal(rgbFrame);
	}
	else
		_newOpenCVFrameSignal(myMat);
	_signalLocked=false;
}
cv::Mat mmGstreamer::getFrame()
{
	if(_ffmpeg)
	{

		if(!_realtime)
		{
			cv::Mat myMat;
			*cap >> myMat;
			if(myMat.empty())
			{
				m_bEndOfStreamReached = true;
				return cv::Mat();
			}
			if(_firstTimeAccess)
			{
				_lastFrameTimeStamp = cap->get(CV_CAP_PROP_POS_MSEC);
				_timeStampOffset=_lastFrameTimeStamp;
				_firstTimeAccess=false;
			}
//			double frameTimeStamp = cap->get(CV_CAP_PROP_POS_MSEC);
//			std::cout << frameTimeStamp << " <- time " << std::endl;
			if(_sync)
			{
				double timeElapsedMs=(((double)getTickCount() - _lastAccessTime)/getTickFrequency())*1000;
				double frameTimeStamp = cap->get(CV_CAP_PROP_POS_MSEC);
				//std::cout << frameTimeStamp << " <- time " << std::endl;
				if(timeElapsedMs<(frameTimeStamp - _lastFrameTimeStamp))
				{
					double toWait = (frameTimeStamp - _lastFrameTimeStamp)-timeElapsedMs;
					usleep((unsigned long)(toWait*1000));
				}
			}
			//Check end of stream
			if(myMat.size().area()<1)
				m_bEndOfStreamReached=true;
			cv::Mat rgbFrame(myMat.rows, myMat.cols, CV_8UC3);
			if(_qtOoutput)
			{
				int from_to[] = { 0,2,  1,1,  2,0};
				cv::mixChannels(&myMat,1,&rgbFrame,1,from_to,3);
				return rgbFrame;
			}
			else
				return myMat;

			_lastFrameTimeStamp = cap->get(CV_CAP_PROP_POS_MSEC);
			_lastAccessTime=(double)cv::getTickCount();
		}
		//Realtime
		else
		{
			if (_streamError)
			{
				m_bEndOfStreamReached = true;
				return cv::Mat();
			}
			while(_lastGetFrameNumber>=_frameNumber)
				usleep(5000);
			_lastGetFrameNumber=_frameNumber;
			_threadMutex->lock();
			cv::Mat myMat=_lastFrame.clone();
			_threadMutex->unlock();
			cv::Mat rgbFrame(myMat.rows, myMat.cols, CV_8UC3);
			if(myMat.size().area()<1)
				m_bEndOfStreamReached=true;
			if(_qtOoutput)
			{
				int from_to[] = { 0,2,  1,1,  2,0};
				cv::mixChannels(&myMat,1,&rgbFrame,1,from_to,3);
				return rgbFrame;
			}
			else
				return myMat;

		}

	}
	else
	{
		if(grabFrame() && !_signal_emitter)
		{
			IplImage* iplMat = retrieveFrame(0);
			cv::Mat myMat(iplMat);
			cv::Mat rgbFrame(myMat.rows, myMat.cols, CV_8UC3);
			if(myMat.size().area()<1)
				m_bEndOfStreamReached=true;
			if(_qtOoutput)
			{
				int from_to[] = { 0,2,  1,1,  2,0};
				cv::mixChannels(&myMat,1,&rgbFrame,1,from_to,3);
				return rgbFrame;
			}
			else
				return myMat;
		}
		else
		{
			m_bEndOfStreamReached=true;
			return cv::Mat();
		}
	}
}
bool mmGstreamer::seek(double position,bool position_in_frame)
{
    GstFormat format;
    GstSeekFlags flags;
    if(!pipeline && !_ffmpeg) {
        std::cout << "GStreamer: no pipeline"<< std::endl;
        return false;
    }
    if(position_in_frame)
    {
    	if(!_ffmpeg){
    		format = GST_FORMAT_DEFAULT;
    		flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE);
    		if(!gst_element_seek_simple(GST_ELEMENT(pipeline), format,
    				flags, (gint64) position)) {
    			std::cout <<"GStreamer: unable to seek"<< std::endl;
    			return false;
    		}
    	}
    	else
    	{
    		return cap->set(CV_CAP_PROP_POS_FRAMES,position);
    	}
    }
    else
    {
    	if(!_ffmpeg)
    	{
    		format = GST_FORMAT_TIME;
    		flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE);
    		if(!gst_element_seek_simple(GST_ELEMENT(pipeline), format,
    				flags, (gint64) (position * GST_MSECOND))) {
    			std::cout <<"GStreamer: unable to seek"<< std::endl;
    			return false;
    		}
    	}
    	else
    	{
    		return cap->set(CV_CAP_PROP_POS_MSEC,position);
    	}
    }

}
double mmGstreamer::getActualPostion(bool in_frame)
{
    GstFormat format;
    gint64 value;

    if(!pipeline && !_ffmpeg) {
        std::cout << "GStreamer: no pipeline"<< std::endl;
        return -1;
    }
    if(in_frame)
    {
    	if(!_ffmpeg){
    		format = GST_FORMAT_DEFAULT;
    		if(!gst_element_query_position(sink, &format, &value)) {
    			std::cout <<"GStreamer: unable to query position of stream"<< std::endl;
    			return -1;
    		}
    		return value;
    	}
    	else
    		return cap->get(CV_CAP_PROP_POS_FRAMES);

    }
    else
    {
    	if(!_ffmpeg){
        format = GST_FORMAT_TIME;
        if(!gst_element_query_position(sink, &format, &value)) {
            std::cout <<"GStreamer: unable to query position of stream"<< std::endl;
            return -1;
        }
        return value * 1e-6; // nano seconds to milli seconds
    	}
    	else
    	{
    		double millPosition=cap->get(CV_CAP_PROP_POS_MSEC);
    		return millPosition-_timeStampOffset;
    	}
    }
}

double mmGstreamer::getStreamDuration(bool in_frame)
{
    GstFormat format;
    gint64 value;

    if(in_frame)
    {
    	if(!_ffmpeg){
    		format = GST_FORMAT_DEFAULT;
    		if(!gst_element_query_duration(pipeline, &format, &value)) {
    			std::cout <<"GStreamer: unable to query pduration of stream"<< std::endl;
    			return false;
    		}
    		return value;
    	}
    	else
    	{
    		return cap->get(CV_CAP_PROP_FRAME_COUNT);
    	}
    }
    else
    {
    	if(!_ffmpeg)
    	{
    		format = GST_FORMAT_TIME;
    		if(!gst_element_query_duration(pipeline, &format, &value)) {
    			std::cout <<"GStreamer: unable to query duration of stream"<< std::endl;
    			return false;
    		}
    		return value * 1e-6;
    	}
    	else
    	{
    		return false;
    	}
    }
}

bool mmGstreamer::setState(GstState state)
{
    if(_successOpen)
    {
        if(gst_element_set_state(GST_ELEMENT(pipeline), state) ==
                GST_STATE_CHANGE_FAILURE) {
            gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
            std::cout << "GStreamer: unable to set pipeline to" << state << std::endl;
            gst_object_unref(pipeline);
            return false;
        }
        handleMessage();
        return true;
    }
    return false;
}



void mmGstreamer::handleMessage()
{
    GstBus* bus = gst_element_get_bus(pipeline);

    while(gst_bus_have_pending(bus)) {
        GstMessage* msg = gst_bus_pop(bus);

//        printf("Got %s message\n", GST_MESSAGE_TYPE_NAME(msg));

        switch (GST_MESSAGE_TYPE (msg)) {
        case GST_MESSAGE_STATE_CHANGED:
            GstState oldstate, newstate, pendstate;
            gst_message_parse_state_changed(msg, &oldstate, &newstate, &pendstate);
//            printf("state changed from %d to %d (%d)\n", oldstate, newstate, pendstate);
            break;
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug;
            gst_message_parse_error(msg, &err, &debug);

            fprintf(stderr, "GStreamer Plugin: Embedded video playback halted; module %s reported: %s\n",
                  gst_element_get_name(GST_MESSAGE_SRC (msg)), err->message);

            g_error_free(err);
            g_free(debug);

            gst_element_set_state(pipeline, GST_STATE_NULL);

            break;
            }
        case GST_MESSAGE_EOS:

            std::cout << "NetStream has reached the end of the stream."<< std::endl;
            break;
        default:
            std::cout << "unhandled message\n" << std::endl;
            break;
        }

        gst_message_unref(msg);
    }

    gst_object_unref(GST_OBJECT(bus));
}
bool mmGstreamer::grabFrame()
{

    if(!pipeline)
        return false;

    if(gst_app_sink_is_eos(GST_APP_SINK(sink)))
        return false;

    if(buffer)
        gst_buffer_unref(buffer);
    handleMessage();

    buffer = gst_app_sink_pull_buffer(GST_APP_SINK(sink));
    if(!buffer)
        return false;

    return true;
}

//
// decode buffer
//
IplImage * mmGstreamer::retrieveFrame(int)
{
    if(!buffer)
        return false;

    if(!frame) {
        gint height, width;
        GstCaps *buff_caps = gst_buffer_get_caps(buffer);
        //std::cout << (int)gst_caps_get_size(buff_caps) << " sixe"<<std::endl;
        assert(gst_caps_get_size(buff_caps) == 1);
        GstStructure* structure = gst_caps_get_structure(buff_caps, 0);

        if(!gst_structure_get_int(structure, "width", &width) ||
           !gst_structure_get_int(structure, "height", &height))
            return false;

        frame = cvCreateImageHeader(cvSize(width, height), IPL_DEPTH_8U, 3);
        gst_caps_unref(buff_caps);
    }

    // no need to memcpy, just use gstreamer's buffer :-)
    frame->imageData = (char *)GST_BUFFER_DATA(buffer);
    //memcpy (frame->imageData, GST_BUFFER_DATA(buffer), GST_BUFFER_SIZE (buffer));
    //gst_buffer_unref(buffer);
    //buffer = 0;
    return frame;
}

void mmGstreamer::restartPipeline()
{


    printf("restarting pipeline, going to ready\n");

    if(gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_READY) ==
       GST_STATE_CHANGE_FAILURE) {
        //  CV_ERROR(CV_StsError, "GStreamer: unable to start pipeline\n");
        return;
    }

    printf("ready, relinking\n");

    gst_element_unlink(uridecodebin, color);
    printf("filtering with %s\n", gst_caps_to_string(caps));
    gst_element_link_filtered(uridecodebin, color, caps);

    printf("relinked, pausing\n");

    if(gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING) ==
       GST_STATE_CHANGE_FAILURE) {
    //    CV_ERROR(CV_StsError, "GStreamer: unable to start pipeline\n");
        return;
    }

    printf("state now paused\n");

    // __END__;
}

void mmGstreamer::setFilter(const char *property, int type, int v1, int v2)
{

    if(!caps) {
        if(type == G_TYPE_INT)
            caps = gst_caps_new_simple("video/x-raw-rgb", property, type, v1, NULL);
        else
            caps = gst_caps_new_simple("video/x-raw-rgb", property, type, v1, v2, NULL);
    } else {
        //printf("caps before setting %s\n", gst_caps_to_string(caps));
        if(type == G_TYPE_INT)
            gst_caps_set_simple(caps, "video/x-raw-rgb", property, type, v1, NULL);
        else
            gst_caps_set_simple(caps, "video/x-raw-rgb", property, type, v1, v2, NULL);
    }

    restartPipeline();
}

void mmGstreamer::removeFilter(const char *filter)
{
    if(!caps)
        return;

    GstStructure *s = gst_caps_get_structure(caps, 0);
    gst_structure_remove_field(s, filter);

    restartPipeline();
}



void mmGstreamer::newPad(GstElement *uridecodebin,
                             GstPad     *pad,
                             gpointer    data)
{
    GstPad *sinkpad;
    GstElement *color = (GstElement *) data;


    sinkpad = gst_element_get_static_pad (color, "sink");

//  printf("linking dynamic pad to colourconverter %p %p\n", uridecodebin, pad);

    gst_pad_link (pad, sinkpad);

    gst_object_unref (sinkpad);
}

bool mmGstreamer::open( int type, const char* filename )
{
    close();

    if(!isInited) {
//        printf("gst_init\n");
        gst_init (NULL, NULL);

//        gst_debug_set_active(TRUE);
//        gst_debug_set_colored(TRUE);
//        gst_debug_set_default_threshold(GST_LEVEL_WARNING);

        isInited = true;
    }
    bool stream = false;
    bool manualpipeline = false;
    char *uri = NULL;
    uridecodebin = NULL;


    if(!gst_uri_is_valid(filename)) {
        uri = realpath(filename, NULL);
        stream=false;
        if(uri) {
            uri = g_filename_to_uri(uri, NULL, NULL);
            if(!uri) {
                std::cout << "GStreamer: Error opening file\n" << std::endl;
                close();
                return false;
            }
        } else {
            GError *err = NULL;
            //uridecodebin = gst_parse_bin_from_description(filename, FALSE, &err);
            uridecodebin = gst_parse_launch(filename, &err);
            if(!uridecodebin) {
                std::cout << "GStreamer: Error opening bin\n" << std::endl;
                close();
                return false;
            }
            stream = true;
            manualpipeline = true;
        }
    } else {
        stream = true;
        uri = g_strdup(filename);
    }

    if(!uridecodebin) {
        uridecodebin = gst_element_factory_make ("uridecodebin", NULL);
        g_object_set(G_OBJECT(uridecodebin),"uri",uri, NULL);
        if(!uridecodebin) {
            std::cout << "GStreamer: Failed to create uridecodebin\n" << std::endl;
            close();
            return false;
        }
    }

    if(manualpipeline) {
        GstIterator *it = gst_bin_iterate_sinks(GST_BIN(uridecodebin));
        if(gst_iterator_next(it, (gpointer *)&sink) != GST_ITERATOR_OK) {
            std::cout << "GStreamer: cannot find appsink in manual pipeline\n" << std::endl;
            return false;
        }

        pipeline = uridecodebin;
    } else {
        pipeline = gst_pipeline_new (NULL);

        color = gst_element_factory_make("ffmpegcolorspace", NULL);
        sink = gst_element_factory_make("appsink", NULL);

        gst_bin_add_many(GST_BIN(pipeline), uridecodebin, color, sink, NULL);
        g_signal_connect(uridecodebin, "pad-added", G_CALLBACK(newPad), color);

        if(!gst_element_link(color, sink)) {
            //CV_ERROR(CV_StsError, "GStreamer: cannot link color -> sink\n");
            std::cout << "GStreamer: cannot link color -> sink\n" << std::endl;
            gst_object_unref(pipeline);
            return false;
        }
    }
    if(!_sync)
        g_object_set (G_OBJECT (sink), "sync", FALSE, NULL);
    gst_app_sink_set_max_buffers (GST_APP_SINK(sink), 1);
    gst_app_sink_set_drop (GST_APP_SINK(sink), _realtime);


    if(_signal_emitter)
    {
       gst_app_sink_set_emit_signals(GST_APP_SINK(sink),TRUE);
       g_signal_connect(sink, "new-buffer", G_CALLBACK(on_new_buffer), this);
    }

    {
    GstCaps* caps;
    caps = gst_caps_new_simple("video/x-raw-rgb",
                               "red_mask",   G_TYPE_INT, 0x0000FF,
                               "green_mask", G_TYPE_INT, 0x00FF00,
                               "blue_mask",  G_TYPE_INT, 0xFF0000,
                               NULL);
    gst_app_sink_set_caps(GST_APP_SINK(sink), caps);
    gst_caps_unref(caps);
    }

    if(gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_READY) ==
       GST_STATE_CHANGE_FAILURE) {
        std::cout << "GStreamer: unable to set pipeline to ready\n" << std::endl;
        gst_object_unref(pipeline);
        return false;
    }

//    if(gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_PLAYING) ==
//       GST_STATE_CHANGE_FAILURE) {
//        gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
//        std::cout << "GStreamer: unable to set pipeline to playing\n" << std::endl;
//        gst_object_unref(pipeline);
//        return false;
//    }



    handleMessage();


    return true;
}
void mmGstreamer::close()
{
    if(pipeline && !_ffmpeg) {
        gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(pipeline));
    }
    if(buffer && !_ffmpeg)
        gst_buffer_unref(buffer);
    if(frame && !_ffmpeg) {
        frame->imageData = 0;
        cvReleaseImage(&frame);
    }
    if(_ffmpeg)
    {
    	delete _threadMutex;
    	_lastFrame.release();
    	delete cap;
    }
}

double mmGstreamer::getProperty( int propId )
{
    GstFormat format;
    //GstQuery q;
    gint64 value;

    if(!pipeline) {
        //CV_WARN("GStreamer: no pipeline");
        return false;
    }

//    switch(propId) {
//    case CV_CAP_PROP_POS_MSEC:
//        format = GST_FORMAT_TIME;
//        if(!gst_element_query_position(sink, &format, &value)) {
//            CV_WARN("GStreamer: unable to query position of stream");
//            return false;
//        }
//        return value * 1e-6; // nano seconds to milli seconds
//    case CV_CAP_PROP_POS_FRAMES:
//        format = GST_FORMAT_DEFAULT;
//        if(!gst_element_query_position(sink, &format, &value)) {
//            CV_WARN("GStreamer: unable to query position of stream");
//            return false;
//        }
//        return value;
//    case CV_CAP_PROP_POS_AVI_RATIO:
//        format = GST_FORMAT_PERCENT;
//        if(!gst_element_query_position(pipeline, &format, &value)) {
//            CV_WARN("GStreamer: unable to query position of stream");
//            return false;
//        }
//        return ((double) value) / GST_FORMAT_PERCENT_MAX;
//    case CV_CAP_PROP_FRAME_WIDTH:
//    case CV_CAP_PROP_FRAME_HEIGHT:
//    case CV_CAP_PROP_FPS:
//    case CV_CAP_PROP_FOURCC:
//        break;
//    case CV_CAP_PROP_FRAME_COUNT:
//        format = GST_FORMAT_DEFAULT;
//        if(!gst_element_query_duration(pipeline, &format, &value)) {
//            CV_WARN("GStreamer: unable to query position of stream");
//            return false;
//        }
//        return value;
//    case CV_CAP_PROP_FORMAT:
//    case CV_CAP_PROP_MODE:
//    case CV_CAP_PROP_BRIGHTNESS:
//    case CV_CAP_PROP_CONTRAST:
//    case CV_CAP_PROP_SATURATION:
//    case CV_CAP_PROP_HUE:
//    case CV_CAP_PROP_GAIN:
//    case CV_CAP_PROP_CONVERT_RGB:
//        break;
//    case CV_CAP_GSTREAMER_QUEUE_LENGTH:
//        if(!sink) {
//                CV_WARN("GStreamer: there is no sink yet");
//                return false;
//        }
//        return gst_app_sink_get_max_buffers(GST_APP_SINK(sink));
//    default:
//        CV_WARN("GStreamer: unhandled property");
//        break;
//    }
    return false;
}

bool mmGstreamer::setProperty( int propId, double value )
{
    GstFormat format;
    GstSeekFlags flags;

    if(!pipeline) {
        //CV_WARN("GStreamer: no pipeline");
        return false;
    }

//    switch(propId) {
//    case CV_CAP_PROP_POS_MSEC:
//        format = GST_FORMAT_TIME;
//        flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE);
//        if(!gst_element_seek_simple(GST_ELEMENT(pipeline), format,
//                        flags, (gint64) (value * GST_MSECOND))) {
//            CV_WARN("GStreamer: unable to seek");
//        }
//        break;
//    case CV_CAP_PROP_POS_FRAMES:
//        format = GST_FORMAT_DEFAULT;
//        flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE);
//        if(!gst_element_seek_simple(GST_ELEMENT(pipeline), format,
//                        flags, (gint64) value)) {
//            CV_WARN("GStreamer: unable to seek");
//        }
//        break;
//    case CV_CAP_PROP_POS_AVI_RATIO:
//        format = GST_FORMAT_PERCENT;
//        flags = (GstSeekFlags) (GST_SEEK_FLAG_FLUSH|GST_SEEK_FLAG_ACCURATE);
//        if(!gst_element_seek_simple(GST_ELEMENT(pipeline), format,
//                        flags, (gint64) (value * GST_FORMAT_PERCENT_MAX))) {
//            CV_WARN("GStreamer: unable to seek");
//        }
//        break;
//    case CV_CAP_PROP_FRAME_WIDTH:
//        if(value > 0)
//            setFilter("width", G_TYPE_INT, (int) value, 0);
//        else
//            removeFilter("width");
//        break;
//    case CV_CAP_PROP_FRAME_HEIGHT:
//        if(value > 0)
//            setFilter("height", G_TYPE_INT, (int) value, 0);
//        else
//            removeFilter("height");
//        break;
//    case CV_CAP_PROP_FPS:
//        if(value > 0) {
//            int num, denom;
//            num = (int) value;
//            if(value != num) { // FIXME this supports only fractions x/1 and x/2
//                num = (int) (value * 2);
//                denom = 2;
//            } else
//                denom = 1;

//            setFilter("framerate", GST_TYPE_FRACTION, num, denom);
//        } else
//            removeFilter("framerate");
//        break;
//    case CV_CAP_PROP_FOURCC:
//    case CV_CAP_PROP_FRAME_COUNT:
//    case CV_CAP_PROP_FORMAT:
//    case CV_CAP_PROP_MODE:
//    case CV_CAP_PROP_BRIGHTNESS:
//    case CV_CAP_PROP_CONTRAST:
//    case CV_CAP_PROP_SATURATION:
//    case CV_CAP_PROP_HUE:
//    case CV_CAP_PROP_GAIN:
//    case CV_CAP_PROP_CONVERT_RGB:
//        break;
//    case CV_CAP_GSTREAMER_QUEUE_LENGTH:
//        if(!sink)
//            break;
//        gst_app_sink_set_max_buffers(GST_APP_SINK(sink), (guint) value);
//        break;
//    default:
//        CV_WARN("GStreamer: unhandled property");
//    }
    return false;
}
}
}
