#include <iostream>
#include <mavlink.h>

int main() {
    // Create a buffer for the message
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    mavlink_message_t msg;
    uint16_t len;

    // Initialize the message
    uint8_t system_id = 1;
    uint8_t component_id = 200;
    uint8_t target_system = 1;
    uint8_t target_component = 1;
    uint16_t payload = 0;

    // Pack the message
    mavlink_msg_heartbeat_pack(system_id, component_id, &msg, MAV_TYPE_QUADROTOR, MAV_AUTOPILOT_GENERIC, 0, 0, MAV_STATE_ACTIVE);

    // Copy the message to the send buffer
    len = mavlink_msg_to_send_buffer(buf, &msg);

    // Print the message bytes
    std::cout << "MAVLink message: ";
    for (int i = 0; i < len; i++) {
        std::cout << std::hex << static_cast<int>(buf[i]) << " ";
    }
    std::cout << std::dec << std::endl;

    return 0;
}
