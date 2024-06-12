#include "VideoTransmitter.h"


VideoTransmitter::VideoTransmitter(cv::VideoCapture& video) : video(video)
{
    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
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
    }
}

VideoTransmitter::~VideoTransmitter()
{
    munmap(fbp, screensize);
    close(fbfd);
}

int VideoTransmitter::transmitFrame(cv::Mat frame)
{
    if (frame.empty()) {
        printf("Error: cannot read image.\n");
        munmap(fbp, screensize);
        close(fbfd);
        return(1);
    }
    printf("read frame successfully\n");

    cv::cvtColor(frame, frame, cv::COLOR_BGR2BGRA);

    // Resize the frame if necessary to fit the framebuffer
    resize(frame, frame, Size(vinfo.xres, vinfo.yres));
    printf("resized frame\n");

    // Copy the frame data to the framebuffer
    for (int y = 0; y < vinfo.yres; y++) {
        memcpy(fbp + y * finfo.line_length, frame.ptr(y), vinfo.xres * 2);
    }
    printf("copied frame data to framebuffer\n");

    // Add delay if needed to control frame rate
    // usleep(10000); // Adjust the delay as necessary

    return 0;
}
