#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <string>
#include "Camera.h"

using namespace cv;
using namespace std;


/*
Allows user to select webcam or video file - this will be replaced with
Camera.cpp
*/
cv::VideoCapture Camera::selectVideo(string videoPath)
{
    cv::VideoCapture video;

    if (videoPath == "")
    {
        // Open webcam
        video = cv::VideoCapture(0);
        // Check if opened
        if (!video.isOpened())
        {
            cout << "ERROR: Could not open webcam" << endl;
            std::exit(EXIT_FAILURE);
        }
    }
    else
    {
        video = cv::VideoCapture(videoPath);
        // check if opened
        if (!video.isOpened())
        {
            cout << "ERROR: Could not read video file" << endl;
            std::exit(EXIT_FAILURE);
        }
        cout << "Success!" << endl;
    }

    return video;
}
