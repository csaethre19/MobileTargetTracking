#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>
#include "Camera.h"

using namespace cv;
using namespace std;

/*
Allows user to select webcam or video file - this will be replaced with
Camera.cpp
*/
cv::VideoCapture Camera::selectVideo(){
    std::string userInput;
    std::cout << "Are you using webcam or video file: ";
    std::getline(std::cin, userInput);

    cv::VideoCapture video;

    if(userInput == "webcam"){
        // Open webcam
         video = cv::VideoCapture(0);
        // Check if opened
        if (!video.isOpened()) {
            cout << "ERROR: Could not open webcam" << endl;
            std::exit(EXIT_FAILURE);
        }
    } else if((userInput == "video file") ||(userInput == "file")){
        // Get the file from the user
        std::string file;
        std::cout << "What video file do you want to open? ";
        std::getline(std::cin, file);
        // Open video file
        cout << "Opening video file..." << endl;
         video = cv::VideoCapture(file);
        //check if opened
        if (!video.isOpened()) {
            cout << "ERROR: Could not read video file" << endl;
            std::exit(EXIT_FAILURE);
    }
    } else{
        // Input doesn't match either option
        cout << "Error: Incorrect input" << endl;
        std::exit(EXIT_FAILURE);
    }
    cout << "Success!" << endl;
    return video;
}
