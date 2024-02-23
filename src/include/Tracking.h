#ifndef TRACKER_FUNCTIONS_H
#define TRACKER_FUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Enum to specify frame size
enum FrameSize { SMALL, MEDIUM, LARGE };

class Tracking {
public:

Tracking(const std::string& trackerType, FrameSize frameSize, cv::VideoCapture& video);

// Function declarations
Mat initializeTracking();
void continuousTracking(Mat frame);

private:
// Global variables for tracking size of frame
int frame_width;
int frame_height;
Rect bbox;
Ptr<Tracker> tracker;
//cv::VideoCapture& video;

}
#endif
