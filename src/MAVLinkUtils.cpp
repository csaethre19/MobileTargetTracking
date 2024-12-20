#include "MAVLinkUtils.h"
#include "UART.h"


std::tuple<double, double, double> parseCustomGpsData(const char buf[]) {
    // buf contains int32_t - lat, int32_t - lon, uint16_t - yaw + \n;

    int32_t temp_lat = 0;
    int32_t temp_lon = 0;
    uint16_t temp_yaw = 0;

    // Copying bytes from buf to the respective variables
    std::memcpy(&temp_lat, &buf[0], sizeof(int32_t));
    std::memcpy(&temp_lon, &buf[4], sizeof(int32_t));
    std::memcpy(&temp_yaw, &buf[8], sizeof(uint16_t));

    // Converting to desired units
    double lat = temp_lat / 1E7;
    double lon = temp_lon / 1E7;
    double yaw = temp_yaw / 100.0;

    // cout << "BEFORE lat: " << lat << " lon: " << lon << " yaw: " << yaw << endl;

    return std::make_tuple(lat, lon, yaw);    
}

std::tuple<double, double> calculateTargetGps(double relative_target_yaw_deg, double target_offset_ft, double aircraft_heading_yaw_deg, double aircraft_lat, double aircraft_lon) {
    const double deg_to_radian = 0.01745329251;
    const double radian_to_deg = 57.2957795131;
    const double R = 6371000.0;//earths radius in meters
    
    //Output Values
    double true_target_yaw = 0.0;
    double target_lat = 0.0;
    double target_lon = 0.0;

    // printf("starting coords: lat=%F, lon=%F\n", aircraft_lat, aircraft_lon);

    //convert lat, long to radians
    aircraft_lat = aircraft_lat * deg_to_radian;
    aircraft_lon = aircraft_lon * deg_to_radian;
    //calculate the absolute heading of the target relative to 0 deg north
    true_target_yaw = relative_target_yaw_deg + aircraft_heading_yaw_deg;
    if(true_target_yaw > 360) true_target_yaw -= 360;
    true_target_yaw = true_target_yaw * deg_to_radian;
    //meters are used in calculation, convert target distance to meters
    //0.3048 for 50ft altitude

    double target_offset_meter = target_offset_ft * 0.3048;
    // Calculate the new latitude
    double new_lat_rad = asin(sin(aircraft_lat) * cos(target_offset_meter / R) + cos(aircraft_lat) * sin(target_offset_meter / R) * cos(true_target_yaw));
    
    // Calculate the new longitude
    double new_lon_rad = aircraft_lon + atan2(sin(true_target_yaw) * sin(target_offset_meter / R) * cos(aircraft_lat), cos(target_offset_meter / R) - sin(aircraft_lat) * sin(new_lat_rad));

    //convert to degree values for target lat, lon
    target_lat = new_lat_rad * radian_to_deg;
    target_lon = new_lon_rad * radian_to_deg;


    // printf("%F,%Fred,marker,\n", target_lat, target_lon);

    return std::make_tuple(target_lat, target_lon);
}

void calculateDistance(int xc, int yc, double &pixDistance, double &distance, double &angleInDegrees) {
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

void payloadPrepare(const std::string& payload, char messageID, int uart_fd) {
    uint16_t bufferSize = 5 + payload.size();
    //rounding buffersize to nearest 32 multiple
    if((bufferSize%32) != 0){
        bufferSize += (32 - bufferSize%32);
    }
    char buffer[bufferSize];
    // Padding
    for(int i = 0; i < bufferSize; i ++){ buffer[i] = '0';}

    // Calculate the number of characters in the payload
    uint16_t payloadSize = static_cast<uint16_t>(payload.size());
    
    // Calculate the 1's complement of the payload
    int32_t sum = 0;
    for(char c : payload) {
        sum += static_cast<uint8_t>(c);
    }
    uint8_t onesComplement = static_cast<uint8_t>(~sum);

    // Fill the buffer
    buffer[0] = '!';
    buffer[1] = static_cast<char>(payloadSize & 0xFF); // Lower 8 bits of payloadSize
    buffer[2] = static_cast<char>((payloadSize >> 8) & 0xFF); // High 8 bits of payloadSize
    buffer[3] = static_cast<char>(onesComplement);
    buffer[4] = messageID;

    // Copy the payload into the buffer starting from index 5
    memcpy(buffer + 5, payload.data(), payload.size());

    // uint16_t verifyPayloadSize = static_cast<uint16_t>(static_cast<unsigned char>(buffer[1])) |
    //     (static_cast<uint16_t>(static_cast<unsigned char>(buffer[2])) << 8);
    // cout << "parsed payload size: " << verifyPayloadSize << endl;

    // send message over uart
    int num_wrBytes = write(uart_fd, buffer, bufferSize);
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