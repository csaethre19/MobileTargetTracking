

#include <opencv2/opencv.hpp>

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
int frameWidth;
int frameHeight;
Rect bbox;
Ptr<Tracker> tracker;

std::string trackerType;
FrameSize frameSize;
cv::VideoCapture video;

//cv::VideoCapture& video;

};
