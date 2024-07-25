#ifndef MAVLINK_UTILS_H
#define MAVLINK_UTILS_H

#include <iostream>
#include <vector>
#include <tuple>
#include <common/mavlink.h>

using namespace std;

/*
    Given a buffer containing a formatted MAVLink packete,
    parses out the lat, lon, and heading (yaw) contained in GPS message.
*/
std::tuple<double, double, double> parse_gps_msg(const std::vector<uint8_t>& buf);

/*
    Given a lattitude and longitude input, constructs GPS MAVLink packet.
    Used to send over UART.
*/
std::vector<uint8_t> create_gps_msg(float lat_input, float lon_input);

/*
    Based on center point of bounding box (xc, yc), calculates distance
    from center of video frame to this point.
*/
float calculate_distance(int xc, int yc);

/*
    Based on current lattitude coordinate and distance to center of bounding box in frame, 
    calculates updated lattitude.
*/
float calculate_updated_lat(float curr_lat, float distance);

/*
    Based on current longitude coordinate and distance to center of bounding box in frame, 
    calculates updated longitude.
*/
float calculate_updated_lon(float curr_lon, float distance);

#endif

