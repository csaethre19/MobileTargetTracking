#ifndef VIDEOTRANSMITTER_H
#define VIDEOTRANSMITTER_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "Camera.h"


int openFrameBuffer();

class VideoTransmitter{
    public:
    VideoTransmitter(cv::VideoCapture& video);

    ~VideoTransmitter();

    int transmitFrame(cv::Mat frame);
    void displayFramebufferInfo();

    private:
    cv::VideoCapture video;

    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    int fbfd = 0;
    char *fbp = 0;
    long int screensize = 0;

};

#endif