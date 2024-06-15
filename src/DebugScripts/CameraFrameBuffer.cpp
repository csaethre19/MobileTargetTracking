#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include <string>
#include "Camera.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

using namespace cv;
using namespace std;


int main(int argc, char* argv[]) {
    
    Camera cam;
    string videoPath = "";
    if (argc > 1) videoPath = argv[1];
    cv::VideoCapture video = cam.selectVideo();
    
    int fbfd = 0; // framebuffer filedescriptor
  struct fb_var_screeninfo var_info;

  // Open the framebuffer device file for reading and writing
  fbfd = open("/dev/fb0", O_RDWR);
  if (fbfd == -1) {
    printf("Error: cannot open framebuffer device.\n");
    return(1);
  }
  printf("The framebuffer device opened.\n");

  // Get variable screen information
  //if (ioctl(fbfd, FBIOGET_VSCREENINFO, &var_info)) {
  //  printf("Error reading variable screen info.\n");
  //}
  printf("Display info %dx%d, %d bpp\n", 
         var_info.xres, var_info.yres, 
         var_info.bits_per_pixel );

  // close file  
  close(fbfd);
    
    return 0;
}

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
