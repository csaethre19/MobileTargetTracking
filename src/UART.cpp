#include "UART.h"


int openUART(void)
{
    // Open serial UART port
    int uart_fd = open("/dev/ttyS0", O_RDWR);

    if (uart_fd == -1) {
        cerr << "Error - Unable to open UART." << endl;
    }
    else {
        cout << "Successfully opened UART!" << endl;
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

SwarmUART::SwarmUART(Tracking tracker)
    : tracker(tracker)
{
    openUART("/dev/ttyS0");
}

SwarmUART::~SwarmUART()
{
    close(uart_fd);
}

void SwarmUART::processCommand() 
{
    if (read(uart_fd, &ch, 1) > 0) {
        if (ch == '\n') {
            if (strncmp(buffer, "transmit-video", 14) == 0) {
                cout << "Initiating transmission of video from Raspberry Pi...\n";
                // TODO: start transmission of video
            }
            else if (strncmp(buffer, "track-start", 11) == 0) {
                cout << "Initiating tracking...\n";
                // TODO: need to parse out track-start and then separately the arguments passed for p1,p2
                // pass p1,p2 to tracker and start tracking

                // Hard coded points for walking video:
                Point p1(448, 261); 
                Point p2(528, 381); 
                int width = p2.x - p1.x;
                int height = p2.y - p1.y;
                Rect bbox(p1.x, p1.y, width, height);

                Mat frame = tracker.initTracker(bbox);

                while (tracker.trackerUpdate(bbox, frame) != 0) {
                    // Calculate top-left and bottom-right corners of bbox
                    Point p1(cvRound(bbox.x), cvRound(bbox.y));
                    Point p2(cvRound(bbox.x + bbox.width), cvRound(bbox.y + bbox.height));

                    // Calculate center of bbox
                    int xc = (p1.x + p2.x) / 2;
                    int yc = (p1.y + p2.y) / 2;

                    char loc[32];
                    snprintf(loc, sizeof(loc), "update-loc %d %d", xc, yc);
                    
                    int num_wrBytes = write(uart_fd, loc, strlen(loc)); // another thing to consider, not flooding the swarm-dongle
                                                                        // only send updated coordinate information when it is beyond some threshold...

                    // TODO: Determine what translation of (xc,yc) needs to happen before passing to swarm-dongle
                    // Possibility:
                    //  Calculate center point, C, from (p1,p2) of bounding box
                    //  Determine center of frame, X, - remains constant and represents where drone is relative to object located in frame
                    //  Calculate distance between C and X and use this information to update drone GPS coordinates
                }
                cout << "Tracking ended.\n";
            }
            else if (strncmp(buffer, "track-end", 9) == 0) {
                cout << "Tracking stopping...\n";
            }
            else {
                cout << "Command not recognized!" << endl;
            }
            cmdBufferPos = 0;
        }
        else {
            buffer[cmdBufferPos++] = ch;
        }

    }
}

void UART::openUART(const char* serial_port) 
{
    // Open serial UART port
    uart_fd = open(serial_port, O_RDWR);

    if (uart_fd == -1) {
        cerr << "Error - Unable to open UART." << endl;
    }
    else {
        cout << "Successfully opened UART!" << endl;
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
}

