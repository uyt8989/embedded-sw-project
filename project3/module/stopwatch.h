#ifndef __STOPWATCH_H__
#define __STOPWATCH_H__

#include <linux/ioctl.h>

/* defines */
// device driver infromation
#define DEV_DRIVER_NAME "stopwatch"
#define DEV_DRIVER_MAJOR 242
// register physical address
#define IOM_FND_ADDRESS 0x08000004
// buttons interrupt
#define HOME_KEY IMX_GPIO_NR(1,11)
#define BACK_KEY IMX_GPIO_NR(1,12)
#define VOLUP_KEY IMX_GPIO_NR(2,15)
#define VOLDOWN_KEY IMX_GPIO_NR(5,14)

#define SUCCESS 0

/* enums */
// device driver status
enum {
	DRIVER_NOT_USED, DRIVER_OPENED
};
// stopwatch status
enum {
	STOPWATCH_OFF, STOPWATCH_ON, STOPWATCH_PLAY, STOPWATCH_PAUSED
};
// terminate key status
enum {
	KEY_INIT, KEY_PRESSED
};
/* ioctl commands */
#define IOCTL_COMMAND _IO(DEV_DRIVER_MAJOR, 0)

#endif