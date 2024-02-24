#include <opencv2/core.hpp> // Core functionalities
#include <opencv2/highgui.hpp> // High-level GUI
#include <opencv2/imgproc.hpp> // Image processing
#include <opencv2/tracking.hpp> // Tracking API
#include <opencv2/tracking/tracking_legacy.hpp> // Legacy tracking API
#include <iostream>

using namespace cv;
using namespace std;
using namespace legacy;

// Enum to specify frame size
enum FrameSize { SMALL, MEDIUM, LARGE };

class Tracking {
    public:

    Tracking(const std::string& trackerType, FrameSize frameSize, cv::VideoCapture& video);

    void continuousTracking();

    private:

    int frameWidth;
    int frameHeight;

    Ptr<cv::Tracker> tracker;
    std::string trackerType;
    FrameSize frameSize;
    cv::VideoCapture video;

};
