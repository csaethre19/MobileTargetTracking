#include "Tracking.h"
#include "Camera.h"

using namespace cv;

int main() {
    
    Camera cam;
     cv::VideoCapture video = cam.selectVideo("src/walking.mp4");
    //cv::VideoCapture video = cam.selectVideo("");
    Tracking tracker("MOSSE", MEDIUM, video);
    tracker.continuousTracking();

    return 0;
}