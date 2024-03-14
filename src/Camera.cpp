#include <opencv2/opencv.hpp> 
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include "Camera.h"

using namespace cv;
using namespace std;


/*
Allows user to select webcam or video file - this will be replaced with
Camera.cpp
*/
VideoCapture Camera::selectVideo(string videoPath)
{
    VideoCapture video;

    if (videoPath == "")
    {
        // Open webcam
        video = VideoCapture(0);
        // Check if opened
        if (!video.isOpened())
        {
            cout << "ERROR: Could not open webcam" << endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {

        video = VideoCapture(videoPath);
        // check if opened
        if(!video.isOpened())
        {
            cout << "ERROR: Could not read video file" << endl;
            exit(EXIT_FAILURE);
        }
        cout << "Success!" << endl;
    }

    return video;
}
