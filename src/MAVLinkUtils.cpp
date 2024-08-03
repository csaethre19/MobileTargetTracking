#include "MAVLinkUtils.h"

std::tuple<double, double, double, double, uint8_t, uint8_t> parse_gps_msg(const std::vector<uint8_t>& buf) {
    mavlink_message_t msg;
    mavlink_status_t status;

    double lat = 0.0;
    double lon = 0.0;
    double yaw = 0.0;
    double alt = 0.0;
    uint8_t sysid = 0;
    uint8_t compid = 0;

    // Parse the MAVLink message from the buffer
    for (size_t i = 0; i < buf.size(); ++i) {
        if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status)) {
            // Message successfully parsed
            if (msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) {
                // Create a structure to hold the decoded message
                mavlink_gps_raw_int_t gps_raw_int;
                mavlink_msg_gps_raw_int_decode(&msg, &gps_raw_int); // TODO: need to update to use: global_position_int (33)

                // Extract latitude, longitude, altitude, and heading (yaw)
                lat = gps_raw_int.lat / 1E7;
                lon = gps_raw_int.lon / 1E7;
                alt = gps_raw_int.alt / 1E3; // Altitude is in millimeters
                yaw = gps_raw_int.cog / 100.0; // Heading is in centidegrees

                // Extract system and component IDs
                sysid = msg.sysid;
                compid = msg.compid;
            }
        }
    }

    std::cout << "lat: " << lat << std::endl;
    std::cout << "lon: " << lon << std::endl;
    std::cout << "yaw: " << yaw << std::endl;
    std::cout << "alt: " << alt << std::endl;
    std::cout << "sysid: " << static_cast<int>(sysid) << std::endl;
    std::cout << "compid: " << static_cast<int>(compid) << std::endl;

    return std::make_tuple(lat, lon, yaw, alt, sysid, compid);
}

// TODO: need to update to set_position_target_global_int (86)
std::vector<uint8_t> create_gps_msg(float lat_input, float lon_input) {
    // Create buffer for the message
    std::vector<uint8_t> buf(MAVLINK_MAX_PACKET_LEN); // buffer for mavlink message using mavlink constant for max packet length
    mavlink_message_t msg; // create mavlink_message_t structure to hold message contents
    uint16_t len; // var to hold length of serialized message

    // Initialize the message
    uint8_t system_id = 1; // sets system ID for message
    uint8_t component_id = 200; // sets component ID for message - ID of the component sending the message (e.g., autopilot, gimbal, camera).

    // Define GPS coordinates and other parameters
    uint64_t time_usec = 0; // Timestamp (microseconds since UNIX epoch or system boot)
    int32_t lat = static_cast<int32_t>(lat_input * 1E7); // Latitude in degrees * 1E7
    int32_t lon = static_cast<int32_t>(lon_input * 1E7); // Longitude in degrees * 1E7
    int32_t alt = 30; // Altitude in millimeters (placeholder value)
    uint16_t eph = UINT16_MAX; // GPS HDOP horizontal dilution of position (unitless). If unknown, set to UINT16_MAX
    uint16_t epv = UINT16_MAX; // GPS VDOP vertical dilution of position (unitless). If unknown, set to UINT16_MAX
    uint16_t vel = UINT16_MAX; // GPS ground speed in cm/s. If unknown, set to UINT16_MAX
    int16_t cog = UINT16_MAX; // Course over ground (NOT heading, but direction of movement) in degrees * 100. If unknown, set to UINT16_MAX
    uint8_t fix_type = 3; // 3D fix
    uint8_t satellites_visible = UINT8_MAX; // Number of satellites visible. If unknown, set to UINT8_MAX
    int32_t alt_ellipsoid = 0; // Altitude above ellipsoid in millimeters (placeholder value)
    uint32_t h_acc = UINT32_MAX; // Horizontal position uncertainty in millimeters (placeholder value)
    uint32_t v_acc = UINT32_MAX; // Vertical position uncertainty in millimeters (placeholder value)
    uint32_t vel_acc = UINT32_MAX; // Speed uncertainty in cm/s (placeholder value)
    uint32_t hdg_acc = UINT32_MAX; // Heading uncertainty in rad * 1e5 (placeholder value)
    uint16_t yaw = UINT16_MAX; // Yaw angle in degrees * 100 (placeholder value)

    // Pack the GPS_RAW_INT message
    // mavlink_msg_gps_raw_int_pack() packs a GPS_RAW_INT message into the msg structure
    mavlink_msg_gps_raw_int_pack(system_id, component_id, &msg, time_usec, fix_type, lat, lon, alt, eph, epv, vel, cog, satellites_visible, alt_ellipsoid, h_acc, v_acc, vel_acc, hdg_acc, yaw);

    // Copy the message to the send buffer
    // Serializes the packed MAVLink message into the buffer buf and returns the length of the serialized message
    len = mavlink_msg_to_send_buffer(buf.data(), &msg);

    // Resize the buffer to the actual length of the serialized message
    buf.resize(len);

    return buf; // this gets passed over UART
}

std::tuple<double, double> target_gps(double relative_target_yaw_deg, double target_offset_ft, double aircraft_heading_yaw_deg, double aircraft_lat, double aircraft_lon) {
    const double deg_to_radian = 0.01745329251;
    const double radian_to_deg = 57.2957795131;
    const double R = 6371000.0;//earths radius in meters
    
    //Output Values
    double true_target_yaw = 0.0;
    double target_lat = 0.0;
    double target_lon = 0.0;

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

    return std::make_tuple(target_lat, target_lon);
}

float calculate_distance(int xc, int yc) {
    // TODO
    return 0;
}

float calculate_updated_lat(float curr_lat, float distance) {
    // TODO
    return curr_lat;
}

float calculate_updated_lon(float curr_lon, float distance) {
    // TODO
    return curr_lon;
}