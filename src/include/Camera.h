#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>
#include "spdlog/spdlog.h"

using namespace cv;
using namespace std;


class Camera{
    public:
    Camera(std::shared_ptr<spdlog::logger> logger);

    cv::VideoCapture selectVideo(string videoPath="");
    
    private:
        std::shared_ptr<spdlog::logger> logger;

};

#endif

