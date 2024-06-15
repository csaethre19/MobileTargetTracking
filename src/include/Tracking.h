#ifndef TRACKING_H
#define TRACKING_H

#include <opencv2/core.hpp> // Core functionalities
#include <opencv2/highgui.hpp> // High-level GUI
#include <opencv2/imgproc.hpp> // Image processing
#include <opencv2/tracking.hpp> // Tracking API
#include <opencv2/tracking/tracking_legacy.hpp> // Legacy tracking API
#include <iostream>

using namespace cv;
using namespace std;
using namespace legacy;


class Tracking {
    public:

    Tracking(const std::string& trackerType, cv::VideoCapture& video);

    /*
        Initializes tracker based on specified bounding box parameter (expected from UI via swarm-dongle).
        Returns Mat object of initial frame used to initialize tracker.
    */
    Mat initTracker(cv::Rect& bbox);

    /*
        Calls internal tracker update method based on specified bounding box and frame parameters (referenced objects).
        Returns true if tracking was success.
        Returns false if tracking failed or was unable to read next frame (via video.read()). 
    */
    bool trackerUpdate(cv::Rect& bbox, cv::Mat& frame);

    private:

    // Resolution NTSC expects for transmitting video
    int frameWidth = 720;
    int frameHeight = 480;

    Ptr<cv::Tracker> tracker;
    std::string trackerType;
    cv::VideoCapture video;

};

#endif
