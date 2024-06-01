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

// Notes:
// Currently this displays the camera frames only on the left side of the screen
// Displays in black and white but pretty clear.
// There is additional code commented out to try and convert from black and white
// to color but is unsuccessful - output is neon colors and looks terrible.

// Updated config.txt to specify framebuffer width and height to be 1920,1080
// Got connected screen resolution with this command: tvservice -s

using namespace cv;

// Function to convert an OpenCV Mat to RGB565 format
void convertToRGB565(const Mat& src, Mat& dst) {
    dst.create(src.rows, src.cols, CV_8UC2); // 2 bytes per pixel
    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            Vec3b pixel = src.at<Vec3b>(y, x);
            uint16_t r = pixel[2] >> 3;
            uint16_t g = pixel[1] >> 2;
            uint16_t b = pixel[0] >> 3;
            uint16_t rgb565 = (r << 11) | (g << 5) | b;
            ((uint16_t*)dst.data)[y * src.cols + x] = rgb565;
        }
    }
}

int main() {
    // Setup Camera
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
    printf("%dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    printf("Line length: %d bytes\n", finfo.line_length);

    // Calculate the screensize
    screensize = finfo.line_length * vinfo.yres;
    printf("Screensize: %ld bytes\n", screensize);

    // Map framebuffer to user memory 
    fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp == MAP_FAILED) {
        printf("Error: failed to map framebuffer device to memory.\n");
        close(fbfd);
        return(1);
    }

    // Create frame buffer
    Mat frame;
    Mat frame_rgb;
    Mat frame_rgb565;

    while (video.read(frame)) { 
        if (frame.empty()) {
            printf("Error: cannot read image.\n");
            munmap(fbp, screensize);
            close(fbfd);
            return(1);
        }
        printf("read frame successfully\n");

        // Resize the frame if necessary to fit the framebuffer
        resize(frame, frame, Size(vinfo.xres, vinfo.yres));
        printf("resized frame\n");

        // This was to get the color from black and white but makes it neon!
        // Convert BGR to RGB
        // cvtColor(frame, frame_rgb, COLOR_BGR2RGB);
        // printf("converted BGR to RGB\n");

        // // Convert to RGB565
        // convertToRGB565(frame_rgb, frame_rgb565);
        // printf("converted to RGB565\n");

        // Copy the frame data to the framebuffer
        for (int y = 0; y < vinfo.yres; y++) {
            memcpy(fbp + y * finfo.line_length, frame.ptr(y), vinfo.xres * 2);
        }
        printf("copied frame data to framebuffer\n");

        // Add delay if needed to control frame rate
        usleep(10000); // Adjust the delay as necessary
    }
    
    // Cleanup
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
