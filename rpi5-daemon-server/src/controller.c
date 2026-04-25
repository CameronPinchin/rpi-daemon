#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>

#include "commands.h"
#include "daemon-master.h"


/* There is a better call than status */
/* systemctl is-active proc */
int get_status( char* proc )
{
    /* Need to return status for a given process */
    int ret, str_ret;
    char cmd[256];
    /* cmd: systemctl status proc | nc -u <ip> <port>  */
    /* allows me to pipe some of the output of the command to return the status */
    /*  at the moment, current implementation is problematic: causes repeated creation and termination of threads */
    /*   reason is because the client doesnt have the port open: its UDP, a one-way datagram   */
    /*   - realistically, we just need a '1' or '0' if it is active or not */

    str_ret = snprintf(cmd, sizeof(cmd), "systemctl is-active %s --quiet sshd", proc);
    printf("STRRET VALUE: %d\n", str_ret);
    if( (ret = system(cmd)) == -1 ){
        fprintf(stderr, "Error: %s\n", strerror(errno));
        exit(errno);
    }
    return ret;
}
