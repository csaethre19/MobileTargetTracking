#include <Tracking.h>
#include <Camera.h>
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>

int main() {
    
    create_Tracker();

// Should be replaced by Camera.cpp function
    select_Video();

    Mat frame = initialize_Tracking();

    continuous_Tracking();

    return 0;
}