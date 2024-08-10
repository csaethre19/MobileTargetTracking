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
            std::cout << "Parsed mavlink char\n";
            // Message successfully parsed
            if (msg.msgid == MAVLINK_MSG_ID_GLOBAL_POSITION_INT) {
                std::cout << "msg id was correct\n";
                // Create a structure to hold the decoded message
                mavlink_global_position_int_t global_position_int;
                mavlink_msg_global_position_int_decode(&msg, &global_position_int);

                // Extract latitude, longitude, altitude, and heading (yaw)
                lat = global_position_int.lat / 1E7;
                lon = global_position_int.lon / 1E7;
                alt = global_position_int.relative_alt / 1E3; // Altitude is in millimeters
                yaw = global_position_int.hdg / 100.0; // Heading is in centidegrees

                // Extract system and component IDs
                sysid = msg.sysid;
                compid = msg.compid;
            }
        }
    }

std::tuple<double, double, double> parse_custom_gps_data(const std::vector<uint8_t>& buf) {

    double lat = 0.0;
    double lon = 0.0;
    double yaw


}

    // std::cout << "lat: " << lat << std::endl;
    // std::cout << "lon: " << lon << std::endl;
    // std::cout << "yaw: " << yaw << std::endl;
    // std::cout << "alt: " << alt << std::endl;
    // std::cout << "sysid: " << static_cast<int>(sysid) << std::endl;
    // std::cout << "compid: " << static_cast<int>(compid) << std::endl;

    return std::make_tuple(lat, lon, yaw, alt, sysid, compid);
}

// This is not going to be used at this time - does not use the right mavlink function
std::vector<uint8_t> create_gps_msg(float lat_input, float lon_input) {
    // Create buffer for the message
    std::vector<uint8_t> buf(MAVLINK_MAX_PACKET_LEN); // buffer for mavlink message using mavlink constant for max packet length
    mavlink_message_t msg; // create mavlink_message_t structure to hold message contents
    uint16_t len; // var to hold length of serialized message

    // Initialize the message
    uint8_t system_id = 1; // sets system ID for message
    uint8_t component_id = 200; // sets component ID for message - ID of the component sending the message (e.g., autopilot, gimbal, camera).

    // Define target position and other parameters
    uint32_t time_boot_ms = 0; // Timestamp (milliseconds since system boot)
    int32_t lat = static_cast<int32_t>(lat_input * 1E7); // Latitude in degrees * 1E7
    int32_t lon = static_cast<int32_t>(lon_input * 1E7); // Longitude in degrees * 1E7
    int32_t alt = 30000; // Altitude in millimeters (placeholder value)
    int32_t relative_alt = 30000; // Altitude relative to the home position in millimeters
    int16_t vx = 0; // X velocity in cm/s (placeholder value)
    int16_t vy = 0; // Y velocity in cm/s (placeholder value)
    int16_t vz = 0; // Z velocity in cm/s (placeholder value)
    uint16_t hdg = UINT16_MAX; // Heading in centidegrees (placeholder value)

    // Pack the GLOBAL_POSITION_INT message
    mavlink_msg_global_position_int_pack(system_id, component_id, &msg, time_boot_ms, lat, lon, alt, relative_alt, vx, vy, vz, hdg);

    // Copy the message to the send buffer
    len = mavlink_msg_to_send_buffer(buf.data(), &msg);

    // Resize the buffer to the actual length of the serialized message
    buf.resize(len);

    return buf; // this gets passed over UART
}

// This is the mavlink message that will tell swarm where to tell the drone to go
std::vector<uint8_t> create_target_gps_msg(float lat_input, float lon_input) {
    // Create buffer for the message
    std::vector<uint8_t> buf(MAVLINK_MAX_PACKET_LEN); // buffer for mavlink message using mavlink constant for max packet length
    mavlink_message_t msg; // create mavlink_message_t structure to hold message contents
    uint16_t len; // var to hold length of serialized message

    // Initialize the message
    uint8_t system_id = 1; // sets system ID for message
    uint8_t component_id = 200; // sets component ID for message - ID of the component sending the message (e.g., autopilot, gimbal, camera).

    // Define target position and other parameters
    uint32_t time_boot_ms = 0; // Timestamp (milliseconds since system boot)
    uint8_t target_system = 1; // Target system ID
    uint8_t target_component = 1; // Target component ID
    uint8_t coordinate_frame = MAV_FRAME_GLOBAL_RELATIVE_ALT_INT; // Frame of reference

    int32_t lat = static_cast<int32_t>(lat_input * 1E7); // Latitude in degrees * 1E7
    int32_t lon = static_cast<int32_t>(lon_input * 1E7); // Longitude in degrees * 1E7
    float alt = 30.0f; // Altitude in meters (placeholder value)
    uint16_t yaw = UINT16_MAX; // Yaw angle in degrees * 100 (placeholder value)

    // The following fields are required but will be set to zero or max value as placeholders
    uint16_t type_mask = 0b0000111111000111; // Bitmask to indicate which dimensions should be ignored (use position only)
    float vx = 0.0f; // X velocity in m/s (ignored)
    float vy = 0.0f; // Y velocity in m/s (ignored)
    float vz = 0.0f; // Z velocity in m/s (ignored)
    float afx = 0.0f; // X acceleration/force (ignored)
    float afy = 0.0f; // Y acceleration/force (ignored)
    float afz = 0.0f; // Z acceleration/force (ignored)
    float yaw_rate = 0.0f; // Yaw rate in rad/s (ignored)

    std::cout << "lat: " << lat << " lon: " << lon << std::endl;

    // Pack the SET_POSITION_TARGET_GLOBAL_INT message
    mavlink_msg_set_position_target_global_int_pack(system_id, component_id, &msg, time_boot_ms, target_system, target_component, coordinate_frame, type_mask, lat, lon, alt, vx, vy, vz, afx, afy, afz, yaw, yaw_rate);

    // Copy the message to the send buffer
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

    printf("starting coords: lat=%F, lon=%F\n", aircraft_lat, aircraft_lon);

    //convert lat, long to radians
    aircraft_lat = aircraft_lat * deg_to_radian;
    aircraft_lon = aircraft_lon * deg_to_radian;
    //calculate the absolute heading of the target relative to 0 deg north
    true_target_yaw = relative_target_yaw_deg + aircraft_heading_yaw_deg;
    if(true_target_yaw > 360) true_target_yaw -= 360;
    true_target_yaw = true_target_yaw * deg_to_radian;
    //meters are used in calculation, convert target distance to meters
    //0.3048 for 50ft altitude

    double target_offset_meter = target_offset_ft * 10.3048;
    // Calculate the new latitude
    double new_lat_rad = asin(sin(aircraft_lat) * cos(target_offset_meter / R) + cos(aircraft_lat) * sin(target_offset_meter / R) * cos(true_target_yaw));
    
    // Calculate the new longitude
    double new_lon_rad = aircraft_lon + atan2(sin(true_target_yaw) * sin(target_offset_meter / R) * cos(aircraft_lat), cos(target_offset_meter / R) - sin(aircraft_lat) * sin(new_lat_rad));

    //convert to degree values for target lat, lon
    target_lat = new_lat_rad * radian_to_deg;
    target_lon = new_lon_rad * radian_to_deg;


    printf("%F,%Fred,marker,\n", target_lat, target_lon);

    return std::make_tuple(target_lat, target_lon);
}

