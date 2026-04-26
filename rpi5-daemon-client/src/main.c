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

/* @brief Based on the process, action, and response from server, print the result.
 * @param char* proc: user-provided process, int action: user-provided action, int8_t result: response from server.
 * @return N/A.
 **/
void interpret_results( char* proc, int action, int8_t result)
{
    fprintf(stderr, "==================================================\n");
    switch (action) {
        case _RPI5_PROC_STOP:
            fprintf(stderr, "The STOP call to %s.service returned with %d\n", proc, result);
            fprintf(stderr, "Result-Table: {0: success, 1: failure}\n");
            break;
        case _RPI5_PROC_START:
            fprintf(stderr, "The START call to %s.service returned with %d\n", proc, result);
            fprintf(stderr, "Result-Table: {0: success, 1: failure}\n");
            break;
        case _RPI5_PROC_RESTART:
            fprintf(stderr, "The RESTART call to %s.service returned with %d\n", proc, result);
            fprintf(stderr, "Result-Table: {0: success, 1: failure}\n");
            break;
        case _RPI5_PROC_STATUS:
            fprintf(stderr, "The STATUS call to %s.service returned with %d\n", proc, result);
            fprintf(stderr, "Result-Table: {0: active, 1: in-active}\n");
            break;
        default:
    }
    fprintf(stderr, "==================================================\n");
}

/* @brief Connect to open socket based on <target-ip>, ready the message, and send datagram.
 * @param const char* arg, user provided C-String to send to socket.
 * @return N/A. Errno printed on failure.
 **/
void send_message(char* arg_0, char* arg_1)
{
    char buffer[BUFFER_SIZE];
    //char* init_message = "coldlake";
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
    DEBUG_PRINT("[CLIENT] Call to socket succeeded.\n");

    if( connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[CLIENT] Call to connect succeeded.\n");

    if( setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[CLIENT] Call to setsockopt succeeded.\n");

    rpi_command cmd;
    format_message( arg_0, arg_1, &cmd );
    DEBUG_PRINT("[CLIENT] {rpi_command: %04x, %04x}\n", cmd.process, cmd.action);

    if( (bytes_sent = sendto(sockfd, (const void*)&cmd, sizeof(cmd), 0, NULL, sizeof(servaddr))) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[CLIENT] Call to sendto successfully transmitted %zd bytes.\n", bytes_sent );

    for(int i = 0; i < 5; ++i){
        if( (bytes_recvd = recvfrom( sockfd, &buffer, sizeof(buffer), 0, NULL, NULL)) == -1 ){
            fprintf(stderr, "Warning: [%s] - Could not receive message from server.\n", strerror(errno));
        } else {
            break;
        }
    }

    DEBUG_PRINT("[CLIENT] Call to recvfrom successfully returned %zd bytes from the server.\n", bytes_recvd );
    DEBUG_PRINT("[CLIENT] Message contents: %d.\n", buffer[0] );
    interpret_results( arg_0, cmd.action, buffer[0] );
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
