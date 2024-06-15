#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

// This successfully paints the entire screen red!

// To run:
// make Red
// ./Red

using namespace cv;

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

    // Verify the reported resolution
    printf("Resolution: %dx%d, %d bits per pixel\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Calculate the screensize
    screensize = vinfo.yres * vinfo.xres * vinfo.bits_per_pixel / 8;
    printf("Screensize: %ld bytes\n", screensize);

    // Map framebuffer to user memory 
    fbp = (char*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (fbp == MAP_FAILED) {
        printf("Error: failed to map framebuffer device to memory.\n");
        close(fbfd);
        return(1);
    }

    // Fill the framebuffer with a solid color (e.g., red)
    uint32_t red = 0x00FF0000; // Red in 32 bpp
    for (int y = 0; y < vinfo.yres; ++y) {
        for (int x = 0; x < vinfo.xres; ++x) {
            ((uint32_t*)fbp)[y * vinfo.xres + x] = red;
        }
    }

    printf("Solid color pattern displayed.\n");

    // Wait to see the result
    sleep(10);

    // Cleanup
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}
