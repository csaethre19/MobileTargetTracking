#include <opencv2/opencv.hpp>
#include <raspicam/raspicam_cv.h>
#include <iostream>

using namespace cv;
using namespace std;

void printFrameProperties(const Mat &frame, const string &label) {
    cout << label << " properties:" << endl;
    cout << "  Size: " << frame.size() << endl;
    cout << "  Type: " << frame.type() << endl;
    cout << "  Depth: " << frame.depth() << endl;
    cout << "  Channels: " << frame.channels() << endl;
}

void printPixelValues(const Mat &frame, const string &label, int numRows, int numCols) {
    cout << label << " pixel values:" << endl;
    for (int y = 0; y < numRows; y++) {
        for (int x = 0; x < numCols; x++) {
            Vec3b pixel = frame.at<Vec3b>(y, x);
            cout << "(" << (int)pixel[0] << "," << (int)pixel[1] << "," << (int)pixel[2] << ") ";
        }
        cout << endl;
    }
}

void captureFrameOpenCV(Mat &frame) {
    VideoCapture cap(0); // Open the default camera
    if (!cap.isOpened()) {
        cerr << "Error: Could not open camera using OpenCV" << endl;
        return;
    }
    cap.set(CAP_PROP_FRAME_WIDTH, 1280);
    cap.set(CAP_PROP_FRAME_HEIGHT, 720);
    cap.read(frame);
    if (frame.empty()) {
        cerr << "Error: Frame capture using OpenCV failed" << endl;
    }
}

void captureFrameRaspiCam(Mat &frame) {
    raspicam::RaspiCam_Cv Camera;
    Camera.set(CAP_PROP_FORMAT, CV_8UC3);
    Camera.set(CAP_PROP_FRAME_WIDTH, 1280);
    Camera.set(CAP_PROP_FRAME_HEIGHT, 720);
    if (!Camera.open()) {
        cerr << "Error: Could not open camera using raspicam" << endl;
        return;
    }
    Camera.grab();
    Camera.retrieve(frame);
    if (frame.empty()) {
        cerr << "Error: Frame capture using raspicam failed" << endl;
    }
}

void compareFrames(const Mat &frame1, const Mat &frame2) {
    if (frame1.size() != frame2.size() || frame1.type() != frame2.type()) {
        cout << "Frames have different sizes or types" << endl;
        printFrameProperties(frame1, "OpenCV Frame");
        printFrameProperties(frame2, "RaspiCam Frame");
        return;
    }
    
    bool equal = true;
    for (int y = 0; y < frame1.rows; y++) {
        const uchar* ptr1 = frame1.ptr(y);
        const uchar* ptr2 = frame2.ptr(y);
        for (int x = 0; x < frame1.cols * frame1.elemSize(); x++) {
            if (ptr1[x] != ptr2[x]) {
                equal = false;
                break;
            }
        }
        if (!equal) {
            break;
        }
    }
    if (equal) {
        cout << "Frames are identical" << endl;
    } else {
        cout << "Frames differ" << endl;
        printPixelValues(frame1, "OpenCV Frame", 10, 10);  // Print top 10x10 region
        printPixelValues(frame2, "RaspiCam Frame", 10, 10);  // Print top 10x10 region
    }
}

int main() {
    Mat frameOpenCV, frameRaspiCam;

    captureFrameOpenCV(frameOpenCV);
    captureFrameRaspiCam(frameRaspiCam);

    if (!frameOpenCV.empty()) {
        imwrite("frame_opencv.jpg", frameOpenCV);
    }
    if (!frameRaspiCam.empty()) {
        imwrite("frame_raspicam.jpg", frameRaspiCam);
    }

    compareFrames(frameOpenCV, frameRaspiCam);

    return 0;
}
