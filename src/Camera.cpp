#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <string>
#include "Camera.h"

using namespace cv;
using namespace std;


Camera::Camera(std::shared_ptr<spdlog::logger> logger) : logger(logger) {}

/*
Allows user to select webcam or video file - this will be replaced with
Camera.cpp
*/
cv::VideoCapture Camera::selectVideo(string videoPath)
{
    cv::VideoCapture video;

    if (videoPath == "")
    {
        // Open default camera port
        video = cv::VideoCapture(0, cv::CAP_V4L2);
        // Check if opened
        if (!video.isOpened())
        {
            logger->debug("ERROR: Could not open default camera port");
            std::exit(EXIT_FAILURE);
        }
        logger->info("Camera accessed.");
    }
    else
    {
        video = cv::VideoCapture(videoPath);
        // check if opened
        if (!video.isOpened())
        {
            logger->debug("ERROR: Could not read video file");
            std::exit(EXIT_FAILURE);
        }
        logger->info("Video accessed.");
    }

    video.set(CAP_PROP_FRAME_WIDTH, 1280);  // Use a lower resolution if 1920x1080 is too high
    video.set(CAP_PROP_FRAME_HEIGHT, 720);  // Use a lower resolution if 1920x1080 is too high
    video.set(CAP_PROP_FPS, 30);

    int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
    video.set(CAP_PROP_FOURCC, fourcc);

    // Print camera properties for debugging
    // cout << "Frame width: " << video.get(CAP_PROP_FRAME_WIDTH) << endl;
    // cout << "Frame height: " << video.get(CAP_PROP_FRAME_HEIGHT) << endl;
    // cout << "Frame format: " << video.get(CAP_PROP_FORMAT) << endl;
    // cout << "Frame rate: " << video.get(CAP_PROP_FPS) << endl;
    logger->info("Frame width: ");
    logger->info("Frame height: ");
    logger->info("Frame format: ");
    logger->info("Frame rate: ");

    return video;
}
