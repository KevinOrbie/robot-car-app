/**
 * @brief Example of a Client Program.
 * @link https://www.linuxhowtos.org/C_C++/socket.htm
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
#include <netdb.h>          // hostent


/* ========================= Functions ========================= */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(void) {
    char* hostname = "localhost";
    int portno = 2556;  // Port Number

    int sockfd;  // fd of socket system
    int n;       // Number of characters read or written

    struct sockaddr_in serv_addr;  // Address of the server to connect to
    struct hostent *server;        // Defines this host computer on the Internet

    char buffer[256];

    /* Creates a new socket */
    // > Address domain: AF_INET (internet domain).
    // > Socket Type: SOCK_STREAM (characters are read in a continuous stream).
    // > Protocol: 0 (Choose appropiate protocol; TCP for stream socket).
    fprintf(stderr, "Creating Socket.\n");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* Get hostent corresponding to hostname. */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host");
        exit(0);
    }

    /* Setup Server Address */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Establish a connection with the server. */
    fprintf(stderr, "Connecting to the Server.\n");
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    /* Writes a user message to server. */
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");
    
    /* Recieves message from server. */
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)
        error("ERROR reading from socket");
    
    printf("%s ",buffer);
    return 0;
}