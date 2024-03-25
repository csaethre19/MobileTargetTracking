#include "Tracking.h"
#include "Camera.h"

using namespace cv;

int main(int argc, char* argv[]) {
    
    Camera cam;
    string videoPath = "";
    if (argc > 1) videoPath = argv[1];
    cv::VideoCapture video = cam.selectVideo(videoPath);
    Tracking tracker("MOSSE", MEDIUM, video);
    tracker.continuousTracking();

    return 0;
}
