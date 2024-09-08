/**
 * @brief C++ stream-based wrapper around POSIX socket interface.
 * 
 * 
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// None

/* Standard C++ Libraries */
#include <streambuf>


/* ========================== Classes ========================== */

/**
 * @note only needed when I need to buffer all the data I recieve 
 * (more than the amount of already buffered data in the kernel buffers).
 * 
 * @link streambuf: https://en.cppreference.com/w/cpp/io/basic_streambuf
 */
class SocketStream: public std::streambuf {

   protected:
    std::streamsize xsputn(const char_type* s, std::streamsize n) override { 
        // TODO: fill in if needed.
        return;
    };

    int_type overflow(int_type ch) override { 
        // TODO: fill in if needed.
        return;
    }
};
