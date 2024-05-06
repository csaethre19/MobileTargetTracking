#include "Camera.h"
#include "UART.h"

using namespace cv;

#include <thread>
#include <mutex>
#include <atomic>
#include <vector>

std::mutex mtx;
std::atomic<bool> continueTracking(true);


void trackingThread(Tracking &tracker, VideoCapture &video, int uart_fd) {
    // Hard coded points for walking video:
    Point p1(448, 261); 
    Point p2(528, 381); 
    int width = p2.x - p1.x;
    int height = p2.y - p1.y;
    Rect bbox(p1.x, p1.y, width, height);
    // Initialize tracking logic...
    Mat frame = tracker.initTracker(bbox);

    while (continueTracking && tracker.trackerUpdate(bbox, frame) != 0) {
        // Continue tracking and sending updates...
    }

    video.release();
    destroyAllWindows();
    
    cout << "Tracking ended.\n";
}

void commandListeningThread(int uart_fd, Tracking &tracker, VideoCapture &video) {
    char ch;
    char buffer[256];
    int cmdBufferPos = 0;

    while (true) {
        if (read(uart_fd, &ch, 1) > 0) {
            if (ch == '\n') {
                std::lock_guard<std::mutex> lock(mtx);
                buffer[cmdBufferPos] = '\0'; // Null-terminate the command string
                if (strncmp(buffer, "track-start", 11) == 0) {
                    cout << "Initiating tracking...\n";
                    continueTracking = true; // Set tracking flag
                    std::thread trackThread(trackingThread, std::ref(tracker), std::ref(video), uart_fd);
                    trackThread.detach(); // Allow the thread to operate independently
                }
                else if (strncmp(buffer, "track-end", 9) == 0) {
                    cout << "Tracking stopping...\n";
                    continueTracking = false; // Clear tracking flag to stop the thread
                }
                cmdBufferPos = 0; // Reset the buffer position
            }
            else {
                buffer[cmdBufferPos++] = ch; // Store command characters
            }
        }
    }
}

int main(int argc, char* argv[]) {
    Camera cam;
    string videoPath = "";
    if (argc > 1) videoPath = argv[1];
    cv::VideoCapture video = cam.selectVideo(videoPath);
    Tracking tracker("MOSSE", MEDIUM, video);

    int uart_fd = openUART();

    std::thread listenerThread(commandListeningThread, uart_fd, std::ref(tracker), std::ref(video));
    listenerThread.join(); // This will keep main thread alive

    return 0;
}


/*
    Testing notes: use following command from build folder to run
    sudo ./TestMain ../src/walking.mp4


int main(int argc, char* argv[]) {

    // Local variables: Camera cam, Tracking tracker, SwarmUART swarmUart
    Camera cam;
    string videoPath = "";
    if (argc > 1) videoPath = argv[1];
    cv::VideoCapture video = cam.selectVideo(videoPath);

    Tracking tracker("MOSSE", MEDIUM, video);

    SwarmUART swarmUart(tracker);

    while (1) 
    {
        swarmUart.processCommand();
    }

    return 0;
}
*/
