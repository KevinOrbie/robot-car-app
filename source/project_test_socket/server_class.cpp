/**
 * @brief Example of a Server Program.
 * @link https://www.linuxhowtos.org/C_C++/socket.htm
 * 
 * @note In practive:
 * - Create a function that is ran in a seperate thread, handling a single connection.
 *      > Child process (forked), closes sockfd, does stuff with newsockfd.
 *      > Parent process, closes newsockfd.
 * - Prevent child processes not closing (Zombie processes)
 *      > When a child dies, it sends a SIGCHLD signal to its parent.
 * - Datagram Sockets for less overhead (UDP)
 *      > Calls recvfrom() to read a message.
 *      > Calls sendto() to read a message.
 */

/* ========================== Include ========================== */
/* Standard C Libraries */
// #include <poll.h>           // poll()
#include <stdio.h>          // IO Declarations
#include <fcntl.h>          // fcntl()
#include <string.h>         // bzero()
#include <stdlib.h>         // exit()
#include <unistd.h>         // read(), white()
#include <sys/types.h>      // Syscall datatypes
#include <sys/socket.h>     // Sockets support
#include <netinet/in.h>     // Internet domain address support (sockaddr_in)

/* Standard C++ Libraries */
#include <string>
#include <chrono>
#include <thread>


/* ========================= Functions ========================= */
void error(char *msg) {
  perror(msg);
  exit(1);
}


/* ========================== Classes ========================== */
class Server {
   public:
    Server(int port, bool blocking): port_number(port), blocking(blocking) {
        sockaddr_in serv_addr = {};  // Server internet address

        /* Creates a new socket */
        fprintf(stderr, "Creating Socket.\n");
        socket_fd = socket(
            AF_INET,        // Address domain: AF_INET (internet domain).
            SOCK_STREAM,    // Socket Type: SOCK_STREAM (characters are read in a continuous stream).
            0               // Protocol: 0 (Choose appropiate protocol; TCP for stream socket).
        );
        if (socket_fd < 0) {
            error("ERROR opening socket");
        }

        /* Fill in internet address. */
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port_number);  // Converted port to network byte order (Big Endian)
        serv_addr.sin_addr.s_addr = INADDR_ANY;   // Set IP to the server's host IP.

        /* Binds the opened socket to an address. */
        fprintf(stderr, "Binding Socket to address.\n");
        if (bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
            error("ERROR on binding");
        }

        /* Listen on the socket for connections. */
        // > Backlog Queue Size: 5 (the number of connections that can be waiting)
        fprintf(stderr,"Listening for client connections.\n");
        listen(socket_fd, 5);
    };

    ~Server() {
        if (socket_fd >= 0) {
            close(socket_fd);
        }

        if (connection_fd >= 0) {
            close(connection_fd);
        }
    }

    void link() {
        socklen_t clilen;  // Size of the address of the client
        sockaddr_in cli_addr = {};  // Client internet address

        /* Establish a connection with a single client. */
        clilen = sizeof(cli_addr);
        fprintf(stderr,"Waiting for a client connection...");

        /* Connect to socket. */
        connection_fd = accept(socket_fd, (struct sockaddr *) &cli_addr, &clilen);  // Block until a client connects to the server
        if (connection_fd < 0) {
            error("ERROR on accept");
        }

        /* Make it socket non-blocking. */
        if (!blocking) {
            fcntl(connection_fd, F_SETFL, O_NONBLOCK);
        }

        /* We only need the established connection, delete the server socket. */
        close(socket_fd);
        socket_fd = -1;

        fprintf(stderr, " >>> Client connected: %d\n", cli_addr.sin_addr.s_addr);
    };

    /**
     * @warning: This reads all recieved message, and outputs them on after another, in the same string.
     */
    std::string recieve() {
        int chars_read = -1;
        std::string buffer = "";
        buffer.resize(255, 0);

        /* Recieve socket data (blocking wait). */
        chars_read = read(connection_fd, &buffer[0], 255);
        buffer.resize(strlen(buffer.c_str()));

        /* Check for errors. */
        if (!blocking && chars_read < 0 && ((errno & EAGAIN) || (errno & EWOULDBLOCK))){
            fprintf(stderr, "Nothing to read.\n");
            return std::string();

        } else if (chars_read < 0) {
            error("ERROR reading from socket");
        }

        return buffer;
    };

    void send(std::string msg) {
        int chars_written = -1;

        /* Send socket data. */
        chars_written = write(connection_fd, msg.c_str(), msg.size()); 
        if (chars_written < 0) error("ERROR writing to socket");

        return;
    };

   private:
    int socket_fd      = -1;
    int connection_fd  = -1;

    int port_number;
    bool blocking = false;
};


/* ========================= Entry-Point ========================= */
int main(void) {
    Server server = Server(2556, false);
    server.link();

    /* Simulate control loop. */
    while (true) {
        std::string recv_msg = server.recieve();
        if (!recv_msg.empty()) {
            fprintf(stderr, "Recieved message: '%s'\n", recv_msg.c_str());
        } else {
            fprintf(stderr, "Nothing to recieve.\n");
        }

        /* Slow down loop. */
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    // server.send("The server recieved a message!\n");
    return 0;
}
