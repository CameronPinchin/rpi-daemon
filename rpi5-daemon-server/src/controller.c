#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

#include "commands.h"
#include "daemon-master.h"

/* Reimplement this function with a switch-case statement. Faster than iteration due to */
/* @brief Iterates through the supported_processes array for a match to the argument.
 * @param int cmd_proc: 4-byte command provided by the user.
 * @return const char*: pointer to an entry in the supported_processes array.
 **/
const char* map_proc( int cmd_proc )
{
    switch( cmd_proc ) {
        case _PROC_SSH:
            return "ssh";
        case _PROC_TEST:
            return "test";
        default:
            return NULL;
    }
}

const char* map_cmd( int action )
{
    switch(action) {
        case _RPI5_PROC_STOP:
            return "stop";
        case _RPI5_PROC_START:
            return "start";
        case _RPI5_PROC_RESTART:
            return "restart";
        case _RPI5_PROC_STATUS:
            return "is-active";
        case _RPI5_PROC_FAILED:
            return "is-failed";
        default:
            return NULL;
    }
}

/* @brief Wrapper-like function for the system() call. [TO-DO: Combine]
 * @param const char* proc, client-provided process.
 * @return 0 on success, error other-wise.
 **/
int8_t run_command( const char* proc, const char* action )
{
    int ret;
    char cmd[256];

    snprintf(cmd, sizeof(cmd), "systemctl %s --quiet %s", action, proc);
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        return -1;
    }

    ret = (ret > 0) ? 1 : 0; // system(cmd) call returns 768 due to metadata, can either bitshift or do this
    DEBUG_PRINT("[SERVER] run_command, return value: %d\n", ret );
    return ret;
}

/* @brief Wrapper-like function for the system() call. [TO-DO: Combine]
 * @param const char* proc, client-provided process.
 * @return 0 on success, error other-wise.
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

    ret = (ret > 0) ? 1 : 0; // system(cmd) call returns 768 due to metadata, can either bitshift or do this
    DEBUG_PRINT("[SERVER] get_status, return value: %d\n", ret );
    return ret;
}

/* @brief Wrapper-like function for the system() call. [TO-DO: Combine]
 * @param const char* proc, client-provided process.
 * @return 0 on success, error other-wise.
 **/
int8_t start_service( const char* proc )
{
    int8_t ret;
    char cmd[256];
    snprintf( cmd, sizeof(cmd), "systemctl start --quiet %s", proc ); /* opts: --quiet */
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] start_service, return value: %d\n", ret );
    return ret;
}

/* @brief Wrapper-like function for the system() call. [TO-DO: Combine]
 * @param const char* proc, client-provided process.
 * @return 0 on success, error other-wise.
 **/
int8_t stop_service( const char* proc )
{
    int8_t ret;
    char cmd[256];
    snprintf( cmd, sizeof(cmd), "systemctl stop --quiet %s", proc ); /* opts: --quiet */
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] stop_service, return value: %d\n", ret );
    return ret;
}

/* @brief Wrapper-like function for the system() call. [TO-DO: Combine]
 * @param const char* proc, client-provided process.
 * @return 0 on success, error other-wise.
 **/
int8_t restart_service( const char* proc )
{
    int8_t ret;
    char cmd[256];
    snprintf( cmd, sizeof(cmd), "systemctl restart --quiet %s", proc ); /* opts: --quiet */
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] restart_service, return value: %d\n", ret );
    return ret;
}

/* @brief Wrapper-like function for the system() call. [TO-DO: Combine]
 * @param const char* proc, client-provided process.
 * @return 0 on success, error other-wise.
 **/
int8_t failed_service( const char* proc )
{
    int8_t ret;
    char cmd[256];
    snprintf( cmd, sizeof(cmd), "systemctl is-failed --quiet %s", proc ); /* opts: --quiet */
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    DEBUG_PRINT("[SERVER] restart_service, return value: %d\n", ret );
    return ret;
}


/* @brief Front-facing function for the main function.
 * @param int cmd_proc: 4-byte command provided by user, int cmd_action: 4-byte command provided by user.
 * @return 0 on success, error other-wise.
 **/
int8_t interpret_command( int cmd_proc, int cmd_action )
{
    const char* proc = map_proc( cmd_proc );
    const char* action = map_cmd( cmd_action );
    if( proc == NULL || action == NULL ){
        fprintf(stderr, "[SERVER] Error, 'proc' or 'action' is NULL. Ensure entries are valid before trying again.\n");
        return -1;
    }

    int8_t ret = run_command( proc, action );
    return ret;
}
