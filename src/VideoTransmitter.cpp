#include "VideoTransmitter.h"

VideoTransmitter::VideoTransmitter(std::shared_ptr<spdlog::logger> logger) : logger(logger)
{
    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (!fbfd) {
        printf("Error: cannot open framebuffer device.\n");
    }
    // printf("The framebuffer device was opened successfully.\n");

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
        printf("Error reading fixed information.\n");
    } 

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
        printf("Error reading variable information.\n");
    }
    // printf("%dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    // printf("Line length: %d bytes\n", finfo.line_length);

    // Calculate the screensize
    screensize = finfo.line_length * vinfo.yres;
    // printf("Screensize: %ld bytes\n", screensize);

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
    // printf("read frame successfully\n");

    cv::cvtColor(frame, frame, cv::COLOR_BGR2BGRA);

    // Resize the frame if necessary to fit the framebuffer
    resize(frame, frame, Size(vinfo.xres, vinfo.yres), 0, 0, INTER_NEAREST);
    // printf("resized frame\n");

    // Copy the frame data to the framebuffer
    for (int y = 0; y < vinfo.yres; y++) {
        memcpy(fbp + y * finfo.line_length, frame.ptr(y), vinfo.xres * 4);
    }
    // printf("copied frame data to framebuffer\n");

    return 0;
}

void VideoTransmitter::displayFramebufferInfo() {
    printf("Fixed screen info:\n");
    printf("  id: %s\n", finfo.id);
    printf("  smem_start: %lx\n", finfo.smem_start);
    printf("  smem_len: %d\n", finfo.smem_len);
    printf("  type: %d\n", finfo.type);
    printf("  type_aux: %d\n", finfo.type_aux);
    printf("  visual: %d\n", finfo.visual);
    printf("  xpanstep: %d\n", finfo.xpanstep);
    printf("  ypanstep: %d\n", finfo.ypanstep);
    printf("  ywrapstep: %d\n", finfo.ywrapstep);
    printf("  line_length: %d\n", finfo.line_length);
    printf("  mmio_start: %lx\n", finfo.mmio_start);
    printf("  mmio_len: %d\n", finfo.mmio_len);
    printf("  accel: %d\n", finfo.accel);

    printf("Variable screen info:\n");
    printf("  xres: %d\n", vinfo.xres);
    printf("  yres: %d\n", vinfo.yres);
    printf("  xres_virtual: %d\n", vinfo.xres_virtual);
    printf("  yres_virtual: %d\n", vinfo.yres_virtual);
    printf("  xoffset: %d\n", vinfo.xoffset);
    printf("  yoffset: %d\n", vinfo.yoffset);
    printf("  bits_per_pixel: %d\n", vinfo.bits_per_pixel);
    printf("  grayscale: %d\n", vinfo.grayscale);
    printf("  red: offset: %d, length: %d, msb_right: %d\n", vinfo.red.offset, vinfo.red.length, vinfo.red.msb_right);
    printf("  green: offset: %d, length: %d, msb_right: %d\n", vinfo.green.offset, vinfo.green.length, vinfo.green.msb_right);
    printf("  blue: offset: %d, length: %d, msb_right: %d\n", vinfo.blue.offset, vinfo.blue.length, vinfo.blue.msb_right);
    printf("  transp: offset: %d, length: %d, msb_right: %d\n", vinfo.transp.offset, vinfo.transp.length, vinfo.transp.msb_right);
    printf("  nonstd: %d\n", vinfo.nonstd);
    printf("  activate: %d\n", vinfo.activate);
    printf("  height: %d mm\n", vinfo.height);
    printf("  width: %d mm\n", vinfo.width);
    printf("  accel_flags: %d\n", vinfo.accel_flags);
    printf("  pixclock: %d\n", vinfo.pixclock);
    printf("  left_margin: %d\n", vinfo.left_margin);
    printf("  right_margin: %d\n", vinfo.right_margin);
    printf("  upper_margin: %d\n", vinfo.upper_margin);
    printf("  lower_margin: %d\n", vinfo.lower_margin);
    printf("  hsync_len: %d\n", vinfo.hsync_len);
    printf("  vsync_len: %d\n", vinfo.vsync_len);
    printf("  sync: %d\n", vinfo.sync);
    printf("  vmode: %d\n", vinfo.vmode);
}
