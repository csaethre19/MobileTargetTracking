#ifndef MAVLINK_UTILS_H
#define MAVLINK_UTILS_H

#include <iostream>
#include <vector>
#include <tuple>
#include <common/mavlink.h>
#include <cstdint>
#include <cstring>

using namespace std;

/*
    Given a buffer containing a formatted MAVLink packete,
    parses out the lat, lon, heading (yaw), and altituide contained in GPS message.
*/
std::tuple<double, double, double, double, uint8_t, uint8_t> parse_gps_msg(const std::vector<uint8_t>& buf);

/*
    Given a lattitude and longitude input, constructs GPS MAVLink packet.
    Used to send over UART.
*/
std::vector<uint8_t> create_target_gps_msg(float lat_input, float lon_input);

std::vector<uint8_t> create_gps_msg(float lat_input, float lon_input);

/*
    Parses out custom gps data from buffer for lat, lon, and yaw.
*/
std::tuple<double, double, double> parseCustomGpsData(const char buf[]);

/*
    Calculates target lat lon for GPS coordinate.
 */
std::tuple<double, double> calculateTargetGps(double relative_target_yaw_deg, double target_offset_ft, double aircraft_heading_yaw_deg, double aircraft_lat, double aircraft_lon);

/*
    Based on center point of bounding box (xc, yc), calculates distance
    from center of video frame to this point.
*/
void calculateDistance(int xc, int yc, double &pixDistance, double &distance, double &angleInDegrees);

/*
    Prepares the payload for sending, by filling a buffer with the required header
    A pre-assigned region of memory of size (payload_bytes + 5) must be reserved. The first byte of this buffer is passed to char* buffer.
    The payload is a series of arbitrary bytes, total bytes in payload must be less than 62.
    messageID should match the purpose of the message, as defined by external documentation
*/
void payloadPrepare(const std::string& payload, char messageID, int uart_fd);

/*
    Packs double lat, lon repsentations into string.
*/
std::string packDoubleToString(double var1, double var2);

#endif

