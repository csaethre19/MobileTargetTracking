#include "Camera.h"
#include "UART.h"

using namespace cv;

/*
    Testing notes: use following command from build folder to run
    sudo ./TestMain ../src/walking.mp4
*/

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