void calculate_distance(int xc, int yc, double &pixDistance, double &distance, double &angleInDegrees) {
    // Calculate the center of the video frame
    int videoCenterX = 1280 / 2;
    int videoCenterY = 720 / 2;

    // Distance from the center of the video to the center of the bounding box
    int deltaX = xc - videoCenterX;
    int deltaY = videoCenterY - yc; // Invert the y-axis

    // Use Pythagorean's theorem
    pixDistance = std::sqrt(deltaX * deltaX + deltaY * deltaY);

    distance = pixDistance/720 * 250;
    distance /= 12;

    // Calculate the angle in radians
    double angleInRadians = std::atan2(deltaY, deltaX);

    // Convert to degrees
    angleInDegrees = angleInRadians * (180.0 / M_PI);

    // Adjust the angle to make 0 degrees the top of the frame
    // and angles increase clockwise
    angleInDegrees = 90.0 - angleInDegrees;

    // Normalize the angle to be within [0, 360) degrees
    if (angleInDegrees < 0) {
        angleInDegrees += 360.0;
    } else if (angleInDegrees >= 360.0) {
        angleInDegrees -= 360.0;
    }

    // Print the results
    // cout << "Distance: " << pixDistance << " pixels" << endl;
    // cout << "Distance: " << distance << " feet" << endl;
    // cout << "Angle: " << angleInDegrees << " degrees" << endl;
}

float calculate_updated_lat(float curr_lat, float distance) {
    // TODO
    return curr_lat;
}

float calculate_updated_lon(float curr_lon, float distance) {
    // TODO
    return curr_lon;

}


/*
Prepares the payload for sending, by filling a buffer with the required header
A pre-assigned region of memory of size (payload_bytes + 5) must be reserved. The first byte of this buffer is passed to char* buffer.
The payload is a series of arbitrary bytes, total bytes in payload must be less than 62.
messageID should match the purpose of the message, as defined by external documentation

*/
void Payload_Prepare(const std::string& payload, char messageID, char* buffer) {
    // Calculate the number of characters in the payload
    uint16_t payloadSize = static_cast<uint16_t>(payload.size());
    
    // Calculate the 2's complement of the payload
    int sum = 0;
    for(char c : payload) {
        sum += static_cast<uint8_t>(c);
    }
    uint8_t twosComplement = static_cast<uint8_t>(~sum + 1);

    // Fill the buffer
    buffer[0] = '!';
    buffer[1] = static_cast<char>(payloadSize & 0xFF); // Lower 8 bits of payloadSize
    buffer[2] = static_cast<char>((payloadSize >> 8) & 0xFF); // High 8 bits of payloadSize
    buffer[3] = static_cast<char>(twosComplement);
    buffer[4] = messageID;
    
    // Copy the payload into the buffer starting from index 5
    memcpy(buffer + 5, payload.data(), payload.size());
}

/*
WARNGING: THIS is extrememly specific to the packing of double lat, lon repsentations

*/
std::string packDoubleToString(double var1, double var2) {
    // Move the decimal point to the right by 7 digits and convert to int32_t
    int32_t intVar1 = static_cast<int32_t>(std::round(var1 * 1e7));
    int32_t intVar2 = static_cast<int32_t>(std::round(var2 * 1e7));

    // Create a string with enough space to hold the two int32_t variables
    std::string result;
    result.reserve(sizeof(int32_t) * 2);
    
    // Append the raw binary data of the first int32_t variable
    result.append(reinterpret_cast<const char*>(&intVar1), sizeof(int32_t));
    
    // Append the raw binary data of the second int32_t variable
    result.append(reinterpret_cast<const char*>(&intVar2), sizeof(int32_t));
    
    return result;
}