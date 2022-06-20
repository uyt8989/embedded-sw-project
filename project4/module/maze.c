#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/random.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include "maze.h"

// maze board
maze_t maze[10][7];
unsigned char board[10];
int cur_x, cur_y;

/* functions */
// initialize maze and make new random maze
static void init_maze(void);
// write value to fnd regiser
static int fnd_write(const unsigned short int value);
static int dot_write(const char *);
// add new timer
static void set_my_timer(void);
// increase time and set new timer
static void kernel_timer_blink(unsigned long timeout);
// module functions
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

static void init_maze(void) {
	int i, j;
	unsigned int random;

	for(i = 0; i < ROW; i++) for(j = 0; j < COL; j++) {
		maze[i][j].wall[0] = 1; maze[i][j].wall[1] = 1;
	}

	get_random_bytes(&random, sizeof(random));

	// top rows
	for (i = 0; i < ROW - 1; i++) for (j = 0; j < COL; j++) {
		if (j == COL - 1) {
			maze[i][j].wall[DOWN] = 0;
			continue;
		}

		random = (random + 10) >> 1;
		if ((random & 1) == 0) {
			maze[i][j].wall[RIGHT] = 0;
		}
		else {
			maze[i][j].wall[DOWN] = 0;
		}
	}

	// bottom row
	for (j = 0; j < COL - 1; j++) {
		maze[ROW - 1][j].wall[RIGHT] = 0;
	}
}

/*
static int fnd_write(const unsigned short int value) {
	outw(value, (unsigned int)fnd_addr);
	return SUCCESS;
}

static void set_my_timer(void) {
    // set next timer
    my_timer.timer.expires = get_jiffies_64() + HZ / 10;
	my_timer.timer.data = (unsigned long)&my_timer;;
    my_timer.timer.function	= kernel_timer_blink;
    // add next timer
	add_timer(&my_timer.timer);
}

static void kernel_timer_blink(unsigned long timeout) {
    current_time++;
    set_my_timer();
    
    return ;
}
*/

static int dev_driver_open(struct inode *minode, struct file *mfile) {    
	/*
    if(stopwatch_usage != DRIVER_NOT_USED) {
		printk("dev_driver is already used\n");
		return -EBUSY;
	}
	
	stopwatch_usage = DRIVER_OPENED;
	*/
	printk("%s is successfully opened\n", DEV_DRIVER_NAME);

	return 0;
}

static int dev_driver_release(struct inode *minode, struct file *mfile) {
	// clear variables
	//stopwatch_usage = DRIVER_NOT_USED;

	printk("%s is released\n", DEV_DRIVER_NAME);

	return 0;
}

static long dev_driver_ioctl(struct file *mfile, 
			unsigned int ioctl_num, unsigned long ioctl_param) {
	int i, j;
	switch(ioctl_num) {
		case IOCTL_COMMAND:
			init_maze();

			for (j = 0; j < COL; j++) printk(" -");
			printk("\n");

			for (i = 0; i < ROW; i++) {
				if (i != 0) {
					for (j = 0; j < COL; j++) {
						printk(" ");
						if (maze[i - 1][j].wall[DOWN] == 1) printk("-");
						else printk(" ");
					}
				printk("\n");
			}

			printk("|");
			for (j = 0; j < COL; j++) {
				printk(" ");
				if (maze[i][j].wall[RIGHT] == 1) printk("|");
				else printk(" ");
			}
			printk("\n");
		}

		for (j = 0; j < COL; j++) printk(" -");
		printk("\n");
		
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
	// reigster device
	result = register_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME, &dev_driver_fops);
	// register error
	if(result < 0) {
		printk("register error %d\n", result);
		return result;
	}
	// print informations of driver
	printk("********************************************\n");
    printk("* %s module init\n", DEV_DRIVER_NAME);
    printk("* dev_file: /dev/%s, major: %d\n", DEV_DRIVER_NAME, DEV_DRIVER_MAJOR);
	printk("* you need to do mknod /dev/%s c %d 0\n", DEV_DRIVER_NAME, DEV_DRIVER_MAJOR);
	printk("********************************************\n");
	// map register's physical address
	//fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
    // initialize timer
	//init_timer(&(my_timer.timer));
	
    return 0;
}

void __exit dev_driver_exit(void)
{
	// release usage counter
	//topwatch_usage = DRIVER_NOT_USED;
	// unmap register's physical address
	//iounmap(fnd_addr);

    // unregister device
	unregister_chrdev(DEV_DRIVER_MAJOR, DEV_DRIVER_NAME);

	printk("********************************************\n");
	printk("* %s module exit\n", DEV_DRIVER_NAME);
	printk("* don't forget rm /dev/%s\n", DEV_DRIVER_NAME);
	printk("********************************************\n");
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");