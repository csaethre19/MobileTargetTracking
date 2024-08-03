#include <iostream>
#include <vector>
#include <common/mavlink.h>

/*

    Explanation of MAVLink Message output:
    Start Byte (Magic Byte): fd
    Payload Length: 34 (52 bytes of payload for GPS_RAW_INT)
    Incompatibility Flags: 0
    Compatibility Flags: 0
    Sequence: 0
    System ID: 1
    Component ID: c8 (200 in decimal)
    Message ID: 18 (24 in decimal, which is the message ID for GPS_RAW_INT)
    Payload:
    time_usec: 0 0 0 0 0 0 0 0
    fix_type: 3
    lat: 14 c4 4a 18 (473592600 in decimal, corresponding to 40.755304405791286 * 1E7)
    lon: 7b c3 48 bd (-1223349000 in decimal, corresponding to -111.93048373250862 * 1E7)
    alt: 0 0 0 0
    eph: ff ff
    epv: ff ff
    vel: ff ff
    cog: ff ff
    satellites_visible: ff
    alt_ellipsoid: 0 0 0 0
    h_acc: ff ff ff ff
    v_acc: ff ff ff ff
    vel_acc: ff ff ff ff
    hdg_acc: ff ff ff ff
    yaw: 2a b2 (UINT16_MAX, placeholder value)

*/


int lat_long_mod_test(double relative_target_yaw_deg, double target_offset_ft, double aircraft_heading_yaw_deg, double aircraft_lat, double aircraft_lon) {
    // ---------------TEST VALUES-------------------//
    //Target Values provided in function decleration


    //
    //Static Values
    const double deg_to_radian = 0.01745329251;
    const double radian_to_deg = 57.2957795131;
    const double R = 6371000.0;//earths radius in meters
    
    //Output Values
    double true_target_yaw = 0.0;
    double target_lat = 0.0;
    double target_lon = 0.0;
    //

    printf("starting coords: %F, %F\n", aircraft_lat, aircraft_lon);

    //convert lat, long to radians
    aircraft_lat = aircraft_lat * deg_to_radian;
    aircraft_lon = aircraft_lon * deg_to_radian;
    //calculate the absolute heading of the target relative to 0 deg north
    true_target_yaw = relative_target_yaw_deg + aircraft_heading_yaw_deg;
    if(true_target_yaw > 360) true_target_yaw -= 360;
    true_target_yaw = true_target_yaw * deg_to_radian;
    //meters are used in calculation, convert target distance to meters
    double target_offset_meter = target_offset_ft * 0.3048;
    // Calculate the new latitude
    double new_lat_rad = asin(sin(aircraft_lat) * cos(target_offset_meter / R) + cos(aircraft_lat) * sin(target_offset_meter / R) * cos(true_target_yaw));
    
    // Calculate the new longitude
    double new_lon_rad = aircraft_lon + atan2(sin(true_target_yaw) * sin(target_offset_meter / R) * cos(aircraft_lat), cos(target_offset_meter / R) - sin(aircraft_lat) * sin(new_lat_rad));

    //convert to degree values for target lat, lon
    target_lat = new_lat_rad * radian_to_deg;
    target_lon = new_lon_rad * radian_to_deg;

    printf("calcualted coords: %F, %F\n", target_lat, target_lon);

    return 1;
}

int main() {
    std::cout << "Beginning gps management test\n";
    double test_target_yaw = 290.0;
    double test_target_dist = 500;
    //
    //Aircraft Values
    double test_aircraft_yaw = 150.0;
    double test_aircraft_lat = (40.7553044); // Latitude in degrees * 1E7
    double test_aircraft_lon = (-111.9304837); // Longitude in degrees * 1E7
    int buf = lat_long_mod_test(test_target_yaw, test_target_dist, test_aircraft_yaw, test_aircraft_lat, test_aircraft_lon);
    //print_msg(buf);
    //std::cout << "Parsing out Lat/Lon from GPS MAVLink Message\n";
    //parse_lat_lon(buf);
    std::cout << "gps management test complete\n";
    return 0;
}
