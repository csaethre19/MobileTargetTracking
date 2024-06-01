#include <opencv2/opencv.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <sstream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>


#include "Camera.h"

//Test of writing raspi camera data ot the frame buffer of a headless mode raspi
int main() {
    //-------------Setup Camera--------------------
    Camera cam;
    string videoPath = "";
    // Run application without path argument to default to camera module
    cv::VideoCapture video = cam.selectVideo(videoPath);

    int fbfd = 0;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize = 0;
    char *fbp = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
        return(1);
    }
    printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
     } 

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
    }
     printf("%dx%d, %d bpp\n", vinfo.xres, vinfo.yres, 
            vinfo.bits_per_pixel );

    // map framebuffer to user memory 
    screensize = finfo.smem_len;

    fbp = (char*)mmap(0, 
                        screensize, 
                        PROT_READ | PROT_WRITE, 
                        MAP_SHARED, 
                        fbfd, 0);

    // TODO: start transmitFrameThread
    // Create thread function that spins off and continuously 
    // calls video.read(frame) and transmits via frame buffer:
    // Mat frame;
    // while (video.read(frame)) 
    // { 
    //     // transmit frame 


    // }

    return 0;
}
