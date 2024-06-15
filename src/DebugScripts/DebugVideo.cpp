#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <string>
#include "Camera.h"

using namespace cv;
using namespace std;

int main() {

    cv::VideoCapture video = cv::VideoCapture(0, cv::CAP_V4L2);

    // Set desired camera properties
    video.set(CAP_PROP_FRAME_WIDTH, 1920);
    video.set(CAP_PROP_FRAME_HEIGHT, 1080);
    video.set(CAP_PROP_FPS, 30);

    int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
    video.set(CAP_PROP_FOURCC, fourcc);

    // Print camera properties for debugging
    cout << "Frame width: " << video.get(CAP_PROP_FRAME_WIDTH) << endl;
    cout << "Frame height: " << video.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "Frame format: " << video.get(CAP_PROP_FORMAT) << endl;
    cout << "Frame rate: " << video.get(CAP_PROP_FPS) << endl;
    cout << "Mode: " << video.get(CAP_PROP_MODE) << endl;
    cout << "FourCC: " << video.get(CAP_PROP_FOURCC) << endl;
    cv::Mat frame;

    while (true) {
        video >> frame;
        if (frame.empty()) break;

        // Print frame properties for debugging
        cout << "Captured frame size: " << frame.cols << "x" << frame.rows << endl;
        cout << "Captured frame type: " << frame.type() << endl;

        // Save the raw captured frame for inspection
        cv::imwrite("raw_captured_frame.png", frame);

        // Convert the frame to BGRA
        cv::Mat bgra_frame;
        cv::cvtColor(frame, bgra_frame, cv::COLOR_BGR2BGRA);
        cv::imwrite("bgra_frame.png", bgra_frame);  // Save the converted frame for inspection

        // Resize the frame if necessary to fit the framebuffer
        cv::resize(bgra_frame, bgra_frame, cv::Size(1920, 1080));
        cv::imwrite("resized_bgra_frame.png", bgra_frame);  // Save the resized frame for inspection
        exit(0);
    }

    return 0;
}
