#ifndef DAEMON_MASTER_H
#define DAEMON_MASTER_H

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
void format_message( const char* arg_0, const char* arg_1, rpi_command* cmd );
int8_t interpret_command( int cmd_proc, int cmd_action );

/* DEBUGGING MACROS */
#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...)
#endif

#endif
