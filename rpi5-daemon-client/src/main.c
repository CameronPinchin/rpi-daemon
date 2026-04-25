#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <errno.h>

#include "commands.h"
#include "daemon-master.h" // external functions forward declared here

#define BUFFER_SIZE 64
#define MAXLINE 128

#define RPI_IP_ADDRESS "192.168.2.124"
#define RPI_PORT 5050

#define TEST_IP_ADDRESS "192.168.2.34" /* opts: "192.168.2.164", "192.168.89.158", "192.168.2.34"*/
#define TEST_PORT 5051

const struct timeval tv = {1, 0};

/* Initialize a connection with the RPi5
 *  - return the file descriptor
 *  - return -1 if err, set errno
 */
void send_message(char* arg_0, char* arg_1)
{
    char buffer[BUFFER_SIZE];
    char* init_message = "coldlake";
    int sockfd;
    struct sockaddr_in servaddr;
    ssize_t bytes_recvd, bytes_sent;

    memset( &servaddr, 0 , sizeof(servaddr) );
    servaddr.sin_addr.s_addr = inet_addr( TEST_IP_ADDRESS );    /* opts: RPI_IP_ADDRESS, TEST_IP_ADDRESS    */
    servaddr.sin_port = htons( TEST_PORT );                     /* opts: RPI_PORT, TEST_PORT                */
    servaddr.sin_family = AF_INET;

    if( (sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[CLIENT] CALL TO socket()             RETURNED [%d].\n", sockfd );

    if( connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[CLIENT] CALL TO connect()            RETURNED [%d].\n", 0 );

    if( setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[CLIENT] CALL TO setsockopt()         RETURNED [0].\n");


    rpi_command cmd;
    format_message( arg_0, arg_1, &cmd );
    DEBUG_PRINT("[CLIENT] {rpi_command: %#x, %#x}\n", cmd.process, cmd.action);

    if( (bytes_sent = sendto(sockfd, (const void*)&cmd, sizeof(cmd), 0, NULL, sizeof(servaddr))) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }

    DEBUG_PRINT("[CLIENT] CALL TO sendto(cmd)        RETURNED [%zd bytes transmitted].\n", bytes_sent );

    for(int i = 0; i < 5; ++i){
        if( (bytes_recvd = recvfrom( sockfd, &buffer, sizeof(buffer), 0, NULL, NULL)) == -1 ){
            fprintf(stderr, "Warning: [%s] - Could not receive message from server.\n", strerror(errno));
        } else {
            break;
        }
    }

    DEBUG_PRINT("[CLIENT] CALL TO recvfrom()        RETURNED [%zd bytes recieved].\n", bytes_recvd );
    DEBUG_PRINT("[CLIENT] INCOMING MESSAGE FROM SERVER: %d.\n", buffer[0] );

    close(sockfd);
}

int main(int argc, char** argv)
{
    if(argc < 3){
        fprintf( stderr, "[CLIENT] Error: invalid number of arguments.\n" );
        return -1;
    }
    send_message( argv[1], argv[2] );
    return 0;
}
