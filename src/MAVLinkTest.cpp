#include "MAVLinkUtils.h"
#include "UART.h"
#include "Logger.h"

int main() {
    // create logger
    Logger appLogger("app_logger", "debug.log");
    auto logger = appLogger.getLogger();

    // open uart
    UART uart(logger, "/dev/ttyS0");
    int uart_fd = uart.openUART();

    // Hard-coded points
    Point p1(448, 261);
    Point p2(528, 381);
    // Calculate center of bbox
    int xc = (p1.x + p2.x) / 2;
    int yc = (p1.y + p2.y) / 2;
    
    // Hard-coded lat/lon
    float curr_lat = 40.75;
    float curr_lon = -111.93;

    float distance = calculate_distance(xc, yc);
    float updated_lat = calculate_updated_lat(curr_lat, distance);
    float updated_lon = calculate_updated_lon(curr_lon, distance);

    std::vector<uint8_t> gps_msg = create_gps_msg(updated_lat, updated_lon);
    // int num_wrBytes = write(uart_fd, gps_msg, strlen(gps_msg)); // will this work?
    int num_wrBytes = write(uart_fd, gps_msg.data(), gps_msg.size());
}