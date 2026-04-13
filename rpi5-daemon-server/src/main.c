#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <errno.h>

#include <pthread.h>

#include "whitelist.h"
#include "commands.h"
#include "daemon-master.h"

#define BUFFER_SIZE 64
#define MAXLINE 128

#define RPI_PORT 5050
#define TEST_PORT 5051

static char* expected_init_message = "coldlake";
//static char* test_expected_init_message = "fail";

static const char* const whitelist_arr[] = { THINKCENTRE_IP_ADDRESS, MACBOOK_IP_ADDRESS };


/*  @brief Check if datagram contains expected initialization string. 
 *  
 *  @param char* init_message is the string data contained in the datagram. 
 *
 *  @return
 **/
void check_init( char* init_message )
{
    if(strcmp(init_message, expected_init_message) != 0){ /* opts: test_expected_init_message, expected_init_message */
        fprintf( stderr, "[RPI5-DAEMON-SERVER] CALL TO check_init()             RETURNED[EXIT_FAILURE].\n");
        exit(EXIT_FAILURE);
    }
    fprintf( stderr, "[RPI5-DAEMON-SERVER] CALL TO check_init()                      RETURNED[NULL].\n");
}

int validate_connection(struct sockaddr_in *cliaddr)
{
    char *incoming_ip_addr = inet_ntoa(cliaddr->sin_addr);
    size_t n = sizeof(whitelist_arr)/sizeof(whitelist_arr[0]);
    fprintf(stderr, "[RPI5-DAEMON-SERVER] [DEBUG] number of elements: %zd\n", n);

    for(size_t i = 0; i < n; ++i){
        if(strcmp(whitelist_arr[i], incoming_ip_addr) == 0){
           fprintf( stderr, "[RPI5-DAEMON-SERVER] CALL TO validate_connection()             RETURNED[0].\n");
           return 0; 
        }
    }
    fprintf( stderr, "[RPI5-DAEMON-SERVER] CALL TO validate_connection()         RETURNED[-1].\n");
    return -1;

}

void* handle_connection(void *arg)
{
    char buffer[BUFFER_SIZE];
    struct sockaddr_in cliaddr;
    socklen_t len;
    ssize_t bytes_recvd;
    int listenfd = *(int *)arg;
    char *init_reply = "pinetree";

    pid_t tid = gettid();

    len = sizeof(cliaddr);
    if( (bytes_recvd = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len)) == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno)); 
        exit(errno);    
    }
    buffer[bytes_recvd] = '\0';
    check_init(buffer);
    validate_connection(&cliaddr);

    char* client_ip_address = inet_ntoa(cliaddr.sin_addr);
    fprintf(stderr, "[RPI5-DAEMON-SERVER] [DEBUG] CLIENT-ADDRESS: %s\n", client_ip_address);
    fprintf(stderr, "[RPI5-DAEMON-SERVER] CALL TO recvfrom()                        RETURNED [%zd bytes].\n", bytes_recvd );

    if( sendto(listenfd, init_reply, strlen(init_reply), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno)); 
        exit(errno);
    }
    fprintf(stderr, "[RPI5-DAEMON-SERVER] CALL TO sendto()                          RETURNED [%d].\n", 0 );
    fprintf(stderr, "(THREAD %ld)[RPI5-DAEMON-SERVER] THREAD TERMINATED          RETURNED [%d].\n", (long int)tid, 0 );
    free(arg); // free file descriptor 
    pthread_exit(NULL);
}

/* TO-DO: 
 * - add support for additional argument, i.e., 'status' 'ssh' would run systemctl status sshd
 * 
 **/

int main(int argc, char** argv)
{
    int listenfd;
    struct sockaddr_in servaddr;
    pthread_t handler;
    int thread_ret;

    fprintf( stderr, "[RPI5-DAEMON-SERVER] [DEBUG] A command is of size: %ld bytes.\n", sizeof(_RPI5_SYSTEM_SHUTDOWN) );

    memset( &servaddr, 0, sizeof(servaddr) );
    if( (listenfd = socket(AF_INET, SOCK_DGRAM, 0 )) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    fprintf( stderr, "[RPI5-DAEMON-SERVER] CALL TO socket()                          RETURNED [%d].\n", listenfd );

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TEST_PORT); /* opts: RPI_PORT, TEST_PORT */
    servaddr.sin_family = AF_INET;

    if( bind( listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr) ) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno)); 
        exit(errno);
    }
    fprintf( stderr, "[RPI5-DAEMON-SERVER] CALL TO bind()                            RETURNED [%d].\n", 0 );

    int *fd_ptr = malloc(sizeof(int));
    *fd_ptr = listenfd;

    if( (thread_ret = pthread_create(&handler, NULL, handle_connection, fd_ptr)) != 0 ) {
        fprintf(stderr, "Error: %s\n", strerror(thread_ret)); 
        exit(thread_ret);
    }
    fprintf( stderr, "[RPI5-DAEMON-SERVER] THREAD CREATED()                          RETURNED [%d].\n", thread_ret );

    if( (thread_ret = pthread_join(handler, NULL)) != 0 ) {
        fprintf(stderr, "Error: %s\n", strerror(thread_ret)); 
        exit(thread_ret);
    }
    // len = sizeof(cliaddr);
    // if( (bytes_recvd = recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&cliaddr, &len)) == -1) {
    //     fprintf(stderr, "Error: %s\n", strerror(errno)); 
    //     exit(errno);    
    // }
    // buffer[bytes_recvd] = '\0';
    // check_init(buffer);
    // validate_connection(&cliaddr);
    return 0;
}