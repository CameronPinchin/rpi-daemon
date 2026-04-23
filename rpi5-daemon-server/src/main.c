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

#include "whitelist.h"					/*  */
#include "commands.h"					/*  */
#include "daemon-master.h"				/*  */

#define BUFFER_SIZE 		64
#define MAXLINE 			128

#define RPI_PORT 			5050
#define TEST_PORT 			5051

static char* expected_init_message = "coldlake";
//static char* test_expected_init_message = "fail";

static const char* const whitelist_arr[] = { THINKCENTRE_IP_ADDRESS, MACBOOK_IP_ADDRESS };


/*  @brief Check if datagram contains expected initialization string.
 *  @param char* init_message is the string data contained in the datagram.
 *  @return
 **/
void check_init( char* init_message )
{
    if(strcmp(init_message, expected_init_message) != 0){ /* opts: test_expected_init_message, expected_init_message */
        fprintf( stderr, "[RPI5-DAEMON-SERVER] CALL TO check_init()             RETURNED[EXIT_FAILURE].\n");
        exit(EXIT_FAILURE);
    }
    DEBUG_PRINT("[RPI5-DAEMON-SERVER] CALL TO check_init()                      RETURNED[NULL].\n");
}

int validate_connection(struct sockaddr_in *cliaddr)
{
    char *incoming_ip_addr = inet_ntoa(cliaddr->sin_addr);
    size_t n = sizeof(whitelist_arr)/sizeof(whitelist_arr[0]);
    DEBUG_PRINT("[RPI5-DAEMON-SERVER] [DEBUG] number of elements: %zd\n", n);

    for(size_t i = 0; i < n; ++i){
        if(strcmp(whitelist_arr[i], incoming_ip_addr) == 0){
            DEBUG_PRINT("[RPI5-DAEMON-SERVER] CALL TO validate_connection()             RETURNED[0].\n");
            return 0;
        }
    }
    DEBUG_PRINT("[RPI5-DAEMON-SERVER] CALL TO validate_connection()         RETURNED[-1].\n");
    return -1;

}

/* thread-function to handle connection processing */
/* Needs to be supplied the fd and a pointer to the client sockaddr_in struct */
/* Need to malloc for the listenfd, client sockaddr_in struct*/
/* Because a threads function can only be passed literal bytes, I need to make a struct */

void* handle_connection(void *arg)
{

    char *init_reply = "pinetree";
    struct client_info *args = (struct client_info *)arg;
    struct sockaddr_in cliaddr = args->cliaddr;
    struct rpi_command rpi_command_client = args->rpi_command_client;
    int listenfd = args->listenfd;

    pid_t tid = gettid();

    char* client_ip_address = inet_ntoa(args->cliaddr.sin_addr);

    DEBUG_PRINT("[RPI5-DAEMON-SERVER] [DEBUG] CLIENT-ADDRESS: %s\n", client_ip_address);
    DEBUG_PRINT("[RPI5-DAEMON-SERVER] {rpi_command_struct received:\n   process: %#x\n    action:%#x\n", rpi_command_client.process, rpi_command_client.action);

    if( sendto(listenfd, init_reply, strlen(init_reply), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr)) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }

    get_status( "ssh" ); /* HARDCODED: NEED-TO-REMOVE */

    DEBUG_PRINT("[RPI5-DAEMON-SERVER] CALL TO sendto()                          RETURNED [%d].\n", 0 );
    DEBUG_PRINT("(THREAD %ld)[RPI5-DAEMON-SERVER] THREAD TERMINATED          	RETURNED [%d].\n", (long int)tid, 0);
    free(arg);
    pthread_exit(NULL);
}


int main(int argc, char** argv)
{
    int listenfd;
    struct sockaddr_in servaddr, cliaddr;
    pthread_t handler;
    int thread_ret;
    char buffer[BUFFER_SIZE];

    socklen_t len;
    ssize_t bytes_recvd;

    memset( &servaddr, 0, sizeof(servaddr) );
    if( (listenfd = socket(AF_INET, SOCK_DGRAM, 0 )) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[RPI5-DAEMON-SERVER] CALL TO socket()                          RETURNED [%d].\n", listenfd);

    servaddr.sin_addr.s_addr = htonl;;
    servaddr.sin_port = htons(TEST_PORT); /* opts: RPI_PORT, TEST_PORT */
    servaddr.sin_family = AF_INET;

    if( bind( listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr) ) == -1 ) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[RPI5-DAEMON-SERVER] CALL TO bind()                            RETURNED [%d].\n", 0 );


    while(1){
        struct client_info *client_info_struct = malloc(sizeof(struct client_info));
        if(client_info_struct == NULL){
            // malloc failed
        }

        client_info_struct->listenfd = listenfd;
        client_info_struct->cliaddr = cliaddr;
        if( (bytes_recvd = recvfrom(client_info_struct->listenfd, (void*)&client_info_struct->rpi_command_client, sizeof(client_info_struct->rpi_command_client), 0, (struct sockaddr *)&client_info_struct->cliaddr, &len) == -1 )){
            fprintf(stderr, "Error: %s\n", strerror(errno));
            exit(errno);
        }
        validate_connection(&client_info_struct->cliaddr);

        if( (thread_ret = pthread_create(&handler, NULL, handle_connection, client_info_struct)) != 0 ) {
            fprintf(stderr, "Error: %s\n", strerror(thread_ret));
            exit(thread_ret);
        }
        DEBUG_PRINT("[RPI5-DAEMON-SERVER] THREAD CREATED()                          RETURNED [%d].\n", thread_ret);
        pthread_detach(handler);

    }

    return 0;
}
