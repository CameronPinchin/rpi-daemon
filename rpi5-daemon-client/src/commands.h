#ifndef COMMANDS_H
#define COMMANDS_H
/* RPI5 Commands 
 *    0 x [ a b c d ]
 * 		a : 	sensitivity {(0, 1)}
 *		b : 	subsystem 
 *		c : 	action
 *		d :	subsytem-service 
 * BREAKDOWN
 *  a: {0 - 1}  - command requires priviliges or not
 *  b: {0 - 9}  - subsystem to target, based on ordering here 
 *  c: {0 - 3}  - {(0, start), (1, stop), (2, restart), (3, get)} 
 *  d: {0 - n}  - subsystem service being actioned (e.g., networking, ssh) - optional
 **/
 
 /*	0. GENERAL	*/
 	#define _RPI5_PROC_START					0x1000
 	#define _RPI5_PROC_STOP						0x1010
 	
 /*	1. NETWORKING	*/
 
  	#define _RPI5_NETWORKING_START_NET			0x1100	/*	unsure how this signal would get delivered	*/
 	#define _RPI5_NETWORKING_STOP_NET			0x1110	/*	unsure how this signal would get delivered	*/
 	#define _RPI5_NETWORKING_RESTART_NET		0x1120	/*	unsure how this signal would get delivered	*/	

 	#define _RPI5_PROC_SSHD						0x0131
	
	#define _RPI5_NETWORKING_START_SSHD			0x1101	/*	MAIN TASK	*/   
 	#define _RPI5_NETWORKING_STOP_SSHD			0x1111	/*	MAIN TASK	*/	
	#define _RPI5_NETWORKING_RESTART_SSHD		0x1121	/*	MAIN TASK	*/

 /*	2. DISK	*/
 	#define _RPI5_DISK_GET_SPACE				0x0230 
 
 /*	3. MEMORY	*/
 	#define _RPI5_MEM_GET_MEMINFO				0x1330 
 	
 /*	4. CPU		*/
 	#define _RPI5_CPU_GET_CPUINFO				0x1430
 	
 /*	5. PROCESS	*/
 	#define _RPI5_CPU_GET_PROCINFO				0x1530
#endif