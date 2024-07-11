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

void parse_lat_lon(const std::vector<uint8_t>& buf) {
    mavlink_message_t msg;
    mavlink_status_t status;

    // Parse the MAVLink message from the buffer
    for (size_t i = 0; i < buf.size(); ++i) {
        if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status)) {
            // Message successfully parsed
            if (msg.msgid == MAVLINK_MSG_ID_GPS_RAW_INT) {
                // Create a structure to hold the decoded message
                mavlink_gps_raw_int_t gps_raw_int;
                mavlink_msg_gps_raw_int_decode(&msg, &gps_raw_int);

                // Extract latitude and longitude
                double lat = gps_raw_int.lat / 1E7;
                double lon = gps_raw_int.lon / 1E7;

                // Print latitude and longitude
                std::cout << "Latitude: " << lat << std::endl;
                std::cout << "Longitude: " << lon << std::endl;
            }
        }
    }
}

std::vector<uint8_t> create_gps_msg() {
    // Create buffer for the message
    std::vector<uint8_t> buf(MAVLINK_MAX_PACKET_LEN); // buffer for mavlink message using mavlink constant for max packet length
    mavlink_message_t msg; // create mavlink_message_t structure to hold message contents
    uint16_t len; // var to hold length of serialized message

    // Initialize the message
    uint8_t system_id = 1; // sets system ID for message
    uint8_t component_id = 200; // sets component ID for message - ID of the component sending the message (e.g., autopilot, gimbal, camera).

    // Define GPS coordinates and other parameters
    uint64_t time_usec = 0; // Timestamp (microseconds since UNIX epoch or system boot)
    int32_t lat = static_cast<int32_t>(40.755304405791286 * 1E7); // Latitude in degrees * 1E7
    int32_t lon = static_cast<int32_t>(-111.93048373250862 * 1E7); // Longitude in degrees * 1E7
    int32_t alt = 0; // Altitude in millimeters (placeholder value)
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

    return buf;
}

void print_msg(const std::vector<uint8_t>& buf) {
    // Print the message bytes
    std::cout << "MAVLink GPS_RAW_INT message: ";
    for (size_t i = 0; i < buf.size(); i++) {
        std::cout << std::hex << static_cast<int>(buf[i]) << " ";
    }
    std::cout << std::dec << std::endl;
}

int main() {
    std::cout << "Creating GPS MAVLink Message\n";
    std::vector<uint8_t> buf = create_gps_msg();
    print_msg(buf);
    std::cout << "Parsing out Lat/Lon from GPS MAVLink Message\n";
    parse_lat_lon(buf);

    return 0;
}
