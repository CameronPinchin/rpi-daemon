#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
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

static const action_entry supported_actions[] = {
    {"stop", _RPI5_PROC_STOP},
    {"start", _RPI5_PROC_START},
    {"restart", _RPI5_PROC_RESTART},
    {"status", _RPI5_PROC_STATUS}
};

/* returns a char */
int parse_arg_process( const char* arg )
{
    size_t n = (sizeof(supported_processes)/sizeof(supported_processes[0]));
    for( size_t i = 0; i < n; ++i ){
        if(strcmp( supported_processes[i].name, arg ) == 0 ){
            return supported_processes[i].id;
        }
    }
    return _RPI5_GENERIC_ERROR; // invalid argument
}

/* this works dynamically now
 *      i.e., if the user sends stop, start, status, or restart, it is reflected to the server
 **/
int parse_arg_action( const char* arg )
{
    size_t n = (sizeof(supported_actions)/sizeof(supported_actions[0]));
    for( size_t i = 0; i < n; ++i ){
        if(strcmp( supported_actions[i].name, arg ) == 0 ){
            return supported_actions[i].action;
        }
    }
    return _RPI5_GENERIC_ERROR; // invalid argument
}

void format_message( char* arg_0, char* arg_1, rpi_command* cmd )
{
    cmd->process = parse_arg_process( arg_0 );
    cmd->action = parse_arg_action( arg_1 );
}
