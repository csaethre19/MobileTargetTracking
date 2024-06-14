#include <raspicam/raspicam_cv.h>
#include <iostream>
#include "Camera.h"

using namespace std;
using namespace cv;

raspicam::RaspiCam_Cv Camera::selectVideo() {
    raspicam::RaspiCam_Cv Camera;
    Camera.set(cv::CAP_PROP_FORMAT, CV_8UC3); // Use 8-bit color
    Camera.set(cv::CAP_PROP_FRAME_WIDTH, 1280);  // Set width
    Camera.set(cv::CAP_PROP_FRAME_HEIGHT, 720);  // Set height

    if (!Camera.open()) {
        cerr << "Error opening camera" << endl;
        exit(1);
    }

    return Camera;
}
