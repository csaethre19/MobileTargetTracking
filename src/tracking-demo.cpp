#include <opencv2/core.hpp> // Core functionalities
#include <opencv2/highgui.hpp> // High-level GUI
#include <opencv2/imgproc.hpp> // Image processing
#include <opencv2/tracking.hpp> // Tracking API
#include <opencv2/tracking/tracking_legacy.hpp> // Legacy tracking API
#include <iostream>

using namespace cv;
using namespace std;
using namespace legacy;

class LegacyTrackerWrapper : public cv::Tracker {
    const Ptr<legacy::Tracker> legacy_tracker_;

public:
    LegacyTrackerWrapper(const Ptr<legacy::Tracker>& legacy_tracker)
        : legacy_tracker_(legacy_tracker) {}

    ~LegacyTrackerWrapper() override {}

    void init(InputArray image, const Rect& boundingBox) override {
        legacy_tracker_->init(image, static_cast<Rect2d>(boundingBox));
    }

    bool update(InputArray image, Rect& boundingBox) override {
        Rect2d boundingBox2d;
        bool res = legacy_tracker_->update(image, boundingBox2d);
        boundingBox = static_cast<Rect>(boundingBox2d);  // Ensure this cast is safe or adjust accordingly
        return res;
    }
};

Ptr<cv::Tracker> upgradeTrackingAPI(const Ptr<legacy::Tracker>& legacy_tracker) {
    return makePtr<LegacyTrackerWrapper>(legacy_tracker);
}

Ptr<cv::Tracker> get_tracker(const string& trackerType) {
    if (trackerType == "CSRT")
        return legacy::upgradeTrackingAPI(legacy::TrackerCSRT::create());
    if (trackerType == "BOOSTING")
        return legacy::upgradeTrackingAPI(legacy::TrackerBoosting::create());
    if (trackerType == "KCF")
        return legacy::upgradeTrackingAPI(legacy::TrackerKCF::create());
    if (trackerType == "MedianFlow")
        return legacy::upgradeTrackingAPI(legacy::TrackerMedianFlow::create());
    if (trackerType == "TLD")
        return legacy::upgradeTrackingAPI(legacy::TrackerTLD::create());
    if (trackerType == "MOSSE")
        return legacy::upgradeTrackingAPI(legacy::TrackerMOSSE::create());
    if (trackerType == "MIL")
        return legacy::upgradeTrackingAPI(legacy::TrackerMIL::create());
    throw std::runtime_error("Unsupported tracker type");

}


int main() {
    string trackerType = "TLD"; 
    // Create a tracker
    cout << "Creating tracker..." << endl;
    Ptr<cv::Tracker> tracker = get_tracker(trackerType);
    cout << trackerType << " Tracker created!" << endl;

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
