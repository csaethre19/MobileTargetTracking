#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include "Camera.h"
#include "UART.h"

using namespace cv;

std::mutex mtx;
std::atomic<bool> continueTracking(true);


void trackingThread(Tracking &tracker, VideoCapture &video, int uart_fd) {
    // Hard coded points for walking video: (p1 and p2 will have to be read from swarm-dongle which is relaying it from the user app)
    Point p1(448, 261); 
    Point p2(528, 381); 
    int width = p2.x - p1.x;
    int height = p2.y - p1.y;
    Rect bbox(p1.x, p1.y, width, height);
    // Initialize tracking logic...
    Mat frame = tracker.initTracker(bbox);

    while (continueTracking && tracker.trackerUpdate(bbox, frame) != 0) {
        // Continue tracking and sending updates...
        // Calculate top-left and bottom-right corners of bbox
        Point p1(cvRound(bbox.x), cvRound(bbox.y));
        Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height));

        // Calculate center of bbox
        int xc = (p1.x + p2.x) / 2;
        int yc = (p1.y + p2.y) / 2;

        char loc[32];
        snprintf(loc, sizeof(loc), "update-loc %d %d", xc, yc);

        int num_wrBytes = write(uart_fd, loc, strlen(loc)); // another thing to consider, not flooding the swarm-dongle
                                                            // only send updated coordinate information when it is beyond some threshold...
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

    close(uart_fd); // Close uart port

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
