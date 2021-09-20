#ifndef MMGSTREAMER_H
#define MMGSTREAMER_H
namespace mmLib {
namespace mmVideo{
class mmGstreamer
{
public:
    mmGstreamer(const std::string &uri, bool ffmpeg=false, bool realtime=true, bool sync=true, bool qtOutput=false, bool signal_emitter=false);
    ~mmGstreamer() { close(); }
    bool play();
    bool pause();
    bool stop();
    bool seek(double position,bool position_in_frame=false);
    double getActualPostion(bool in_frame=false);
    double getStreamDuration(bool in_frame=false);
    cv::Mat getFrame();
    int test;
    typedef boost::signals2::signal<void(cv::Mat)> signalT;
    boost::signals2::connection newOpenCVFrameEvent(const signalT::slot_type &subscriber)
    {
        return _newOpenCVFrameSignal.connect(subscriber);
    }
    bool isEndOfStreamReached();

protected:
    void init();
    bool open( int type, const char* filename );
    bool grabFrame();
    bool setState(GstState state);
    void close();
    double getProperty(int);
    bool setProperty(int, double);
    IplImage* retrieveFrame(int);
    bool reopen();

    void handleMessage();
    void internalGrabber();
    void processNewOpenCvSignal();
    void restartPipeline();
    void setFilter(const char*, int, int, int);
    void removeFilter(const char *filter);
    static void on_new_buffer(GstElement * elt, void * data);
    void static newPad(GstElement *myelement,
                                GstPad     *pad,
                                gpointer    data);
    GstElement *pipeline;
    GstElement *uridecodebin;
    GstElement *color;
    GstElement *sink;
    GstBuffer  *buffer;
    GstCaps    *caps;
    IplImage   *frame;
    cv::VideoCapture *cap;
    double _lastAccessTime;
    double _lastFrameTimeStamp;
    double _timeStampOffset;
    bool _realtime;
    bool _sync;
    bool _qtOoutput;
    bool _successOpen;
    bool _ffmpeg;
    std::string _uri;
    bool _signal_emitter;
    bool _firstTimeAccess;
    bool _isPlaying;
    cv::Mat _lastFrame;
    int _frameNumber;
    int _lastGetFrameNumber;
    bool _streamError;
    bool _threadToInit;
    bool _signalLocked;
    bool _resuming;
    bool _killThread;
    boost::mutex *_threadMutex;
    boost::shared_ptr<boost::thread> _frameThread;
    signalT _newOpenCVFrameSignal;
    bool m_bEndOfStreamReached;
};
}
}
#endif // MMGSTREAMER_H
