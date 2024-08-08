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



void transmitterThread(VideoTransmitter &vidTx, VideoCapture &video) 
{
    cout << "Starting to transmit video" << endl;
    Mat frame;
    cout << "transmit tracking frame flag: " << transmitTrackingFrame << endl;
    int frameRead;
    while ((frameRead = video.read(frame)) && !transmitTrackingFrame) {
        vidTx.transmitFrame(frame);
    }
    cout << "frameRead: " << frameRead << endl;
    cout << "Exiting transmitterThread" << endl;
}

void trackingThread(std::shared_ptr<spdlog::logger> &logger, int uart_fd, Point p1, Point p2, VideoTransmitter &vidTx, VideoCapture &video) 
{
    // Set flight mode to guided mode
    string payload = ""; // EMPTY PAYLOAD
    char msg_id = 'b';
    size_t bufferSize = 5 + payload.size();
    char fm_msg[bufferSize];

    // TODO: Add header to payload

    // Send payload to swarm-dongle
    int num_wrBytes = write(uart_fd, fm_msg.data(), fm_msg.size());

    Tracking tracker("MOSSE", video, logger);
    cout << "Begin of transmitting tracking frames..." << endl;
    transmitTrackingFrame = true;
    int width = p2.x - p1.x;
    int height = p2.y - p1.y;
    Rect bbox(p1.x, p1.y, width, height);
    Mat frame = tracker.initTracker(bbox);
    if (frame.empty()) {
        cout << "tracker initialization failed.\n";
        return;
    }

    cout << "Begin Tracking" << endl;
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
        calculate_distance(int xc, int yc, double &pixDistance, double &distance, double &angleInDegrees);
        
        double target_lat = 0.0;
        double target_lon = 0.0;
        // Calculate target gps coordinates based on distance calculated
        std::lock_guard<std::mutex> lock(pos_mtx);
        auto [target_lat, target_lon] = target_gps(angleInDegrees, distance, pos.yaw, pos.lat, pos.lon);
        
        // TO BE REMOVED: updating lat/lon to target lat/lon that was calculated
        pos.lat = target_lat;
        pos.lon = target_lon;
        ////////////////////////////////////////////////////////////////////////

        // Format string with target lat/lon
        string target_lat_str = std::to_string(target_lat);
        string target_lon_str = std::to_string(target_lon);
        payload = target_lat_str + " " + target_lon_str;
        msg_id = 'a';
        bufferSize = 5 + payload.size();
        char target_msg[bufferSize];

        // TODO: Add header to payload

        // Send payload to swarm-dongle
        num_wrBytes = write(uart_fd, target_msg, strlen(target_msg));

        cout << "Transmitting tracking frame now..." << endl;
        vidTx.transmitFrame(frame);
        
    }

    cout << "Tracking ended.\n";
    // need to start up transmitter thread and send track-fail to app
    transmitTrackingFrame = false;
    char msg[32];
    snprintf(msg, sizeof(msg), "D track-fail\n");
    int num_wrBytes = write(uart_fd, msg, strlen(msg));
    std::thread videoTxThread(transmitterThread, std::ref(vidTx), std::ref(video));
    videoTxThread.detach(); // video thread runs independently
}

void commandListeningThread(int uart_fd, std::shared_ptr<spdlog::logger> &logger, VideoTransmitter &vidTx, VideoCapture &video) {
    char ch;
    char buffer[256];
    int cmdBufferPos = 0;

    while (true) {
        if (read(uart_fd, &ch, 1) > 0) {
            if (ch == '\n') {
                cout << "Received new command...\n";
                std::lock_guard<std::mutex> lock(mtx);
                buffer[cmdBufferPos] = '\0';
                cout << "BUFFER: " << buffer << endl;
                // From grond-station (user app)
                if (strncmp(buffer, "R track-start", 13) == 0) {
                    // Extract coordinates of user selected region 
                    // e.g. 'track-start 448 261 528 381' -> Point p1(448, 261) Point p2(528, 381)
                    string extractedString = &buffer[14];
                    int x1, y1, x2, y2;
                    std::istringstream stream(extractedString);
                    if (stream >> x1 >> y1 >> x2 >> y2) {
                        cout << "x1: " << x1 << " y1: " << y1 << endl;
                        cout << "x2: " << x2 << " y2: " << y2 << endl;

                        Point p1(x1, y1); 
                        Point p2(x2, y2); 

                        cout << "Initiating tracking...\n";

                        continueTracking = true; // Set tracking flag
                        std::thread trackThread(trackingThread, std::ref(logger), uart_fd, p1, p2, std::ref(vidTx), std::ref(video));
                        trackThread.detach(); // Allow the thread to operate independently
                    }
                    else {
                        cout << "Unable to parse integers from track-start command\n";
                        cmdBufferPos = 0;
                        memset(buffer, 0, sizeof(buffer));
                        continue;
                    }
                }
                else if (strncmp(buffer, "R track-end", 11) == 0) {
                    cout << "Tracking stopping...\n";
                    continueTracking = false; // Clear tracking flag to stop the thread
                }
                // GPS Msg from swarm-dongle
                else {
                    // Whatever is in the buffer is expected to be a mavlink gps msg sent from swarm-dongle
                    // if it is not any of the other messages
                    auto [lat, lon, yaw, alt, sysid, compid] = parse_gps_msg(buffer);

                    std::cout << "Values parsed out from gps msg: " << std::endl;
                    std::cout << "Latitude: " << lat << std::endl;
                    std::cout << "Longitude: " << lon << std::endl;
                    std::cout << "Heading (Yaw): " << yaw << std::endl;
                    std::cout << "Altituide: " << alt << std::endl;

                    // Add new values to shared variable representing position
                    std::lock_guard<std::mutex> lock(pos_mtx);
                    pos.lat = lat;
                    pos.lon = lon;
                    pos.yaw = yaw;
                    pos.alt = alt;
                    pos.sys_id = sysid;
                    pos.comp_id = compid;

                    // confirming struct pos updated accordingly
                    cout << "pos.lat: " << pos.lat << endl << "pos.lon: " << pos.lon << endl << "pos.yaw: " << pos.yaw << endl << "pos.alt: " << pos.alt << endl;;

                    cmdBufferPos = 0; // Reset the buffer position
                    memset(buffer, 0, sizeof(buffer)); // Clear buffer after handling
                }
            }
            else {
                buffer[cmdBufferPos++] = ch; // Store command characters
            }
        }
    }
}

int main(int argc, char* argv[]) {

    // TO BE REMOVED: setting lat lon to hard-coded coordinates
    pos.lat = 40.7553044
    pos.lon = -111.9304837;
    pos.yaw = 0.0; // not sure what this should be?
    ///////////////////////////////////////////////////////////

    Logger appLogger("app_logger", "debug.log");
    auto logger = appLogger.getLogger();

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
