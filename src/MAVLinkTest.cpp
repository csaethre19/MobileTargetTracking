#include <thread>
#include "MAVLinkUtils.h"
#include "Logger.h"
#include "UART.h"
#include <string>

using namespace std;

// void sendThread(int uart_fd) {
//     cout << "Inside sendThread\n";
//     // Send Hello World over uart with header
//     string message_payload = "Hello World"; 
//     char msg_id = 'b';
//     payloadPrepare(message_payload, msg_id, uart_fd);
//     cout << "Exiting sendThread\n";
// }

// void listeningThread(int uart_fd) {
//     cout << "Inside listeningThread\n";
//     char ch;
//     while (true) {
//         cout << "Listening for characters...\n";
//         if (read(uart_fd, &ch, 1) > 0) {
//             cout << "Received character: " << ch << endl; // Debug: print every received character
//             if (ch == '!') {
//                 cout << "got start char\n";
//                 char header[4];
//                 if (read(uart_fd, &header, 4) > 0) {
//                     cout << "read header portion\n";
//                     uint16_t payloadSize = static_cast<uint16_t>(static_cast<unsigned char>(header[0])) |
//                         (static_cast<uint16_t>(static_cast<unsigned char>(header[1])) << 8);
//                     cout << "parsed payload size: " << payloadSize << endl;
//                 }
//             }
//         }
//     }
//     cout << "Exiting listeningThread\n";
// }


int main() {
    Logger appLogger("app_logger", "debug.log");
    auto logger = appLogger.getLogger();

    UART uart(logger, "/dev/ttyS0");
    int uart_fd = uart.openUART();

    // std::thread listenerThread(listeningThread, uart_fd);
    // listenerThread.detach(); // Let the listener thread run independently

    // std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Give listener thread some time to start

    // std::thread helloWorldThread(sendThread, uart_fd);
    // helloWorldThread.join(); // Wait for the send thread to finish

    // Send Hello World over uart with header
    string message_payload = "Hello World"; 
    char msg_id = 'b';
    payloadPrepare(message_payload, msg_id, uart_fd);
    
    close(uart_fd); // Close UART port

    return 0;
}

