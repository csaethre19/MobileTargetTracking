#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
    // List of tracker types
    string trackerTypes[8] = {"BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
    string trackerType = trackerTypes[1]; 

    // if (trackerType == "BOOSTING")
    //     tracker = TrackerBoosting::create();
    // if (trackerType == "MIL")
    //     tracker = TrackerMIL::create();
    // if (trackerType == "KCF")
    //     tracker = TrackerKCF::create();
    // if (trackerType == "TLD")
    //     tracker = TrackerTLD::create();
    // if (trackerType == "MEDIANFLOW")
    //     tracker = TrackerMedianFlow::create();
    // if (trackerType == "GOTURN")
    //     tracker = TrackerGOTURN::create();
    // if (trackerType == "MOSSE")
    //     tracker = TrackerMOSSE::create();
    // if (trackerType == "CSRT")
    //     tracker = TrackerCSRT::create();

    // Create a tracker
    cout << "Creating tracker..." << endl;
    Ptr<Tracker> tracker;

    // MIL is the only tracker object that works...
    tracker = TrackerMIL::create();
    cout << "MIL Tracker created!" << endl;

    // Open video file
    cout << "Opening video file..." << endl;
    VideoCapture video("src/walking.mp4");
    if (!video.isOpened()) {
        cout << "Could not read video file" << endl;
        return 1;
    }

    cout << "Successfully opened video!" << endl;

    // Read first frame
    cout << "Reading first frame..." <<  endl;
    Mat frame;
    bool ok = video.read(frame);
    if (!ok) {
        cout << "Cannot read video file" << endl;
        return 1;
    }

    cout << "Resizing frame..." <<  endl;
    // Frame dimensions
    int frame_width = frame.cols;
    int frame_height = frame.rows;
    // Resize frame
    resize(frame, frame, Size(frame_width / 2, frame_height / 2));

    cout << "Initializing video writer..." << endl;
    // Initialize video writer
    VideoWriter output(trackerType + ".avi", VideoWriter::fourcc('X','V','I','D'), 60.0, Size(frame_width / 2, frame_height / 2), true);

    cout << "Selecting ROI manually with frame..." << endl;
    // Select ROI
    Rect bbox = selectROI(frame, false);
    // Initialize tracker with first frame and bounding box
    tracker->init(frame, bbox);
    cout << "Tracker initialized." << endl;

    cout << "Tracking Started..." << endl;
    while(video.read(frame)) {
        // Resize frame
        resize(frame, frame, Size(frame_width / 2, frame_height / 2));

        // Start timer
        double timer = (double)getTickCount();
        cout << "Timer started." << endl;

        cout << "Updating tracker..." << endl;
        // Update tracking result
        bool found = tracker->update(frame, bbox);
        cout << "Tracker updated!" << endl;

        cout << "Calculating FPS..." << endl;
        // Calculate Frames per second (FPS)
        float fps = getTickFrequency() / ((double)getTickCount() - timer);
        cout << "FPS: " << fps << endl;

        if (found) {
            cout << "Tracking Success!" << endl;
            // Tracking success: Draw the tracked object
            Point p1(cvRound(bbox.x), cvRound(bbox.y)); // Top left corner
            Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height)); // Bottom right corner
            rectangle(frame, p1, p2, Scalar(255, 0, 0), 2, 1);
        } else {
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
        }

        // Display tracker type on frame
        putText(frame, trackerType + " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);

        // Display FPS on frame
        putText(frame, "FPS : " + to_string((int)fps), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);

        // Display result
        imshow("Tracking", frame);
        output.write(frame);

        // Exit if ESC pressed
        int k = waitKey(1);
        if(k == 27) {
            break;
        }
    }

    video.release();
    output.release();
    destroyAllWindows();
    return 0;
}
