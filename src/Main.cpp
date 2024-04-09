#include "Tracking.h"
#include "Camera.h"

using namespace cv;

int main() {
    
    Camera cam;
    cv::VideoCapture video = cam.selectVideo("src/long_snowboard_drone.mp4");
    Tracking tracker("MOSSE", MEDIUM, video);
    tracker.continuousTracking();

    return 0;
}