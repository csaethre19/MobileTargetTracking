#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


class Camera{
    public:

    Camera();
    
    cv::VideoCapture selectVideo(string videoPath="");
    
    bool read(cv::Mat frame);
    
    raspicam::RaspiCam Camera; 

};

