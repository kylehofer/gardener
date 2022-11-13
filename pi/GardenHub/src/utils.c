// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()

int close_port(int port_fd)
{
    if (port_fd > 0)
    {
        if (close(port_fd) < 0)
        {
            printf("Error while trying to close port. Error: %s\n", strerror(errno));
            return -1;
        }
    }
    return 0;
}

int open_port(const char *port, int baud, char parity, int data_bit, int stop_bits)
{
    int serial_fd;
    struct termios tty;
    serial_fd = open(port, O_RDWR);

    // Check for errors
    if (serial_fd < 0)
    {
        printf("Error while trying to open port: %s. Error: %s\n", port, strerror(errno));
        close_port(serial_fd);
        return -1;
    }

    if (tcgetattr(serial_fd, &tty) != 0)
    {
        printf("Error while trying to get parameters from the port: %s. Error: %s\n", port, strerror(errno));
        close_port(serial_fd);
        return -1;
    }

    switch (parity)
    {
    case 'N':
        tty.c_cflag &= ~PARENB; // Clear parity
        break;
    case 'E':
        tty.c_cflag = (PARENB | tty.c_cflag) & ~PARODD; // Set parity and clear odd
        break;
    case 'O':
        tty.c_cflag |= (PARENB | PARODD);
        break;
    default:
        printf("Error: %c parity is not supported. Supported parity: N, E, O\n", parity);
        close_port(serial_fd);
        return -1;
    }

    switch (stop_bits)
    {
    case 1:
        tty.c_cflag &= ~CSTOPB; // Clear Stop bit. Aka 1 stop bit
        break;
    case 2:
        tty.c_cflag |= CSTOPB; // Set Stop bit. Aka 2 stop bits
        break;
    default:
        printf("Error: %d stop bits are not supported. Supported stop bits: 1, 2\n", stop_bits);
        close_port(serial_fd);
        return -1;
    }

    tty.c_cflag &= ~CSIZE; // Clear all the size bits
    
    switch (data_bit)
    {
    case 8:
        tty.c_cflag |= CS8; // 8 bits per byte
        break;
    case 7:
        tty.c_cflag |= CS7; // 7 bits per byte
        break;
    case 6:
        tty.c_cflag |= CS6; // 6 bits per byte
        break;
    case 5:
        tty.c_cflag |= CS5; // 5 bits per byte
        break;
    default:
        printf("Error: %d data bits are not supported. Supported data bits: 5, 6, 7, 8\n", data_bit);
        close_port(serial_fd);
        return -1;
    }

    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON; // Disabling Canoical mode

    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

     // No wait
    tty.c_cc[VTIME] = 0;
    tty.c_cc[VMIN] = 0;

    switch (baud)
    {
    case B9600:
    case B19200:
    case B38400:
        cfsetspeed(&tty, baud);
        break;
    default:
        printf("Error: %d not supported. Supported Bauds: B9600, B19200, B38400\n", baud);
        close_port(serial_fd);
        return -1;
    }

    // Save tty settings, also checking for error
    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0)
    {
        printf("Error while trying to set parameters to the port: %s. Error: %s\n", port, strerror(errno));
        close_port(serial_fd);
        return -1;
    }

    return serial_fd;
}

int set_port_synchronization(const char *port, int vmin, int vtime)
{
    int serial_fd;
    struct termios tty;
    serial_fd = open(port, O_RDWR);

    // Check for errors
    if (serial_fd < 0)
    {
        printf("Error while trying to open port: %s. Error: %s\n", port, strerror(errno));
        close_port(serial_fd);
        return -1;
    }

    if (tcgetattr(serial_fd, &tty) != 0)
    {
        printf("Error while trying to get parameters from the port: %s. Error: %s\n", port, strerror(errno));
        close_port(serial_fd);
        return -1;
    }

    // NSetting Syncronization    
    tty.c_cc[VMIN] = vmin;
    tty.c_cc[VTIME] = vtime;

    // Save tty settings, also checking for error
    if (tcsetattr(serial_fd, TCSANOW, &tty) != 0)
    {
        printf("Error while trying to set parameters to the port: %s. Error: %s\n", port, strerror(errno));
        close_port(serial_fd);
        return -1;
    }

    return close_port(serial_fd);
}