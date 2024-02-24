#include "Tracking.h"
#include "Camera.h"

using namespace cv;

int main() {
    
    Camera cam;
    cv::VideoCapture video = cam.selectVideo("src/walking.mp4");
    Tracking tracker("CSRT", MEDIUM, video);
    tracker.continuousTracking();

    return 0;
}