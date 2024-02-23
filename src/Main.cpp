#include "Tracking.h"
#include "Camera.h"

using namespace cv;

int main() {
    
    Camera cam;

    cv::VideoCapture video = cam.selectVideo();

    Tracking tracker("MIL", MEDIUM, video);

    Mat frame = tracker.initializeTracking();

    tracker.continuousTracking(frame);

    return 0;
}