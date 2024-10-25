/**
 * @file arduino_socket.h
 * @brief C++ class to interface with an Arduino board over a serial interface.
 */

/* =========================== Imports =========================== */
/* Standard C Libraries */
#include <stdint.h>

/* Standard C++ Libraries */
#include <deque>
#include <mutex>
#include <string>
#include <string_view>

/* Custom C++ Code */
#include "arduino_types.h"
#include "arduino_message.h"


/* ============================ Class =========================== */

/* Custom BaudRate wrapper to not have to expose <termios.h>. */
enum class ArduinoBaudRate: int {
    B0=0, B50, B75, B110, B134, B150, B200, B300, B600, B1200, B1800,
    B2400, B4800, B9600, B19200, B38400, B115200, NUM_BAUDS
};

/**
 * @brief Arduino Serial Interface.
 * 
 * @param baudrate: The baudrate of the servial connection.
 * @param vtime: The maximum read wait time (in 0.1 seconds). 
 * @param vmin: Return as soon as vmin bytes are recieved.
 */
class ArduinoSocket {
   public:
    ArduinoSocket(ArduinoBaudRate baudrate=ArduinoBaudRate::B9600, unsigned char vtime=10, unsigned char vmin=0);
    ~ArduinoSocket();

    void flush();
    void wait();
    void recieve();
    void send(arduino::Message msg);

    arduino::Message getMessage();

   private:
    std::deque<arduino::Message> recv_queue_ = {};
    std::mutex socket_mutex_;
    int fd_;
};
