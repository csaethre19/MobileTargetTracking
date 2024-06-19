#include "Camera.h"


Camera::Camera(std::shared_ptr<spdlog::logger> logger) : logger(logger) {}

/*
    Allows user to select webcam or video file.
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
            logger->error("ERROR: Could not open default camera port");
            std::exit(EXIT_FAILURE);
        }
        logger->debug("Camera accessed.");
    }
    else
    {
        video = cv::VideoCapture(videoPath);
        // check if opened
        if (!video.isOpened())
        {
            logger->error("ERROR: Could not read video file");
            std::exit(EXIT_FAILURE);
        }
        logger->debug("Video accessed.");
    }

    video.set(CAP_PROP_FRAME_WIDTH, 1280);  // Use a lower resolution if 1920x1080 is too high
    video.set(CAP_PROP_FRAME_HEIGHT, 720);  // Use a lower resolution if 1920x1080 is too high
    video.set(CAP_PROP_FPS, 30);

    int fourcc = VideoWriter::fourcc('M', 'J', 'P', 'G');
    video.set(CAP_PROP_FOURCC, fourcc);

    logger->info("Frame width: {}", video.get(CAP_PROP_FRAME_WIDTH));
    logger->info("Frame height: {}", video.get(CAP_PROP_FRAME_HEIGHT));
    logger->info("Frame format: {}", video.get(CAP_PROP_FORMAT));
    logger->info("Frame rate: {}", video.get(CAP_PROP_FPS));

    return video;
}
