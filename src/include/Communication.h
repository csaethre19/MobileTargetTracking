#include <string.h>  // For strerror
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include <iostream>

/*
 * 
 * Note: Right now we are using serial port ttyS0 and this defaults to being used for SSH console login
 * In order to test use of uart on this port we need to disable agetty from using this port.
 * Run following commands to DISABLE:
 * sudo systemctl stop serial-getty@ttyS0.service
 * sudo systemctl disable serial-getty@ttyS0.service
 * 
 * To RE-ENABLE:
 * sudo systemctl enable serial-getty@ttyS0.service
 * sudo systemctl start serial-getty@ttyS0.service
 * 
 * TODO: Need to look into the use of ttyAMA0 serial port (defaults to being used for Bluetooth)
 * for gimbal communication.
 * 
 * /

using namespace std;



class Communication{
    public:
    
    Communication();
    
    private:
    
    int uart_fd;


};

