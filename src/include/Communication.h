#include <string.h>  // For strerror
#include <fcntl.h>   // Contains file controls like O_RDWR
#include <errno.h>   // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h>  // write(), read(), close()

#include <iostream>

using namespace std;



class Communication{
    public:
    
    Communication();
    
    private:
    
    int uart_fd;


};

