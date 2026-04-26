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

/* @brief Parse user-input and return the process.
 * @param const char* arg, user provided C-String to parse.
 * @return Return one of the defined processes found in commands.h.
 **/
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

/* @brief Parse user-input and return the command.
 * @param const char* arg, user provided C-String to parse.
 * @return Return one of the defined commands found in commands.h.
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

/* @brief Provides a clean front-end for my main function.
 * @param const char* arg_0, arg_1, user-provided C-Strings to forward to parsing functions, populating the rpi_command struct cmd is pointing to.
 * @return N/A
 **/
void format_message( const char* arg_0, const char* arg_1, rpi_command* cmd )
{
    cmd->process = parse_arg_process( arg_0 );
    cmd->action = parse_arg_action( arg_1 );
}
