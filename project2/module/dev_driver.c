#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "dot_font.h"
#include "mydev.h"

/* functions */
static int fnd_write(const char, const char);
static int dot_write(const char *);
static int led_write(const char);
static int lcd_write(const char *);
static int device_write(const int);
static void kernel_timer_blink(unsigned long);
static void handle_status();
static int dev_driver_open(struct inode *, struct file *);
static int dev_driver_release(struct inode *, struct file *);
static long dev_driver_ioctl(struct file *, unsigned int, unsigned long);

/* structs */
static struct file_operations dev_driver_fops =
{   
    .open = dev_driver_open,
    .release = dev_driver_release,
	.unlocked_ioctl = dev_driver_ioctl
};

// timer struct
static struct struct_my_timer {
	struct timer_list timer;
	int count;
};

/* Global variables */
struct struct_my_timer my_timer;

// usage counter for driver
static char dev_driver_usage = DRIVER_NOT_USED;

// register address
static unsigned char *fnd_addr;
static unsigned char *dot_addr;
static unsigned char *led_addr;
static unsigned char *lcd_addr;

// data from 
static struct my_struct my_data;
// LCD text
static char text[33];
// current status
static int num, pos, id_dir, name_dir;
// HZ value
static unsigned long user_HZ;
// flags
static char flag[8];

// write to FND device
static int fnd_write(const char number, const char position) {
	unsigned short int value = 0;
	value = value | number << (position * 4);
	outw(value, (unsigned int)fnd_addr);
	return SUCCESS;
}

// write to DOT device
static int dot_write(const char *data) {
	int i;
	unsigned short int value = 0;
	for(i = 0; i < 10; i++) {
		value = data[i] & 0x7F;
		outw(value,(unsigned int)dot_addr + i * 2);
	}
	return SUCCESS;
}

// write to LED device
static int led_write(const char number) {
	unsigned short int value = 0;
	if(number != 0)
		value = value | ( 1 << (8 - number));
	outw(value, (unsigned int)led_addr);
	return SUCCESS;
}

// write to LCD device
static int lcd_write(const char *data) {
	int i;
	unsigned short int value = 0;
	for(i = 0; i < 32; i++) {
		value = (data[i] & 0xFF) << 8 | data[i + 1] & 0xFF;
		outw(value, (unsigned int)lcd_addr + (i++));
	}
	return SUCCESS;
}

// write to device according to signal argument
static int device_write(const int sig) {
	// print current status
	if(sig == PRINT_STATE) {
		fnd_write(num, pos);
		dot_write(&fpga_number[num][0]);
		led_write(num);
		lcd_write(text);
	}
	// clear device
	else if (sig == TURN_OFF){
		fnd_write(0, 0);
		dot_write(fpga_set_blank);
		led_write(0);
		lcd_write(text);
	}

	return SUCCESS;
}

// timer function
static void kernel_timer_blink(unsigned long timeout) {
	struct struct_my_timer *p_data = (struct struct_my_timer*)timeout;
	int i;

	// print remain iterations
	if(p_data->count % 5 == 0 || p_data->count <= 10)
		printk("Remain iterations %d\n", p_data->count);

	// decrease current counter
	p_data->count--;

	// end excution
	if( p_data->count < 0 ) {
		printk("Execution is ended\n");

		// clear lcd text
		for(i = 0; i < 32; i++) {
			text[i] = ' ';
		}

		// clear device
		device_write(TURN_OFF);
		return;
	}

	// change current status to next status
	handle_status();
	// write to device
	device_write(PRINT_STATE);

	// set next timer
	my_timer.timer.expires = get_jiffies_64() + user_HZ;
	my_timer.timer.data = (unsigned long)&my_timer;
	my_timer.timer.function = kernel_timer_blink;
	
	// add next timer
	add_timer(&my_timer.timer);
}

