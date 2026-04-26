#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

#include "commands.h"
#include "daemon-master.h"

typedef struct {
    const char* name;
    int id;
} proc_entry;

static const proc_entry supported_processes[] = {
    {"sshd", _PROC_SSHD},
    {"test", _PROC_TEST}
};

typedef struct {
    const char* name;
    int action;
} action_entry;

/*
static const action_entry supported_actions[] = {
    {"stop", _RPI5_PROC_STOP},
    {"start", _RPI5_PROC_START},
    {"restart", _RPI5_PROC_RESTART},
    {"status", _RPI5_PROC_STATUS}
};
*/

/* map_proc accepts the int value passed from the client */
/* mapped to a process entry */
const char* map_proc( int cmd_proc )
{
    size_t n = sizeof(supported_processes)/sizeof(supported_processes[0]);
    for(size_t i = 0; i < n; ++i){
        if( supported_processes[i].id == cmd_proc ){
            return supported_processes[i].name;
        }
    }
    return "NAP";
}

/* These functions can be combined.
 *  - logically they all do the same thing, can shorten to being more generic by including the action
 *  - and creating the command with that.
 *  - for now though, this will do.
 **/
int8_t get_status( const char* proc )
{
    int ret;
    char cmd[256];

    snprintf(cmd, sizeof(cmd), "systemctl is-active --quiet %s", proc);
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return -1;
    }
    ret = (ret == 768) ? 1 : 0; // system(cmd) call returns 768 due to metadata, can either bitshift or do this
    DEBUG_PRINT("[SERVER] get_status, return value: %d\n", ret );
    return ret;
}

/* FOR DEBUGGING, DONT USE '--quiet' FLAG */
int start_service( const char* proc )
{
    int ret;
    char cmd[256];
    snprintf( cmd, sizeof(cmd), "systemctl start --quiet %s", proc ); /* opts: --quiet */
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] start_service, return value: %d\n", ret );
    return ret;
}

/* FOR DEBUGGING, DONT USE '--quiet' FLAG */
int stop_service( const char* proc )
{
    int ret;
    char cmd[256];
    snprintf( cmd, sizeof(cmd), "systemctl stop --quiet %s", proc ); /* opts: --quiet */
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] stop_service, return value: %d\n", ret );
    return ret;
}

/* FOR DEBUGGING, DONT USE '--quiet' FLAG */
int restart_service( const char* proc )
{
    int ret;
    char cmd[256];
    snprintf( cmd, sizeof(cmd), "systemctl restart --quiet %s", proc ); /* opts: --quiet */
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] restart_service, return value: %d\n", ret );
    return ret;
}


/* interpret_command accepts the int commands from client, passes on to map_proc, map_action */
int interpret_command( int cmd_proc, int cmd_action )
{
    const char* proc = map_proc( cmd_proc );
    if(strcmp(proc, "NAP") == 0){
        fprintf(stderr, "[SERVER] Error, proc mapped to NAP: Not-a-process.\n");
        return -1;
    }

    int ret = -1;
    switch(cmd_action) {
        case _RPI5_PROC_STOP:
            DEBUG_PRINT("[SERVER] Successfully mapped command to STOP, calling stop_service.\n");
            ret = stop_service(proc);
            break;
        case _RPI5_PROC_START:
            DEBUG_PRINT("[SERVER] Successfully mapped command to START, calling start_service.\n");
            ret = start_service(proc);
            break;
        case _RPI5_PROC_RESTART:
            DEBUG_PRINT("[SERVER] Successfully mapped command to RESTART, calling restart_service.\n");
            ret = restart_service(proc);
            break;
        case _RPI5_PROC_STATUS:
            DEBUG_PRINT("[SERVER] Successfully mapped command to STATUS, calling get_status.\n");
            ret = get_status(proc);
            break;
        default:
            ret = -1;
    }
    return ret;
}
