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
#include <stdio.h>          // IO Declarations
#include <string.h>         // bzero()
#include <stdlib.h>         // exit()
#include <unistd.h>         // read(), white()
#include <sys/types.h>      // Syscall datatypes
#include <sys/socket.h>     // Sockets support
#include <netinet/in.h>     // Internet domain address support (sockaddr_in)


/* ========================= Functions ========================= */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(void) {
    int sockfd;     // fd of socket system
    int newsockfd;  // fd of established connection
    int portno;     // Port Number
    int n;          // Number of characters read or written
    socklen_t clilen;  // Size of the address of the client

    char buffer[256]; // Stores data read from socket

    struct sockaddr_in serv_addr, cli_addr; // internet address

    /* Creates a new socket */
    // > Address domain: AF_INET (internet domain).
    // > Socket Type: SOCK_STREAM (characters are read in a continuous stream).
    // > Protocol: 0 (Choose appropiate protocol; TCP for stream socket).
    fprintf(stderr, "Creating Socket.\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // sets all values in a buffer to zero.
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 2566;

    /* Fill in internet address */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);      // Converted port to network byte order (Big Endian)
    serv_addr.sin_addr.s_addr = INADDR_ANY;  // Set IP of this server host PC.

    /* Binds a socket to an address. */
    fprintf(stderr, "Binding Socket to address.\n");
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    /* Listen on the socket for connections. */
    // > Backlog Queue Size: 5 (the number of connections that can be waiting)
    fprintf(stderr,"Listening for client connections.\n");
    listen(sockfd, 5);

    /* Establish a connection with the client */
    clilen = sizeof(cli_addr);
    fprintf(stderr,"Waiting for a client connection.\n");
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);  // Block until a client connects to the server
    if (newsockfd < 0)
        error("ERROR on accept");

    /* Recieve socket data. */
    bzero(buffer,256);  // Initialize buffer
    n = read(newsockfd, buffer, 255);  // Block until it can read from the socket.
    if (n < 0) error("ERROR reading from socket");
    printf("Here is the message: %s\n",buffer);

    /* Send data to the client. */
    n = write(newsockfd,"I got your message\n", 18);  // Write data to the socket.
    if (n < 0) error("ERROR writing to socket");

    return 0;
}
