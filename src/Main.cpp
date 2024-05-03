#include "Tracking.h"
#include "Camera.h"
#include "UART.h"

using namespace cv;

/*
    Testing notes: use following command from build folder to run
    sudo ./TestMain ../src/walking.mp4
*/

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
                    // TODO: need to parse out track-start and then separately the arguments passed for p1,p2
                    // pass p1,p2 to tracker and start tracking

                    // Hard coded points for walking video:
                    Point p1(448, 261); 
                    Point p2(528, 381); 
                    int width = p2.x - p1.x;
                    int height = p2.y - p1.y;
                    Rect bbox(p1.x, p1.y, width, height);

                    Mat frame = tracker.initTracker(bbox);

                    while (tracker.trackerUpdate(bbox, frame) != 0) {
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

                        // TODO: Determine what translation of (xc,yc) needs to happen before passing to swarm-dongle
                        // Possibility:
                        //  Calculate center point, C, from (p1,p2) of bounding box
                        //  Determine center of frame, X, - remains constant and represents where drone is relative to object located in frame
                        //  Calculate distance between C and X and use this information to update drone GPS coordinates
                    }
                    cout << "Tracking ended.\n";
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
