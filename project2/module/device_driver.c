#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/delay.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/version.h>

#define IOM_FND_ADDRESS 0x08000004
#define IOM_FPGA_DOT_ADDRESS 0x08000210
#define IOM_LED_ADDRESS 0x08000016
#define IOM_FPGA_TEXT_LCD_ADDRESS 0x08000090

#define DRIVER_NAME "dev_driver"


static struct file_operations dev_driver_fops =
{   
    .open = kernel_timer_open,
    .write = kernel_timer_write,
    .release = kernel_timer_release 
};


int __init kernel_timer_init(void)
{
	int result;


	printk("kernel_timer_init\n");


	result = register_chrdev(KERNEL_TIMER_MAJOR, KERNEL_TIMER_NAME, &kernel_timer_fops);
	if(result <0) {
		printk( "error %d\n",result);
		return result;
	}
    printk( "dev_file : /dev/%s , major : %d\n",KERNEL_TIMER_NAME,KERNEL_TIMER_MAJOR);

	init_timer(&(mydata.timer));

	printk("init module\n");
	return 0;
}

void __exit kernel_timer_exit(void)
{
	printk("kernel_timer_exit\n");
	kernel_timer_usage = 0;
	del_timer_sync(&mydata.timer);

	unregister_chrdev(KERNEL_TIMER_MAJOR, KERNEL_TIMER_NAME);
}


module_init( kernel_timer_init);
module_exit( kernel_timer_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");