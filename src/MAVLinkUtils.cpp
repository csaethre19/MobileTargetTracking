#include "MAVLinkUtils.h"

std::tuple<double, double, double, double> parse_gps_msg(const std::vector<uint8_t>& buf) {
    mavlink_message_t msg;
    mavlink_status_t status;

    double lat = 0.0;
    double lon = 0.0;
    double yaw = 0.0;
    double alt = 0.0;

    // Parse the MAVLink message from the buffer
    for (size_t i = 0; i < buf.size(); ++i) {
        if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status)) {
            // Message successfully parsed
            if (msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) {
                // Create a structure to hold the decoded message
                mavlink_gps_raw_int_t gps_raw_int;
                mavlink_msg_gps_raw_int_decode(&msg, &gps_raw_int);

                // Extract latitude, longitude, altitude, and heading (yaw)
                lat = gps_raw_int.lat / 1E7;
                lon = gps_raw_int.lon / 1E7;
                alt = gps_raw_int.alt / 1E3; // Altitude is in millimeters
                yaw = gps_raw_int.cog / 100.0; // Heading is in centidegrees
            }
        }
    }

    std::cout << "lat: " << lat << std::endl;
    std::cout << "lon: " << lon << std::endl;
    std::cout << "yaw: " << yaw << std::endl;
    std::cout << "alt: " << alt << std::endl;

    return std::make_tuple(lat, lon, yaw, alt);
}

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