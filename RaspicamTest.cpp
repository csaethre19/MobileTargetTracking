#include <raspicam/raspicam_cv.h>
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
    raspicam::RaspiCam_Cv Camera;
    Camera.set(cv::CAP_PROP_FORMAT, CV_8UC3); // Use 8-bit color
    Camera.set(cv::CAP_PROP_FRAME_WIDTH, 1280);  // Set width
    Camera.set(cv::CAP_PROP_FRAME_HEIGHT, 720);  // Set height

    if (!Camera.open()) {
        cerr << "Error opening camera" << endl;
        return -1;
    } else {
        cout << "Camera opened successfully" << endl;
    }

    cv::Mat frame;
    for (int i = 0; i < 10; ++i) {
        cout << "Grabbing frame " << i + 1 << endl;
        if (!Camera.grab()) {
            cerr << "Error grabbing frame " << i + 1 << endl;
            continue;
        }
        cout << "Frame grabbed successfully" << endl;

        Camera.retrieve(frame);
        if (frame.empty()) {
            cerr << "Error retrieving frame " << i + 1 << endl;
            continue;
        }

        cout << "Frame retrieved successfully" << endl;
        string filename = "frame_" + to_string(i + 1) + ".jpg";
        cv::imwrite(filename, frame);
        cout << "Frame saved as " << filename << endl;
    }

    Camera.release();
    return 0;
}
