#include "UART.h"


int openUART(void)
{
    // Open serial UART port
    int uart_fd = open("/dev/ttyS0", O_RDWR);

    if (uart_fd == -1) {
    cerr << "Error - Unable to open UART." << endl;
    }
    else {
    cerr << "Successfully opened UART!" << endl;

    }

    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(uart_fd, &tty) != 0) {
        cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
    }

    // https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
    tty.c_cflag &= ~PARENB;        // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB;        // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE;         // Clear all bits that set the data size 
    tty.c_cflag |= CS8;            // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS;       // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

	tty.c_lflag |= ICANON | ECHO; // Enable canonical mode and echo

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl

    // Configure input flags
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR);  // Clear existing input settings
    tty.c_iflag |= ICRNL;  // Translate carriage return to newline on input

    // Configure output flags
    tty.c_oflag &= ~OPOST;  // Prevent special interpretation of output bytes (e.g., newline chars)
    tty.c_oflag |= ONLCR;  // Map newline to carriage return-line feed on output

    tty.c_cc[VTIME] = 10;   // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0; 

    // Set in/out baud rate to be 9600
    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    // Save tty settings, also checking for error
    if (tcsetattr(uart_fd, TCSANOW, &tty) != 0) {
        cout << "Error " << errno << " from tcsetattr: " << strerror(errno) << endl;
    }

    return uart_fd;
}

