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

Tracking::Tracking(const std::string &trackerType, cv::VideoCapture &video)
    : trackerType(trackerType), video(video)
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
    cout << "Inside trackerUpdate" << endl;
    bool found = false;

    if (video.read(frame)) {
        cout << "successfully read frame" << endl;
        resize(frame, frame, Size(frameWidth, frameHeight));
        found = tracker->update(frame, bbox);
        if (found)
        {
            cout << "successfully found object within frame" << endl;
            // Tracking success: Draw the tracked object
            Point p1(cvRound(bbox.x), cvRound(bbox.y));                            // Top left corner
            Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height)); // Bottom right corner
            rectangle(frame, p1, p2, Scalar(255, 0, 0), 2, 1);
            cout << "Added bbox to frame" << endl;
        }
        else
        {
            cout << "did not find object in frame" << endl;
            // Tracking failure detected.
            putText(frame, "Tracking failure detected", Point(100, 80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0, 0, 255), 2);
        }

        // Display tracker type on frame
        putText(frame, trackerType + " Tracker", Point(100, 20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50, 170, 50), 2);

    }
    else {
        cout << "Unable to read frame!\n";
    }

    return found;

}
