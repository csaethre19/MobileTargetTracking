#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <sstream>
#include <string>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "Camera.h"
#include "UART.h"
#include "VideoTransmitter.h"
#include "Logger.h"
#include "MAVLinkUtils.h"

using namespace cv;

/*
    Testing notes: use following command from build folder to run
    sudo ./TestMain 
    This will access camera, enter transmit video thread, enter listening commands thread,
    and waits for commands to come in over uart.
    Upon getting track-start command, initiates tracking and switches to transmitting
    tracking frames.
*/

std::mutex mtx;
std::atomic<bool> continueTracking(true);
std::atomic<bool> transmitTrackingFrame(false);

struct Position {
    double lat;
    double lon;
    double yaw;
    double alt;
    uint8_t sys_id;
    uint8_t comp_id;

};

Position pos;
std::mutex pos_mtx;

Logger appLogger("app_logger", "debug.log");
auto logger = appLogger.getLogger();



void transmitterThread(VideoTransmitter &vidTx, VideoCapture &video) 
{
    logger->debug("Transmitter Thread: Beginning to transmit video frames.");
    Mat frame;
    int frameRead;
    while ((frameRead = video.read(frame)) && !transmitTrackingFrame) {
        vidTx.transmitFrame(frame);
    }
    logger->debug("Exiting Transmitter Thread.");
}

void trackingThread(std::shared_ptr<spdlog::logger> &logger, int uart_fd, Point p1, Point p2, VideoTransmitter &vidTx, VideoCapture &video) 
{
    // Set flight mode to guided mode
    string message_payload = ""; // EMPTY PAYLOAD
    char msg_id = 'b';
    Payload_Prepare(message_payload, msg_id, uart_fd);

    // Initialize tracking
    Tracking tracker("MOSSE", video, logger);
    transmitTrackingFrame = true;
    int width = p2.x - p1.x;
    int height = p2.y - p1.y;
    Rect bbox(p1.x, p1.y, width, height);
    Mat frame = tracker.initTracker(bbox);
    if (frame.empty()) {
        logger->error("Tracking Thread: Tracker initialization failed.");
        return;
    }

    uint16_t onehz_update_counter = 0;

    logger->debug("Tracking Thread: Entering tracking process...");
    while (continueTracking && tracker.trackerUpdate(bbox, frame) != 0) {
        // Continue tracking and sending updates...
        // Calculate top-left and bottom-right corners of bbox
        Point p1(cvRound(bbox.x), cvRound(bbox.y));
        Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height));

        // Calculate center of bbox
        int xc = (p1.x + p2.x) / 2;
        int yc = (p1.y + p2.y) / 2;

        double pixDistance = 0.0;
        double distance = 0.0;
        double angleInDegrees = 0.0;
        // Calculate distance based on center point of updated bbox
        calculate_distance(xc, yc, pixDistance, distance, angleInDegrees);
        
        // Calculate target gps coordinates based on distance calculated

        //------------------ ~~~1 hz update to aircraft position, request updated position of airacraft at the same time------------------//
        onehz_update_counter++;
        if(onehz_update_counter > 20){
            //prepare a payload into a string data type
            std::lock_guard<std::mutex> lock(pos_mtx);
            auto [target_lat, target_lon] = target_gps(angleInDegrees, distance, pos.yaw, pos.lat, pos.lon);
            string message_payload = packDoubleToString(target_lat, target_lon);
            msg_id = 'a';
            Payload_Prepare(message_payload, msg_id, uart_fd);

            onehz_update_counter = 0;
            
            message_payload = "";
            msg_id = 'd';
            Payload_Prepare(message_payload, msg_id, uart_fd);
        }
        vidTx.transmitFrame(frame);
    }

    // Set flight mode to loiter mode
    message_payload = ""; // EMPTY PAYLOAD
    msg_id = 'c';
    Payload_Prepare(message_payload, msg_id, uart_fd);

    logger->debug("Tracking Thread: Tracking Ended.");
    // need to start up transmitter thread and send track-fail to app
    transmitTrackingFrame = false;

    // Send desktop app track fail message
    msg_id = 'g';
    string trackfailstring = "D track-fail\n";
    Payload_Prepare(trackfailstring, msg_id, uart_fd);

    std::thread videoTxThread(transmitterThread, std::ref(vidTx), std::ref(video));
    videoTxThread.detach(); // video thread runs independently
}

