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
    double test_target_yaw = 290.0;
    // double distance = calculate_distance(xc, yc);
    double test_target_dist = 500;

    //Aircraft Values
    double test_aircraft_yaw = 150.0;
    double test_aircraft_lat = (40.7553044); // Latitude in degrees * 1E7
    double test_aircraft_lon = (-111.9304837); // Longitude in degrees * 1E7

    auto [updated_lat, updated_lon]  = target_gps(test_target_yaw, test_target_dist, test_aircraft_yaw, test_aircraft_lat, test_aircraft_lon);

    // Create gps MAVLink message and send over UART
    std::vector<uint8_t> gps_msg = create_gps_msg(updated_lat, updated_lon);
    auto [lat, lon, yaw, alt, sysid, compid] = parse_gps_msg(gps_msg);
    // int num_wrBytes = write(uart_fd, gps_msg.data(), gps_msg.size());
}