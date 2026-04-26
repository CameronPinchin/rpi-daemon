#ifndef DAEMON_MASTER_H
#define DAEMON_MASTER_H

/* File to be used by both client and server, contains
 *  struct definition
 **/

typedef struct rpi_command {
    int process;
    int action;
} rpi_command;

typedef struct client_info {
    int listenfd;
    struct sockaddr_in cliaddr;
    struct rpi_command rpi_command_client;
} client_info;

/* FORWARD DECLARATIONS */
void format_message( char* arg_0, char* arg_1, rpi_command* cmd );
void get_status( char* proc );

/* SUPPORTED PROCESSES */
// #define _PROC_SSHD          "sshd"
//#define _PROC_SSH           "ssh"

/* SUPPORTED ACTIONS */
#define _ACTION_STATUS      "status"
#define _ACTION_RESTART     "restart"
#define _ACTION_START       "start"
#define _ACTION_STOP        "stop"

/* DEBUGGING MACROS */
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#endif
