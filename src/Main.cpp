#include "Tracking.h"
#include "Camera.h"

using namespace cv;

int main() {
    
    Camera cam;
    //cv::VideoCapture video = cam.selectVideo("/home/niron/Desktop/MobileTargetTracking/src/test_avi.avi");
    Tracking tracker("CSRT", MEDIUM, cam);
    tracker.continuousTracking();

    return 0;
}
