#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::cout << "Starting WEBCAM Demo..." << std::endl;
    // Step 1: Create a VideoCapture object to capture video from the first webcam
    cv::VideoCapture cap(0); // The parameter 0 indicates the default webcam

    // Check if the webcam is opened successfully
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open the webcam" << std::endl;
        return -1;
    }

    // Step 2: Capture and display video
    cv::Mat frame;
    while (true) {
        // Read a new frame from the video capture
        cap >> frame;

        // Check if the frame is empty (e.g., if the webcam is disconnected)
        if (frame.empty()) {
            std::cerr << "Error: Empty frame grabbed" << std::endl;
            break;
        }

        // Display the frame
        cv::imshow("Webcam Feed", frame);

        // Break the loop when 'ESC' is pressed
        if (cv::waitKey(10) == 27) {
            break;
        }
    }

    // Step 3: Release the video capture and close any open windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
