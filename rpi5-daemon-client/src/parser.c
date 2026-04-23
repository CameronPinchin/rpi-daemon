#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "commands.h"
#include "daemon-master.h"

static const char* const supported_proccesses[] = { _PROC_SSHD, _PROC_SSH };
static const char* const supported_actions[] = { _ACTION_STATUS, _ACTION_START, _ACTION_STOP, _ACTION_RESTART };

int parse_arg_process( char* arg )
{
    size_t n = (sizeof(supported_proccesses)/sizeof(supported_proccesses[0]));
    for( size_t i = 0; i < n; ++i ){
        if(strcmp( supported_proccesses[i], arg ) == 0 ){
            /* TO-DO: map to command */
            DEBUG_PRINT("PROC-MAPPED.\n");
            return __tmp_PROC_SSH;
            // break; // valid argument detected
        }
    }
    return __tmp_PROC_SSH; // invalid argument
}

int parse_arg_action( char* arg )
{
    size_t n = (sizeof(supported_actions)/sizeof(supported_actions[0]));
    for( size_t i = 0; i < n; ++i ){
        if(strcmp( supported_actions[i], arg ) == 0 ){
            /* TO-DO: map to action */
            DEBUG_PRINT("ACTION-MAPPED.\n");
            return __tmp_ACT_START;
            // break; // valid argument detected
        }
    }
    return __tmp_ACT_START; /* opts: -1 (error) */
}
/*may need change this to mutate a ptr to a rpi_command struct */
void format_message( char* arg_0, char* arg_1, rpi_command* cmd )
{
    cmd->process = parse_arg_process( arg_0 );
    cmd->action = parse_arg_action( arg_1 );
}
