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

#define BUFFER_SIZE         64
#define MAXLINE             128

#define RPI_PORT            5050
#define TEST_PORT           5051

/* @brief Thread-routine for connection handling.
 * @param void *arg, pointer to a memory-address containing client information.
 * @return N/A.
 **/
void* handle_connection(void *arg)
{
    // TO-DO: Clean up variable declaration: it looks messy.
    struct client_info *args = (struct client_info *)arg;
    struct sockaddr_in cliaddr = args->cliaddr;
    struct rpi_command cmd = args->rpi_command_client;
    int listenfd = args->listenfd;
    int8_t ret = interpret_command( cmd.process, cmd.action );
    ssize_t bytes_sent;
    char* client_ip = inet_ntoa(args->cliaddr.sin_addr);
    pid_t tid = gettid();
    socklen_t cliaddr_len = sizeof(cliaddr);

    DEBUG_PRINT("(THREAD %ld)[SERVER](c:%s) {cmd received: %04x, %04x}\n", (long int)tid, client_ip, cmd.process, cmd.action);
    if( (bytes_sent = sendto(listenfd, &ret, sizeof(ret), 0, (struct sockaddr *)&cliaddr, cliaddr_len)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("(THREAD %ld)[SERVER](c:%s) Succesfully sent %zd bytes to the client.\n", (long int)tid, client_ip, bytes_sent );
    DEBUG_PRINT("(THREAD %ld)[SERVER](c:%s) Cleaning up thread.\n", (long int)tid, client_ip);
    free(arg);
    pthread_exit(NULL);
}


int main(int argc, char** argv)
{
    int listenfd;
    int thread_ret;
    struct sockaddr_in servaddr, cliaddr;
    pthread_t handler;

    // initialize sockaddr_in structs
    memset( &servaddr, 0, sizeof(servaddr) );
    memset( &cliaddr, 0, sizeof(cliaddr) );

    if( (listenfd = socket(AF_INET, SOCK_DGRAM, 0 )) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] Call to socket succeeded.\n");

    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(TEST_PORT); /* opts: RPI_PORT, TEST_PORT */
    servaddr.sin_family = AF_INET;

    if( bind( listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr) ) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] Call to bind succeeded.\n");

    while(1){
        struct client_info *client_info_struct = malloc(sizeof(struct client_info));
        socklen_t len;
        ssize_t bytes_recvd = -1;

        if(client_info_struct == NULL){
            fprintf(stderr, "Error: %s\n", strerror(errno));
            exit(errno);
        }

        client_info_struct->listenfd = listenfd;
        client_info_struct->cliaddr = cliaddr;
        len = sizeof(client_info_struct->cliaddr);

        if( (bytes_recvd = recvfrom(client_info_struct->listenfd, &client_info_struct->rpi_command_client, sizeof(client_info_struct->rpi_command_client), 0, (struct sockaddr *)&client_info_struct->cliaddr, &len)) == -1 ){
            fprintf(stderr, "Error: %s\n", strerror(errno));
            exit(errno);
        }

        if( (thread_ret = pthread_create(&handler, NULL, handle_connection, client_info_struct)) != 0 ) {
            fprintf(stderr, "Error: %s\n", strerror(thread_ret));
            exit(thread_ret);
        }
        DEBUG_PRINT("[SERVER] Successfully created thread.\n");
        pthread_detach(handler);
    }
    return 0;
}
