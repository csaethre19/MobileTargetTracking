#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

// This was supposed to display a smooth gradiant across the entire screen
// What displays is a gradiant like pattern that is not smooth but looks more tiled and is only across half the screen

// To run: 
// make Grad
// ./Grad

using namespace cv;

void displayTestPattern(char* fbp, int width, int height, int line_length) {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            uint16_t r = (x * 31 / width) << 11;
            uint16_t g = (y * 63 / height) << 5;
            uint16_t b = (x * 31 / width);
            uint16_t color = r | g | b;
            uint16_t* pixel_ptr = (uint16_t*)(fbp + y * line_length + x * 2);
            *pixel_ptr = color;

            // Debugging statement
            if (x % 100 == 0 && y % 100 == 0) {
                std::cout << "Writing pixel at (" << x << ", " << y << ") with color " << color << std::endl;
            }
        }
    }
}

int main() {
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

    // Display a test pattern
    displayTestPattern(fbp, vinfo.xres, vinfo.yres, finfo.line_length);
    printf("Displayed test pattern.\n");

    // Wait to see the result
    sleep(10);

    // Cleanup
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
