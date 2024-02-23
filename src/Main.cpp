#include "Tracking.h"
#include "Camera.h"
#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>

/*
Function to convert string to FrameSize enum
*/
FrameSize stringToFrameSize(const std::string& sizeStr) {
    static const std::map<std::string, FrameSize> frameSizeMap = {
        {"small", FrameSize::SMALL},
        {"medium", FrameSize::MEDIUM},
        {"large", FrameSize::LARGE}
    };

    auto it = frameSizeMap.find(sizeStr);
    if (it != frameSizeMap.end()) {
        return it->second;
    } else {
        // Default to a specific size or handle error as needed
        return FrameSize::MEDIUM;
    }
}



int main() {
    
    Camera cam;

    cv::VideoCapture video = cam.select_Video();

    std::string trackerType;
    std::cout << "PLease select tracker type:  ";
    std::getline(std::cin, trackerType);
    
    std::string frameSize;
    std::cout << "PLease select tracker type:  ";
    std::getline(std::cin, frameSize);
    
    FrameSize size = stringToFrameSize(frameSize);

    Tracking tracker(trackerType, size, video);

    Mat frame = tracker.initializeTracking();

    tracker.continuousTracking(frame);

    return 0;
}