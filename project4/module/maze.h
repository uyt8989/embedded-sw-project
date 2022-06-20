#ifndef __MAZE_H__
#define __MAZE_H__

#include <linux/ioctl.h>

#define SUCCESS 0

/* defines */
// device driver infromation
#define DEV_DRIVER_NAME "maze"
#define DEV_DRIVER_MAJOR 242
// register physical address
#define IOM_FND_ADDRESS 0x08000004
#define IOM_FPGA_DOT_ADDRESS 0x08000210
// maze size
#define ROW 10
#define COL 7

/* ioctl commands */
#define IOCTL_COMMAND _IO(DEV_DRIVER_MAJOR, 0)

/* enums */
enum { // device driver status
	DRIVER_NOT_USED, DRIVER_OPENED
};

enum { // maze wall direction
	RIGHT, DOWN	
};

typedef struct _MAZE {
	int wall[2];
} maze_t;

#endif