static void handle_status() {
	int i;

	// increase number
	if(++num == 9) num = 1;

	// change position
	if(flag[num - 1] == 0) {
		flag[num - 1] = 1;
	}
	else {
		if(pos == 0) {
			pos = 3;
		}
		else {
			pos--;
		}
		
		// reset flags
		for(i = 0; i < 8; i++) {
			flag[i] = 0;
		}
		flag[num - 1] = 1;
	}

	// move id
	switch(id_dir) {
	case MOVE_RIGHT: // move id right
		for(i = 15; i > 0; i--)
			text[i] = text[i - 1];
		text[0] = ' ';
		// chage direction
		if(text[15] == '9') id_dir = MOVE_LEFT;
		break;
	case MOVE_LEFT: // move id left
		for(i = 0; i < 15; i++) 
			text[i] = text[i + 1];
		text[15] = ' ';
		// chage direction
		if(text[0] == '2') id_dir = MOVE_RIGHT;
		break;
	}
	
	// move name
	switch (name_dir) {
	case MOVE_RIGHT: // move name right
		for(i = 31; i > 16; i--)
			text[i] = text[i - 1];
		text[16] = ' ';
		// chage direction
		if(text[31] == 'n') name_dir = MOVE_LEFT;
		break;
	case MOVE_LEFT:  // move name left 
		for(i = 16; i < 31; i++) 
			text[i] = text[i + 1];
		text[31] = ' ';
		// chage direction
		if(text[16] == 'Y') name_dir = MOVE_RIGHT;
	}
}

static int dev_driver_open(struct inode *minode, struct file *mfile) {
	if(dev_driver_usage != DRIVER_NOT_USED) {
		printk("dev_driver is already used\n");
		return -EBUSY;
	}
	
	dev_driver_usage = DRIVER_OPENED;
	
	printk("dev_driver is successfully opened\n");

	return 0;
}

static int dev_driver_release(struct inode *minode, struct file *mfile) {
	dev_driver_usage = DRIVER_NOT_USED;

	printk("dev_driver is released");

	return 0;
}

static long dev_driver_ioctl(struct file *mfile, 
			unsigned int ioctl_num, unsigned long ioctl_param) {
	int result, i;

	switch(ioctl_num) {
		// set options
		case IOCTL_SET_OPTION: 
			result = copy_from_user(&my_data, (void __user *)ioctl_param, sizeof(struct my_struct));
			if(result) {
				printk("Set option error\n");
				return -EFAULT;
			}

			// initialize options
			num = my_data.num;
			pos = my_data.pos;
			user_HZ = my_data.interval * HZ / 10;
			
			// initialize flags
			for(i = 0; i < 8; i++) {
				flag[i] = 0;
			}
			flag[num - 1] = 1;

			// initialize text lcd
			for(i = 0; i < 32; i++) {
				text[i] = ' ';
			}
			for(i = 0; i < strlen(my_id); i++) {
				text[i] = my_id[i];
			}
			for(i = 16; i < strlen(my_name) + 16; i++) {
				text[i] = my_name[i - 16];
			}
			// initialize text direction
			id_dir = MOVE_RIGHT; name_dir = MOVE_RIGHT;

			// set initial state of device
			device_write(PRINT_STATE);

			break;

		// execute device
		case IOCTL_COMMAND:
			printk("Start iterations\n");
			
			// set first timer
			del_timer_sync(&my_timer.timer);

			my_timer.count = my_data.cnt - 1;

			// set first timer
			my_timer.timer.expires = get_jiffies_64() + user_HZ;
			my_timer.timer.data = (unsigned long)&my_timer;
			my_timer.timer.function	= kernel_timer_blink;

			// add first timer
			add_timer(&my_timer.timer);
			
			break;

		// invalid ioctl command
		default:
			printk("Invalid ioctl option\n");
			return -EFAULT;
	}

	return SUCCESS;
}

int __init dev_driver_init(void)
{
	int result;

	printk("dev_driver init\n");

	// reigster device
	result = register_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME, &dev_driver_fops);

	// register error
	if(result < 0) {
		printk("register error %d\n", result);
		return result;
	}

	// print informations of driver
	printk("********************************************\n");
    printk("* dev_file: /dev/%s, major: %d\n", DEV_DRIVER_NAME, DEV_DRIVER_MAJOR);
	printk("* you need to do mknod /dev/dev_driver c 242 0\n");
	printk("********************************************\n");
	
	// map register's physical address
	fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
	led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
	lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);

	// initialize timer
	init_timer(&(my_timer.timer));

	printk("init module\n");

	return 0;
}

void __exit dev_driver_exit(void)
{
	// release usage counter
	dev_driver_usage = DRIVER_NOT_USED;

	del_timer_sync(&my_timer.timer);

	// unmap register's physical address
	iounmap(fnd_addr);
	iounmap(dot_addr);
	iounmap(led_addr);
	iounmap(lcd_addr);

	unregister_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME);
	printk("********************************************\n");
	printk("* dev_driver exit\n");
	printk("* don't forget rm /dev/dev_driver\n");
	printk("********************************************\n");
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");