void commandListeningThread(int uart_fd, std::shared_ptr<spdlog::logger> &logger, VideoTransmitter &vidTx, VideoCapture &video) 
{
    char ch;
    char buffer[256];
    int cmdBufferPos = 0;

    while (true) {
        if (read(uart_fd, &ch, 1) > 0) {
            if (ch == '\n') {
                logger->debug("Listening Thread: received new command.");
                std::lock_guard<std::mutex> lock(mtx);
                buffer[cmdBufferPos] = '\0';
                // Look past header to get payload
                char* payload = &buffer[5];
                logger->debug("Listening Thread: contents of payloadfer={}", payload);

                // User initiated start of tracking 
                if (strncmp(payload, "R track-start", 13) == 0) {
                    // Extract coordinates of user selected region 
                    // e.g. 'track-start 448 261 528 381' -> Point p1(448, 261) Point p2(528, 381)
                    string extractedString = &payload[14];
                    int x1, y1, x2, y2;
                    std::istringstream stream(extractedString);
                    if (stream >> x1 >> y1 >> x2 >> y2) {
                        cout << "x1: " << x1 << " y1: " << y1 << endl;
                        cout << "x2: " << x2 << " y2: " << y2 << endl;

                        Point p1(x1, y1); 
                        Point p2(x2, y2); 

                        continueTracking = true; // Set tracking flag
                        std::thread trackThread(trackingThread, std::ref(logger), uart_fd, p1, p2, std::ref(vidTx), std::ref(video));
                        trackThread.detach(); // Allow the thread to operate independently
                    }
                    else {
                        logger->error("Listening Thread: Unable to parse integers from track-start command.");
                        cmdBufferPos = 0;
                        memset(buffer, 0, sizeof(buffer));
                        continue;
                    }
                }
                // User initiated tracking end
                else if (strncmp(payload, "R track-end", 11) == 0) {
                    logger->debug("Listening Thread: Tracking has been stopped by user, received track-end command.");
                    continueTracking = false; // Clear tracking flag to stop the thread
                }
                // GPS Msg from swarm-dongle
                else {
                    //NEW VERSION OF AIRCRAFT POSITION ONLY INCLUDES LAT, LONG, YAW
                    auto [lat, lon, yaw] = parse_custom_gps_data(payload);
                }
                // Reset buffer
                cmdBufferPos = 0;
                memset(buffer, 0, sizeof(buffer));
            }
            else {
                buffer[cmdBufferPos++] = ch; // Store command characters
            }
        }
    }
}

int main(int argc, char* argv[]) {

    // TO BE REMOVED: setting lat lon to hard-coded coordinates
    pos.lat = 40.7553044;
    pos.lon = -111.9304837;
    pos.yaw = 0.0; // not sure what this should be?
    ///////////////////////////////////////////////////////////

    Camera cam(logger);
    string videoPath = "";
    if (argc > 1) videoPath = argv[1];
    // Run application without path argument to default to camera module
    cv::VideoCapture video = cam.selectVideo(videoPath);

    VideoTransmitter vidTx(logger);
    std::thread videoTxThread(transmitterThread, std::ref(vidTx), std::ref(video));
    videoTxThread.detach(); // video thread runs independently
    
    UART uart(logger, "/dev/ttyS0");
    int uart_fd = uart.openUART();

    std::thread listenerThread(commandListeningThread, uart_fd, std::ref(logger), std::ref(vidTx), std::ref(video));
    listenerThread.join(); // This will keep main thread alive

    close(uart_fd); // Close uart port

    return 0;
}
