#include "Tracking.h"

using namespace cv;
using namespace std;
using namespace legacy;

class LegacyTrackerWrapper : public cv::Tracker
{
    const Ptr<legacy::Tracker> legacy_tracker_;

public:
    LegacyTrackerWrapper(const Ptr<legacy::Tracker> &legacy_tracker)
        : legacy_tracker_(legacy_tracker) {}

    ~LegacyTrackerWrapper() override {}

    void init(InputArray image, const Rect &boundingBox) override
    {
        legacy_tracker_->init(image, static_cast<Rect2d>(boundingBox));
    }

    bool update(InputArray image, Rect &boundingBox) override
    {
        Rect2d boundingBox2d;
        std::cout << "Calling update from wrapper..." << std::endl;
        bool res = legacy_tracker_->update(image, boundingBox2d);
        boundingBox = static_cast<Rect>(boundingBox2d); // Ensure this cast is safe or adjust accordingly

        return res;
    }
};

Ptr<cv::Tracker> upgradeTrackingAPI(const Ptr<legacy::Tracker> &legacy_tracker)
{
    return makePtr<LegacyTrackerWrapper>(legacy_tracker);
}

Ptr<cv::Tracker> getTracker(const string &trackerType)
{
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

Tracking::Tracking(const std::string &trackerType, FrameSize frameSize, cv::VideoCapture &video)
    : trackerType(trackerType), frameSize(frameSize), video(video)
{
    // Creates tracker
    tracker = getTracker(trackerType);
    cout << "Tracker created!" << endl;
}

Mat Tracking::initTracker(cv::Rect& bbox)
{
    Mat frame;
    bool ok = video.read(frame);

    if (!ok)
    {
        cout << "ERROR: Could not read frame" << endl;
        std::exit(EXIT_FAILURE);
    }

    if (frameSize == FrameSize::SMALL)
    {
        frameWidth = frame.cols / 2;
        frameHeight = frame.rows / 2;
    }
    else if (frameSize == FrameSize::MEDIUM)
    {
        frameWidth = 800;
        frameHeight = 600;
    }
    else if (frameSize == FrameSize::LARGE)
    {
        frameWidth = 1600;
        frameHeight = 1200;
    }

    // Resize frame
    resize(frame, frame, Size(frameWidth, frameHeight));

    if (bbox.width >= frameWidth || bbox.height >= frameHeight) {
        cout << "ROI outside of frame bounds\n";
        return Mat(); // return empty frame - bbox outside of frame bounds
    }
    if (bbox.width <= 1 || bbox.height <= 1) {
        cout << "ROI too small\n";
        return Mat(); // return empty frame - too small of bbox
    }

    tracker->init(frame, bbox);
    cout << "Tracker initialized with initial frame and bbox\n";

    return frame;
}

bool Tracking::trackerUpdate(cv::Rect& bbox, cv::Mat& frame)
{
    bool found = false;

    if (video.read(frame)) {
        resize(frame, frame, Size(frameWidth, frameHeight));
        found = tracker->update(frame, bbox);
        if (found)
        {
            // Tracking success: Draw the tracked object
            Point p1(cvRound(bbox.x), cvRound(bbox.y));                            // Top left corner
            Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height)); // Bottom right corner
            rectangle(frame, p1, p2, Scalar(255, 0, 0), 2, 1);
        }
        else
        {
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
        }

        // TODO: clean-up
        // remove the following logic
        // Don't need to display frame on raspi or calculate p1, p2, or center point here - done in main

        // Display tracker type on frame
        putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

        // calculate top-left and bottom-right corners
        Point p1(cvRound(bbox.x), cvRound(bbox.y));                            // Top left corner
        Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height)); // Bottom right corner

        // calculate center of bounding box
        int xc = (p1.x + p2.x) / 2;
        int yc = (p1.y + p2.y) / 2;

        // draw center point - this was for testing purposes
        int radius = 5;
        cv::Scalar color = cv::Scalar(0, 0, 255); 
        cv::circle(frame, cv::Point(xc, yc), radius, color, -1);

        // Display result
        imshow("Tracking", frame);

        // Exit if ESC pressed
        int k = waitKey(1);
        if (k == 27)
        {
            video.release();
            destroyAllWindows();
            
            return 0;
        }
    }
    else {
        cout << "Unable to read frame!\n";
    }

    return found;

}

void Tracking::continuousTracking()
{
    // Read first frame
    cout << "Reading first frame..." << endl;
    Mat frame;
    bool ok = video.read(frame);
    if (!ok)
    {
        cout << "ERROR: Could not read frame" << endl;
        std::exit(EXIT_FAILURE);
    }

    if (frameSize == FrameSize::SMALL)
    {
        frameWidth = frame.cols / 2;
        frameHeight = frame.rows / 2;
    }
    else if (frameSize == FrameSize::MEDIUM)
    {
        frameWidth = 800;
        frameHeight = 600;
    }
    else if (frameSize == FrameSize::LARGE)
    {
        frameWidth = 1600;
        frameHeight = 1200;
    }

    // Resize frame
    resize(frame, frame, Size(frameWidth, frameHeight));

    cout << "Initializing video writer..." << endl;
    // Initialize video writer
    VideoWriter output(trackerType + ".avi", VideoWriter::fourcc('X', 'V', 'I', 'D'), 60.0, Size(frameWidth / 2, frameHeight / 2), true);
    cout << "Selecting ROI manually with frame..." << endl;
    // Select ROI
    Rect bbox = selectROI(frame, false);
    Point p1(cvRound(bbox.x), cvRound(bbox.y));                            // Top left corner
    Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height)); // Bottom right corner
    cout << "BBOX: " << endl;
    cout << "Point p1: (" << p1.x << ", " << p1.y << ")" << endl;
    cout << "Point p2: (" << p2.x << ", " << p2.y << ")" << endl;

    // Initialize tracker with first frame and bounding box
    tracker->init(frame, bbox);
    cout << "Tracker initialized." << endl;
    cout << "Tracking Started..." << endl;

    
    while (video.read(frame))
    {
        // Resize frame
        resize(frame, frame, Size(frameWidth, frameHeight));

        // Start timer
        double timer = (double)getTickCount();
        cout << "Timer started." << endl;

        cout << "Updating tracker..." << endl;
        // Update tracking result
        bool found = tracker->update(frame, bbox);
        cout << "Tracker updated!" << endl;

        cout << "Calculating FPS..." << endl;
        // Calculate Frames per second (FPS)
        float fps = video.get(CAP_PROP_FPS);
        cout << "FPS: " << fps << endl;

        if (found)
        {
            cout << "Tracking Success!" << endl;
            // Tracking success: Draw the tracked object
            Point p1(cvRound(bbox.x), cvRound(bbox.y));                            // Top left corner
            Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height)); // Bottom right corner
            rectangle(frame, p1, p2, Scalar(255, 0, 0), 2, 1);
        }
        else
        {
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
        }

        // Display tracker type on frame
        putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

        // Display FPS on frame
        putText(frame, "FPS : " + to_string((int)fps), Point(100, 50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

        // Display result
        imshow("Tracking", frame);
        output.write(frame);

        // Exit if ESC pressed
        int k = waitKey(1);
        if (k == 27)
        {
            break;
        }
    }
    

    cout << "Ending tracking" << endl;

    video.release();
    output.release();

    destroyAllWindows();
    
}