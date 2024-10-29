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
#include <unordered_map>

/* Custom C++ Libraries */
#include "common/logger.h"
#include "arduino_types.h"


/* ============================ Class =========================== */
/* Custom baudrate wrapper, to not have to expose <termios.h>. */
const std::array<int, static_cast<int>(ArduinoBaudRate::NUM_BAUDS)> BaudrateConvertion {
    B0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800, B2400, B4800, B9600, 
    B19200, B38400, B115200
};

static std::string stringify_buffer(uint8_t *buffer, int length) {
    std::string output = "";
    for (int i = 0; i < length; i++) {
        switch (buffer[i]) {
            case static_cast<uint8_t>('<'):
                output += "<";
                break;

            case static_cast<uint8_t>('>'):
                output += ">";
                break;
            
            default: 
                /* Create HEX representation. */
                char hex[3];
                sprintf(hex, "%02X", buffer[i]);
                output += std::string(hex);
                break;
        };
        
        output += " ";
    }
    return output;
}

ArduinoSocket::ArduinoSocket(ArduinoBaudRate baudrate, unsigned char vtime, unsigned char vmin) {
    /* Open the serial port. Change device path as needed (currently set to an standard FTDI USB-UART cable type device) */
    fd_ = open("/dev/ttyACM0", O_RDWR);

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

    flush();
}

ArduinoSocket::~ArduinoSocket() {
    close(fd_);
}

void ArduinoSocket::flush() {
    tcflush(fd_, TCIOFLUSH);
}

/**
 * @brief Block this thread until data is available to read.
 */
void ArduinoSocket::wait() {
    // TODO: implement
};

/**
 * @brief Recieve Bytes from arduino, and parse message data.
 * @warning Don't call recieve() and getMessage() in different threads.
 */
void ArduinoSocket::recieve() {
    /* Allocate memory for read buffer. */
    static uint8_t read_buf[32];
    static int last_filled_index = -1;

    /* Read bytes from serial port. */
    /* Note: This read() is thread-safe, no mutex required (can result in deadlock). */
    int num_bytes = read(fd_, &read_buf[last_filled_index + 1], 32 - last_filled_index);

    if (num_bytes < 0) {
        LOGE("Reading from file descriptor issue (error %d: %s)", errno, strerror(errno));
        return;
    } else if ( num_bytes == 0) {
        return;
    }
    
    last_filled_index += num_bytes;
    // LOGI(">>> Recieved #bytes: %d", num_bytes);
    // LOGI("Last filled buffer index: %d", last_filled_index);
    // LOGI("Buffer: [%s]", stringify_buffer(read_buf, last_filled_index + 1).c_str());

    /* Parse out messages. */
    enum class ParseState {OPEN, ID, DATA, CLOSE, FINISHED} state = ParseState::OPEN;
    arduino::MessageID msg_id = arduino::MessageID::EMPTY;
    int last_processed_index = -1;
    int close_index = -1;
    int open_index = -1;

    for (int i=0; i <= last_filled_index; i++) {
        /* Parse incoming bytes depending on state. */
        switch (state) {
            /* Wait for an open character. */
            case ParseState::OPEN: {
                if (read_buf[i] == static_cast<uint8_t>('<')) {
                    state = ParseState::ID;
                    last_processed_index = i - 1;
                    open_index = i; 
                }
            } break;

            /* Recieve a MessageID character. */
            case ParseState::ID: {
                msg_id = static_cast<arduino::MessageID>(read_buf[i]);
                if (arduino::getDataSize(msg_id) > 0) {
                    state = ParseState::DATA;
                } else {
                    /* Skip data if msg type has no specified data length. */
                    state = ParseState::CLOSE;
                }
            } break;
            
            /* Wait for all data characters. */
            case ParseState::DATA: {
                /* Number of data elements recieved, including the current one. */
                int data_size = i - open_index - 1;
                if (data_size >= arduino::getDataSize(msg_id)) {
                    state = ParseState::CLOSE;
                }
            } break;

            /* Wait for the close character. */
            case ParseState::CLOSE: {
                if (read_buf[i] == static_cast<uint8_t>('>')) {
                    state = ParseState::FINISHED;
                    close_index = i; 
                }
            } break;

            default: break;
        };

        /* Message detected, add to queue. */
        if (state == ParseState::FINISHED) {
            // LOGI("Message Detected: open: %d, close: %d", open_index, close_index);

            /* Create Message */
            arduino::Message msg = {msg_id};
            msg.data.assign(&read_buf[open_index + 2], &read_buf[close_index]);
            recv_queue_.push_back(msg);
            
            /* Set this msg data to be removed. */
            last_processed_index = close_index;

            /* Reset message indexes. */
            state = ParseState::OPEN;
            open_index = -1;
            close_index = -1;
        }
    }

    /* Remove parsed messages from buffer. */
    int num_processed_bytes = last_processed_index + 1;
    int num_unprocessed_bytes = last_filled_index - last_processed_index;
    // LOGI("Processed: %d, Unprocessed: %d", num_processed_bytes, num_unprocessed_bytes);
    memmove(&read_buf[0], &read_buf[last_processed_index + 1], (num_unprocessed_bytes)*sizeof(uint8_t));  // ERROR: Overflow
    last_filled_index -= num_processed_bytes; 

    /* Prevent garbage data from accumilating. */
    if (last_filled_index > 24) {
        last_filled_index = -1;
    }
    
    return;
}

void ArduinoSocket::send(arduino::Message msg) {
    /* Create Message Byte Buffer. */
    std::vector<uint8_t> msg_bytes = std::vector<uint8_t>(msg.data.size() + 3);
    msg_bytes.front()   = static_cast<uint8_t>('<');
    msg_bytes[1]        = static_cast<uint8_t>(msg.id);
    for (size_t i = 0; i < msg.data.size(); i++) {
        msg_bytes[2 + i] = msg.data[i];
    }
    msg_bytes.back()    = static_cast<uint8_t>('>');

    /* Write bytes over serial port. */
    /* Note: This write() is thread-safe, no mutex required (could result in deadlock). */
    int num_bytes = write(fd_, msg_bytes.data(), msg_bytes.size());
    // tcflush(fd_, TCOFLUSH);

    // LOGI("Sending Message: %c, data size=%d", static_cast<char>(msg.id), msg.data.size());
    // LOGI("Send Buffer: [%s]", stringify_buffer(&msg_bytes[0], msg_bytes.size()).c_str());
    if (num_bytes < 0) {
        LOGE("Writing to file descriptor issue (error %d: %s)", errno, strerror(errno));
    }
}

/**
 * @returns The oldest recieved message not yet returned.
 * @returns An empty message if no message is available.
 */
arduino::Message ArduinoSocket::getMessage() {
    if (recv_queue_.empty()) { return {}; }
    arduino::Message msg = recv_queue_.front();
    recv_queue_.pop_front();
    return msg;
}
