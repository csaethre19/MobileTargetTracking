#include "MAVLinkUtils.h"
#include "UART.h"
#include "Logger.h"

int main() {
    // Create logger
    Logger appLogger("app_logger", "debug.log");
    auto logger = appLogger.getLogger();

    // Open uart
    UART uart(logger, "/dev/ttyS0");
    int uart_fd = uart.openUART();

    // Calculate center of bbox using hard-coded points
    Point p1(100, 100);
    Point p2(1000, 1000);
    int xc = (p1.x + p2.x) / 2;
    int yc = (p1.y + p2.y) / 2;
    
    // Calculate distance and update lat/lon with starting hard-coded lat/lon values
    double curr_lat = 40.75;
    double curr_lon = -111.93;
    double distance = calculate_distance(xc, yc);
    double updated_lat = calculate_updated_lat(curr_lat, distance);
    double updated_lon = calculate_updated_lon(curr_lon, distance);

    // Create gps MAVLink message and send over UART
    std::vector<uint8_t> gps_msg = create_gps_msg(updated_lat, updated_lon);
    auto [lat, lon, yaw, alt] = parse_gps_msg(gps_msg);
    // should print 40.76140, -111.918598 for lat/lon
    int num_wrBytes = write(uart_fd, gps_msg.data(), gps_msg.size());
}