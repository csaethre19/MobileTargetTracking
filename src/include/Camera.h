#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


class Camera{
    public:

    cv::VideoCapture selectVideo(string videoPath="");

};

#endif

