#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/ioctl.h>

#include <asm/io.h>
#include <asm/uaccess.h>

// register physical address
#define IOM_FND_ADDRESS 0x08000004
#define IOM_FPGA_DOT_ADDRESS 0x08000210
#define IOM_LED_ADDRESS 0x08000016
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090

// device driver infromation
#define DEV_DRIVER_NAME "dev_driver"
#define DEV_DRIVER_MAJOR 242

// ioctl command
#define IOCTL_SET_OPTION _IOW(DEV_DRIVER_MAJOR, 0, char *)
#define IOCTL_COMMAND _IO(DEV_DRIVER_MAJOR, 1)

#define SUCCESS 0

enum {
	DRIVER_NOT_USED, DRIVER_OPENED
}

static int fnd_write(const char *);
static int dot_write(const char *);
static int led_write(const char *);
static int lcd_write(const char *);

static int dev_driver_open(struct inode *, struct file *);
static int dev_driver_release(struct inode *, struct file *);
static long dev_driver_ioctl(struct file *, unsigned int, unsinged long);

static struct file_operations dev_driver_fops =
{   
    .open = dev_driver_open,
    .release = dev_driver_release,
	.unlocked_ioctl = dev_driver_ioctl
};

static char dev_driver_usage = ATOMIC_INIT(DRIVER_NOT_USED);
static unsigned char *fnd_addr;
static unsigned char *dot_addr;
static unsigned char *led_addr;
static unsigned char *lcd_addr;

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

static int dev_driver_open(struct inode *minode, struct file *mfile) {
	if(atomic_cmpxchg(&dev_driver_usage, DRIVER_NOT_USED, DRIVER_OPENED)) {
		printk("dev_driver is already using\n");
		return -EBUSY;
	}
	
	printk("dev_driver is successfully opened\n");

	return SUCCESS;
}


static int dev_driver_release(struct inode *minode, struct file *mfile) {
	printk("dev_driver is released");

	atomic_set(&dev_driver_usage, DRIVER_NOT_USED);

	return SUCCESS;
}

static long dev_driver_ioctl(struct file *mfile, 
			unsigned int ioctl_num, unsinged long ioctl_param) {

	switch(ioctl_num) {
		// set options
		case IOCTL_SET_OPTION: 
			
			break;
		// execute device
		case IOCTL_COMMAND:

			break;
		default:
			printk("wrong ioctl option\n");
			return -1;
	}
}

int __init dev_driver_init(void)
{
	int result;

	printk("dev_driver_init\n");

	result = register_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME, &dev_driver_fops);

	if(result < 0) {
		printk("register error %d\n", result);
		return result;
	}

    printk("dev_file : /dev/%s , major : %d\n", DEV_DRIVER_NAME, DEV_DRIVER_MAJOR);

	fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
	led_addr = ioremap(IOM_LED_ADDRESS, 0x1);
	lcd_addr = ioremap(IOM_FPGA_TEXT_LCD_ADDRESS, 0x32);

	init_timer(&(mydata.timer));

	printk("init module\n");

	return 0;
}

void __exit dev_driver_exit(void)
{
	printk("dev_driver_exit\n");

	atomic_set(&dev_driver_usage, DRIVER_NOT_USED);
	
	//del_timer_sync(&mydata.timer);

	iounmap(fnd_addr);
	iounmap(dot_addr);
	iounmap(led_addr);
	iounmap(lcd_addr);

	unregister_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME);
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");