#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <sstream>
#include <string>

#include "Camera.h"
#include "UART.h"

using namespace cv;

/*
    Testing notes: use following command from build folder to run
    sudo ./TestMain ../src/walking.mp4
*/

std::mutex mtx;
std::atomic<bool> continueTracking(true);
std::atomic<bool> transmitTrackingFrame(false)


void trackingThread(Tracking &tracker, VideoCapture &video, int uart_fd, Point p1, Point p2) {
    int width = p2.x - p1.x;
    int height = p2.y - p1.y;
    Rect bbox(p1.x, p1.y, width, height);
    Mat frame = tracker.initTracker(bbox);
    if (frame.empty()) {
        cout << "tracker initialization failed.\n";
        return;
    }

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
        // Send updated coordinates to swarm-dongle
        int num_wrBytes = write(uart_fd, loc, strlen(loc)); // another thing to consider, not flooding the swarm-dongle
                                                            // only send updated coordinate information when it is beyond some threshold...
        
        // TODO: switch to transmitting frame that is outputted via tracking algorithm
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
                cout << "Received new command...\n";
                std::lock_guard<std::mutex> lock(mtx);
                buffer[cmdBufferPos] = '\0'; // Null-terminate the command string
                cout << "BUFFER: " << buffer << endl;
                if (strncmp(buffer, "track-start", 11) == 0) {
                    // Extract coordinates of user selected region 
                    // e.g. 'track-start 448 261 528 381' -> Point p1(448, 261) Point p2(528, 381)
                    string extractedString = &buffer[12];
                    int x1, y1, x2, y2;
                    std::istringstream stream(extractedString);
                    if (stream >> x1 >> y1 >> x2 >> y2) {
                        cout << "x1: " << x1 << " y1: " << y1 << endl;
                        cout << "x2: " << x2 << " y2: " << y2 << endl;

                        Point p1(x1, y1); 
                        Point p2(x2, y2); 

                        cout << "Initiating tracking...\n";

                        continueTracking = true; // Set tracking flag
                        std::thread trackThread(trackingThread, std::ref(tracker), std::ref(video), uart_fd, p1, p2);
                        trackThread.detach(); // Allow the thread to operate independently
                    }
                    else {
                        cout << "Unable to parse integers from track-start command\n";
                        cmdBufferPos = 0;
                        memset(buffer, 0, sizeof(buffer));
                        continue;
                    }
                }
                else if (strncmp(buffer, "track-end", 9) == 0) {
                    cout << "Tracking stopping...\n";
                    continueTracking = false; // Clear tracking flag to stop the thread
                }
                cmdBufferPos = 0; // Reset the buffer position
                memset(buffer, 0, sizeof(buffer)); // Clear buffer after handling
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
    // Run application without path argument to default to camera module
    cv::VideoCapture video = cam.selectVideo(videoPath);
    Tracking tracker("MOSSE", MEDIUM, video);

    Mat frame;
    bool ok = video.read(frame);
    // TODO: start transmitFrameThread
    // Create thread function that spins off and continuously 
    // calls video.read(frame) and transmits via frame buffer:

    //  while (video.read(frame) && !transmitTrackingFrame) 
    //  { 
    //      // transmit frame 
    //  }

    // When we receive a start-track command -> transmitTrackingFrame = true
    // Transmitting will then take place in the trackThread to transmit the frame
    // getting updated by the tracking algorithm

    int uart_fd = openUART("/dev/ttyS0");

    // TODO: remove video as parameter to listenerThread - not needed!
    std::thread listenerThread(commandListeningThread, uart_fd, std::ref(tracker), std::ref(video));
    listenerThread.join(); // This will keep main thread alive

    close(uart_fd); // Close uart port

    return 0;
}
