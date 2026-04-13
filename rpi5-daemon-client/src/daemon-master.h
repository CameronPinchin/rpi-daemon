#ifndef DEAMON_MASTER_H
#define DEAMON_MASTER_H

/* File to be used by both client and server, contains
 *  struct definition 
 **/

typedef struct rpi_command {
    int proccess;
    int action;
} rpi_command;

/* FORWARD DECLARATIONS */
rpi_command format_message( char* arg_0, char* arg_1 );

/* SUPPORTED PROCESSES */
#define _PROC_SSHD          "sshd"
#define _PROC_SSH           "ssh"

/* SUPPORTED ACTIONS */
#define _ACTION_STATUS      "status"
#define _ACTION_RESTART     "restart"
#define _ACTION_START       "start"
#define _ACTION_STOP        "stop"

#endif