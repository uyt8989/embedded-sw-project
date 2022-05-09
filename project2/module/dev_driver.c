#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "dot_font.h"
#include "mydev.h"

/* functions */

static int fnd_write(const char *);
static int dot_write(const char *);
static int led_write(const char *);
static int lcd_write(const char *);
static void handle_devie(unsigned long);
static void change_status();
static int dev_driver_open(struct inode *, struct file *);
static int dev_driver_release(struct inode *, struct file *);
static long dev_driver_ioctl(struct file *, unsigned int, unsinged long);

/* structs */

static struct file_operations dev_driver_fops =
{   
    .open = dev_driver_open,
    .release = dev_driver_release,
	.unlocked_ioctl = dev_driver_ioctl
};

static struct struct_my_timer {
	struct timer_list timer;
	int cnt;
};

/* Global variables */

// usage counter for driver
static char dev_driver_usage = ATOMIC_INIT(DRIVER_NOT_USED);

// register address
static unsigned char *fnd_addr;
static unsigned char *dot_addr;
static unsigned char *led_addr;
static unsigned char *lcd_addr;

static struct my_struct data;
struct struct struct_my_timer my_timer;

static int fnd_write(const char *data) {
	unsigned short int value = 0;
	value = data[0] << 12 | data[1] << 8 | data[2] << 4 | data[3];
	outw(value, (unsinged int)fnd_addr);
	return SUCCESS;
}

static int dot_write(const char *data) {
	int i;
	unsigned short int value = 0;
	for(i = 0; i < 10; i++) {
		value = data[i] & 0x7F;
		outw(value,(unsigned int)dot_addr+i*2);
	}
	return SUCCESS;
}

static int led_write(const char *data) {
	unsigned short int value = 0;
	value = (unsigned short int)data;
	outw(value, (unsigned int)led_addr);
	return SUCCESS;
}

static int lcd_write(const char *data) {
	int i;
	unsigned short int value = 0;
	for(i = 0; i < 32; i++) {
		value = (data[i] & 0xFF) << 8 | value[i + 1] & 0xFF;
		outw(value,(unsigned int)lcd_addr+(i++));
	}
	return SUCCESS;
}

static void handle_device(unsigned long timeout) {
	struct struct_my_timer *p_data = (struct_my_timer *)timeout;

	printk("Remain operations : %d\n", p_data->cnt);

	if(--(p_data->cnt) < 0) {
		printk("Excution is finished.\n");
		return;
	}

	change_status();

	my_timer.timer.expires = get_jiffies_64() + (data.interval * HZ / 10);
	my_timer.timer.data = (unsigned long)&my_timer;
	my_timer.timer.function = handle_device;
	
	add_timer(&my_timer.timer);
}

static void change_status() {

}

static int dev_driver_open(struct inode *minode, struct file *mfile) {
	if(atomic_cmpxchg(&dev_driver_usage, DRIVER_NOT_USED, DRIVER_OPENED)) {
		printk("dev_driver is already used\n");
		return -EBUSY;
	}
	
	printk("dev_driver is successfully opened\n");

	return SUCCESS;
}

static int dev_driver_release(struct inode *minode, struct file *mfile) {
	atomic_set(&dev_driver_usage, DRIVER_NOT_USED);
	
	printk("dev_driver is released");

	return SUCCESS;
}

static long dev_driver_ioctl(struct file *mfile, 
			unsigned int ioctl_num, unsinged long ioctl_param) {
	int result;

	switch(ioctl_num) {
		// set options
		case IOCTL_SET_OPTION: 
			result = copy_from_user(&data, (void __user *)ioctl_param, sizeof(my_struct));
			if(result) {
				printk("Set option error\n");
				return -EFAULT;
			}
			del_timer_sync(&my_timer.timer);
			my_timer.timer.expires = jiffies + (data.interval * HZ / 10);
			my_timer.timer.data = (unsigned long)&my_timer;
			my_timer.timer.function = handle_device;
			my_timer.cnt = data.cnt;
			break;
		// execute device
		case IOCTL_COMMAND:
			printk("Execute device\n");
			add_timer(&my_timer.timer);
			break;
		default:
			printk("Invalid ioctl option\n");
			return -EFAULT;
	}
}

int __init dev_driver_init(void)
{
	int result;

	printk("dev_driver_init\n");

	// reigster device
	result = register_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME, &dev_driver_fops);

	// register error
	if(result < 0) {
		printk("register error %d\n", result);
		return result;
	}

	// print informations of driver
    printk("dev_file : /dev/%s , major : %d\n", DEV_DRIVER_NAME, DEV_DRIVER_MAJOR);

	// map register's physical address
	fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
	led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
	lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);

	init_timer(&(my_timer.timer));

	printk("init module\n");

	return 0;
}

void __exit dev_driver_exit(void)
{
	// release usage counter
	atomic_set(&dev_driver_usage, DRIVER_NOT_USED);
	
	del_timer_sync(&my_timer.timer);

	// unmap register's physical address
	iounmap(fnd_addr);
	iounmap(dot_addr);
	iounmap(led_addr);
	iounmap(lcd_addr);

	unregister_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME);

	printk("dev_driver_exit\n");
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");