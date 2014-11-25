/****
 * map.h
 *
 * Mostly macros and prototypes for the map character device
 * driver.
 *
 * CREDITS:
 *   o Many parts of the driver code has to be credited to
 *     Ori Pomerantz, in his chardev.c (Copyright (C) 1998-1999)
 *
 *     Source:  The Linux Kernel Module Programming Guide (specifically,
 *              http://www.tldp.org/LDP/lkmpg/2.6/html/index.html)
 */

#ifndef _MAP_DEVICE_H
#define _MAP_DEVICE_H

/* The maximum length of the message from the device */
#define BSIZE 8192

#include <linux/ioctl.h> /* for ioctl defs */

/* For input output control */
#define MAJOR_NUM 	130
#define IOCTL_RESET_MAP 	_IO(MAJOR_NUM, 0) /*reset to the default map*/
#define IOCTL_ZERO_OUT		_IO(MAJOR_NUM, 1) /*zeros out the buffer with
							    * reseting the lengths and the pointer*/
#define IOCTL_CHECK_CONSISTENCY _IO(MAJOR_NUM, 2) /*checks that consistency*/

/* The name for our device, as it will appear
 * in /proc/devices
 */
#define DEVICE_NAME  "/dev/asciimap"


#endif /* _MAP_DEVICE_H */
