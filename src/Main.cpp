#include "Tracking.h"
#include "Camera.h"
#include "UART.h"

using namespace cv;

int main(int argc, char* argv[]) {

    Camera cam;
    string videoPath = "";
    if (argc > 1) videoPath = argv[1];
    cv::VideoCapture video = cam.selectVideo(videoPath);

    Tracking tracker("MOSSE", MEDIUM, video);

    int uart_fd = openUART();
    char buffer[128];
    int cmdBufferPos = 0;
    char ch;

    while (1) 
    {
        if (read(uart_fd, &ch, 1) > 0) {
            if (ch == '\n') {
                
                if (strncmp(buffer, "transmit-video", 14) == 0) {
                    cout << "Initiating transmission of video from Raspberry Pi...\n";
                    // TODO: start transmission of video
                }
                else if (strncmp(buffer, "track-start", 11) == 0) {
                    cout << "Initiating tracking...\n";
                    // Hard coded points for walking video:
                    Point p1(448, 261); 
                    Point p2(528, 381); 
                    int width = p2.x - p1.x;
                    int height = p2.y - p1.y;
                    Rect bbox(p1.x, p1.y, width, height);

                    Mat frame = tracker.initTracker(bbox);

                    while (tracker.trackerUpdate(bbox, frame) != 0) {
                        Point p1(cvRound(bbox.x), cvRound(bbox.y));                            // Top left corner
                        Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height)); // Bottom right corner
                        cout << "BBOX: " << endl;
                        cout << "Point p1: (" << p1.x << ", " << p1.y << ")" << endl;
                        cout << "Point p2: (" << p2.x << ", " << p2.y << ")" << endl;
                    }
                    cout << "Tracking ended.\n";
                    // pass p1 and p2 as references to a new continuous tracking method 
                    // put call to method in while loop and continue until found is false
                    // only call update once in the method and return found but also modifying the p1 and p2 in the bounding box

                    // TODO: need to parse out track-start and then separately the arguments passed for p1,p2
                    // pass p1,p2 to tracker and start tracking
                    // Output of tracker should write over uart_fd an 'update-loc' command with new coordinates
                }
                else if (strncmp(buffer, "track-end", 9) == 0) {
                    cout << "Tracking stopping...\n";

                }
                else {
                    cout << "Command not recognized!" << endl;
                }
                cmdBufferPos = 0;
            }
            else {
                buffer[cmdBufferPos++] = ch;
            }

        }
    }

    close(uart_fd);

    /*
    Camera cam;
    string videoPath = "";
    if (argc > 1) videoPath = argv[1];
    cv::VideoCapture video = cam.selectVideo(videoPath);
    Tracking tracker("MOSSE", MEDIUM, video);
    tracker.continuousTracking();
    */
    return 0;
}
