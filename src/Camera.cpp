#include <opencv2/opencv.hpp> 
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>
#include "Camera.h"

using namespace cv;
using namespace std;


Camera::Camera()
{
    
}

/*
Allows user to select webcam or video file - this will be replaced with
Camera.cpp
*/
VideoCapture Camera::selectVideo(string videoPath)
{
    VideoCapture video;

    if (videoPath == "")
    {
        // Open webcam
        video = VideoCapture(0);
        // Check if opened
        if (!video.isOpened())
        {
            cout << "ERROR: Could not open webcam" << endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {

        video = VideoCapture(videoPath);
        // check if opened
        if(!video.isOpened())
        {
            cout << "ERROR: Could not read video file" << endl;
            exit(EXIT_FAILURE);
        }
        cout << "Success!" << endl;
    }

    return video;
}

int Camera::read(cv::Mat frame)
{
     //Open camera 
    cout<<"Opening Camera..."<<endl;
    if ( !Camera.open()) {cerr<<"Error opening camera"<<endl;return 0;}
    //wait a while until camera stabilizes
    cout<<"Sleeping for 3 secs"<<endl;
    for(int i = 0; i < 1000000000; i++){
        
    }
    //capture
    Camera.grab();
    //allocate memory
    unsigned char* data=new unsigned char[  Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB )];
    //extract the image in rgb format
    Camera.retrieve ( data,raspicam::RASPICAM_FORMAT_RGB );//get camera image
    //save
    std::ofstream outFile ( "raspicam_image2.jpg",std::ios::binary );
    outFile<<"P6\n"<<Camera.getWidth() <<" "<<Camera.getHeight() <<" 255\n";
    outFile.write ( ( char* ) data, Camera.getImageTypeSize ( raspicam::RASPICAM_FORMAT_RGB ) );
    cout<<"Image saved at raspicam_image.ppm"<<endl;
    //free resrources
    delete data;
    
    return 1;   
    
}
