#include <opencv2/opencv.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <iostream>

using namespace cv;
using namespace std;

// Global variables for tracking size of frame
int frame_width;
int frame_height;
// Global variable for the tracking box
Rect bbox;

/*
Function creates a tracker based on user's input. There are 8 possible choices.
If one is not selected print out error message.
*/
void create_Tracker(){
    // List of tracker types
    string trackerTypes[8] = {"BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW", "GOTURN", "MOSSE", "CSRT"};
    string trackerType = trackerTypes[1]; 
    Ptr<Tracker> tracker;

    // User selects tracker wanted
    std::string userInput;
    std::cout << "Please select a tracker type: ";
    std::getline(std::cin, userInput);
    std::cout << "Selected tracker: " << userInput << std::endl;
    // Based on user's input create the selected tracker
    cout << "Creating tracker..." << endl;
    if (userInput == "BOOSTING")
        tracker = TrackerBoosting::create();
    else if (userInput == "MIL")
        tracker = TrackerMIL::create();
    else if (userInput == "KCF")
        tracker = TrackerKCF::create();
    else if (userInput == "TLD")
        tracker = TrackerTLD::create();
    else if (userInput == "MEDIANFLOW")
        tracker = TrackerMedianFlow::create();
    else if (userInput == "GOTURN")
        tracker = TrackerGOTURN::create();
    else if (userInput == "MOSSE")
        tracker = TrackerMOSSE::create();
    else if (userInput == "CSRT")
        tracker = TrackerCSRT::create();
    else{
        cout << "ERROR: Invalid tracker type" << endl;
        std::exit(EXIT_FAILURE);
    }

    cout << "Tracker created!" << endl;
    return;
}


/*
Initializes tracking. Gets user input on frame size and
has user select the target to be tracked.
*/
Mat initialize_Tracking(){
        // Read first frame
    cout << "Reading first frame..." <<  endl;
    Mat frame;
    bool ok = video.read(frame);
    if (!ok) {
        cout << "ERROR: Could not read frame" << endl;
        std::exit(EXIT_FAILURE);
    }
    // Allow user to select the size of the frame they want
    cout << "Resizing frame..." <<  endl;
    std::string userInput;
    std::cout << "Please select size for frame: small, medium, large ";
    std::getline(std::cin, userInput);

    if(userInput == "small"){
        // Frame dimensions
        frame_width = frame.cols / 2;
        frame_height = frame.rows / 2;
        // Resize frame
        resize(frame, frame, Size(frame_width, frame_height));
    } else if(userInput == "medium"){
            // Frame dimensions
        int frame_width = 800;
        int frame_height = 600;
        // Resize frame
        resize(frame, frame, Size(frame_width, frame_height));
    } else if(userInput == "large"){
        // Frame dimensions
        int frame_width = 1600;
        int frame_height = 1200;
        // Resize frame
        resize(frame, frame, Size(frame_width, frame_height));
    }else{
        cout << "ERROR: No size selected" << endl;
        std::exit(EXIT_FAILURE);
    }

    cout << "Initializing video writer..." << endl;
    // Initialize video writer
    VideoWriter output(trackerType + ".avi", VideoWriter::fourcc('X','V','I','D'), 60.0, Size(frame_width, frame_height), true);

    cout << "Selecting ROI manually with frame..." << endl;
    // Select ROI
    bbox = selectROI(frame, false);
    // Initialize tracker with first frame and bounding box
    tracker->init(frame, bbox);
    cout << "Tracker initialized." << endl;
    cout << "Tracking Started..." << endl;
    return frame;
}

/*
Performs continuous tracking of user's selected target.
*/
void continuous_Tracking(Mat frame){
        while(video.read(frame)) {
        // Resize frame
        resize(frame, frame, Size(frame_width, frame_height));

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
    cout << "Ending tracking" << endl;
    video.release();
    output.release();
    destroyAllWindows();
    return;
}
