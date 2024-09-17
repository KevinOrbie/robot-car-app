/**
 * @file arduino_socket.cpp
 * @brief Wrapper around thermios.h to interface with arduino over serial interface.
 * 
 * @link https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
 */

/* =========================== Imports =========================== */
#include "arduino_socket.h"

/* Standard C Libraries */
#include <stdio.h>
#include <string.h>
#include <fcntl.h>      // Contains file controls like O_RDWR
#include <errno.h>      // Error integer and strerror() function
#include <unistd.h>     // write(), read(), close()
#include <termios.h>    // Contains POSIX terminal control definitions

/* Standard C++ Libraries */
#include<array>
#include<vector>
#include <stdexcept>

/* Custom C++ Libraries */
#include "common/logger.h"


/* ============================ Class =========================== */
/* Custom baudrate wrapper, to not have to expose <termios.h>. */
const std::array<int, static_cast<int>(ArduinoBaudRate::NUM_BAUDS)> BaudrateConvertion {
    B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, 
    B19200, B38400
};

ArduinoSocket::ArduinoSocket(ArduinoBaudRate baudrate, unsigned char vtime, unsigned char vmin) {
    /* Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device) */
    fd_ = open("/dev/ttyUSB0", O_RDWR);

    /* Verify opening was successful. */
    if (fd_ == -1) {
        LOGE("Serial port open issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("Serial port open issue");
    }

    /* ------------------------ Set Socket Settings ------------------------ */
    /* Create new termios struct, we call it 'tty' for convention */
    struct termios tty;

    /* Read in existing settings. */
    if(tcgetattr(fd_, &tty) != 0) {
        LOGE("tcgetattr issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("tcgetattr issue");
    }

    /* Set specific settings. */
    tty.c_cflag &= ~PARENB;         // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB;         // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE;          // Clear all bits that set the data size
    tty.c_cflag |= CS8;             // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS;        // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL;  // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO;           // Disable echo
    tty.c_lflag &= ~ECHOE;          // Disable erasure
    tty.c_lflag &= ~ECHONL;         // Disable new-line echo
    tty.c_lflag &= ~ISIG;           // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = vtime;  // Wait for up to 1s (10 x 0.1 seconds)
    tty.c_cc[VMIN]  = vmin;   // Return as soon as vmin chars are recieved

    /* Set in/out baud rate to the given baudrate. */
    cfsetispeed(&tty, BaudrateConvertion[static_cast<int>(baudrate)]);
    cfsetospeed(&tty, BaudrateConvertion[static_cast<int>(baudrate)]);

    /* Save tty settings. */
    if (tcsetattr(fd_, TCSANOW, &tty) != 0) {
        LOGE("tcsetattr issue (error %d: %s)", errno, strerror(errno));
        throw std::runtime_error("tcsetattr issue");
    }
}

ArduinoSocket::~ArduinoSocket() {
    close(fd_);
}

bool ArduinoSocket:: recieve(std::string& recieved_msg) {
    /* Allocate memory for read buffer, set size according to your needs. */
    char read_buf [256];

    /* Read bytes from serial port. */
    int num_bytes = read(fd_, read_buf, 256);

    if (num_bytes < 0) {
        LOGE("Reading from file descriptor issue (error %d: %s)", errno, strerror(errno));
        return false;
    }

    recieved_msg = std::string(read_buf, num_bytes);
    return true;
}

bool ArduinoSocket::send(const std::vector<uint8_t> msg) {
    /* Write bites over serial port. */
    int num_bytes = write(fd_, msg.data(), msg.size());

    if (num_bytes < 0) {
        LOGE("Writing to file descriptor issue (error %d: %s)", errno, strerror(errno));
        return false;
    }

    return true;
}


