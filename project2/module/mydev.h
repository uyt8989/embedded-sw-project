#ifndef __MY_DEV_H__
#define __MY_DEV_H__

#include <linux/ioctl.h>

struct my_struct {
    char interval, cnt, num, pos;
};

// device driver infromation
#define DEV_DRIVER_NAME "dev_driver"
#define DEV_DRIVER_MAJOR 242

// register physical address
#define IOM_FND_ADDRESS 0x08000004
#define IOM_FPGA_DOT_ADDRESS 0x08000210
#define IOM_LED_ADDRESS 0x08000016
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090

#define SUCCESS 0

enum {
	DRIVER_NOT_USED, DRIVER_OPENED
};

enum {
	PRINT_STATE, TURN_OFF
};

const char my_id[] =   "20171659        ";
const char my_name[] = "Yoo Kyungyun    ";
const char blank_text[32] = {' ', };

// ioctl command
#define IOCTL_SET_OPTION _IOW(DEV_DRIVER_MAJOR, 0, struct my_struct)
#define IOCTL_COMMAND _IO(DEV_DRIVER_MAJOR, 1)

#endif