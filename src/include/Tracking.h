#ifndef TRACKER_FUNCTIONS_H
#define TRACKER_FUNCTIONS_H

#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Global variables for tracking size of frame
extern int frame_width;
extern int frame_height;
extern Rect bbox;

// Function declarations
void create_Tracker();
void select_Video();
Mat initialize_Tracking();
void continuous_Tracking(Mat frame);

#endif
