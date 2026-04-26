#ifndef COMMANDS_H
#define COMMANDS_H
/* RPI5 Commands
 *   Poorly designed at the moment
 *         - Make ranges:
 *       10.  Errors:         [0x9000, 0x9999]
 *        9.  Testing:        [0x8000, 0x8999]
 *        8.  range_7:        [0x7000, 0x7999]
 *        7.  range_7:        [0x6000, 0x6999]
 *        6.  range_6:        [0x5000, 0x5999]
 *        5.  range_5:        [0x4000, 0x4999]
 *        4.  range_4:        [0x3000, 0x3999]
 *        3.  range_3:        [0x2000, 0x2999]
 *        2.  PROCESSES:      [0x1000, 0x1999]
 *        1.  COMMANDS:       [0x0000, 0x0999]
 **/

/* ERRORS: [0x9000, 0x9999] */
#define _RPI5_GENERIC_ERROR                 0x9999
#define _RPI5_PROC_NOT_FOUND                0x9998
#define _RPI5_

/* TESTING [0x8000, 0x8999] */
#define _PROC_TEST                          0x8999

/* 0. COMMANDS */
#define _RPI5_PROC_STOP                     0x0000
#define _RPI5_PROC_START                    0x1000
#define _RPI5_PROC_RESTART                  0x2000
#define _RPI5_PROC_STATUS                   0x3000

/* 1. PROCESSES */
#define _PROC_SSHD                          0x0400 /* opts: temp, "sshd", perm: 0x0400*/

#endif
