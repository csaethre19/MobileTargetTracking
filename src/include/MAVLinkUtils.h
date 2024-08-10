#ifndef MAVLINK_UTILS_H
#define MAVLINK_UTILS_H

#include <iostream>
#include <vector>
#include <tuple>
#include <common/mavlink.h>

using namespace std;

/*
    Given a buffer containing a formatted MAVLink packete,
    parses out the lat, lon, heading (yaw), and altituide contained in GPS message.
*/
std::tuple<double, double, double, double, uint8_t, uint8_t> parse_gps_msg(const std::vector<uint8_t>& buf);

std::tuple<double, double, double> parse_custom_gps_data(const std::vector<uint8_t>& buf);

/*
    Given a lattitude and longitude input, constructs GPS MAVLink packet.
    Used to send over UART.
*/
std::vector<uint8_t> create_target_gps_msg(float lat_input, float lon_input);


std::vector<uint8_t> create_gps_msg(float lat_input, float lon_input);

/*
    Calculates target lat lon for GPS coordinate.
 */
std::tuple<double, double> target_gps(double relative_target_yaw_deg, double target_offset_ft, double aircraft_heading_yaw_deg, double aircraft_lat, double aircraft_lon);

/*
    Based on center point of bounding box (xc, yc), calculates distance
    from center of video frame to this point.
*/
void calculate_distance(int xc, int yc, double &pixDistance, double &distance, double &angleInDegrees);

void Payload_Prepare(const std::string& payload, char messageID, char* buffer);

std::string packDoubleToString(double var1, double var2);

#endif

