#include <iostream>
#include <vector>
#include <common/mavlink.h>


std::vector<uint8_t> set_flight_mode(uint8_t system_id, uint8_t component_id, uint8_t target_system, uint8_t target_component, uint32_t custom_mode) {
    mavlink_message_t msg;
    mavlink_command_long_t command_long;
    std::vector<uint8_t> buf(MAVLINK_MAX_PACKET_LEN);
    uint16_t len; 

    // Set the command parameters
    command_long.target_system = target_system;            // Target system ID (drone)
    command_long.target_component = target_component;      // Target component ID
    command_long.command = MAV_CMD_DO_SET_MODE;            // Command ID to set mode
    command_long.confirmation = 0;                         // No confirmation
    command_long.param1 = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED; // Enable custom mode
    command_long.param2 = custom_mode;                     // Custom mode (GUIDED mode for ArduPilot)
    command_long.param3 = 0;                               // Unused
    command_long.param4 = 0;                               // Unused
    command_long.param5 = 0;                               // Unused
    command_long.param6 = 0;                               // Unused
    command_long.param7 = 0;                               // Unused

    // Copy the message to the send buffer
    // Serializes the packed MAVLink message into the buffer buf and returns the length of the serialized message
    len = mavlink_msg_to_send_buffer(buf.data(), &msg);

    // Resize the buffer to the actual length of the serialized message
    buf.resize(len);

    return buf; // this gets passed over UART
}

int main() {
    uint8_t system_id = 1;            // ID of this system
    uint8_t component_id = 1;         // ID of this component
    uint8_t target_system = 1;        // ID of the drone
    uint8_t target_component = 1;     // ID of the component on the drone
    uint32_t custom_mode = 4;         // Custom mode number for GUIDED mode in ArduPilot

    // Set the flight mode to GUIDED
    set_flight_mode(system_id, component_id, target_system, target_component, custom_mode);

    return 0;
}
