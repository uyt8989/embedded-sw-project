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
#include <linux/timer.h>
#include <linux/ioport.h>
#include <linux/version.h>
#include <linux/random.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/delay.h>
#include "maze.h"

// timer struct
static struct struct_my_timer {
	struct timer_list timer;
	int count;
};

/* Global variables */
struct struct_my_timer fnd_timer;
struct struct_my_timer dot_timer;

// maze board
maze_t maze[10][7];
unsigned char board[10];
int cur_x, cur_y, cur_time;

// register address
static unsigned char *fnd_addr;
static unsigned char *dot_addr;

// usage counter for driver
static char driver_usage = DRIVER_NOT_USED;

/* functions */
// initialize maze and make new random maze
static void init_maze(void);
// move cursor on board
static int move_maze(int);

// write value to fnd regiser
static int fnd_write(const unsigned short int value);
static int dot_write(void);
// add new timer
static void set_fnd_timer(void);
static void set_dot_timer(void)
// increase time and set new timer
static void fnd_timer_blink(unsigned long timeout);
static void dot_timer_blink(unsigned long timeout);
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
	unsigned int random, temp;

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
		
		get_random_bytes(&temp, sizeof(temp));
		temp = temp % 100;

		random = ((random + 13) * temp) >> 1;
		if ((random & 1) == 1) {
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

int move_maze(int dir) {
	bool flag = false;
	int dx[] = {-1, 0, 1, 0};
	int dy[] = {0, -1, 0, 1};
	int nx = cur_x + dx[dir]; int ny = cur_y + dy[dir];

	// invalid
	if(nx < 0 || nx >= ROW || ny < 0 || ny >= COL) {
		printk("Invalid move\n");
		return -1;
	}

	switch(dir) {
		// check wall
		case 0:	// up
			if(maze[nx][ny].wall[DOWN] != 1) flag = true;
			break;
		case 1: // right
			if(maze[cur_x][cur_y].wall[RIGHT] != 1) flag = true;
			break;
		case 2: // down
			if(maze[cur_x][cur_y].wall[DOWN] != 1) flag = true;
			break;
		case 3: // left
			if(maze[nx][ny].wall[RIGHT] != 1) flag = true;
			break;
		default:
			printk("Invalid direction\n");
			break;
	}

	// allowed
	if(flag) {
		// change current location
		cur_x = nx; cur_y = ny;
		// make footprint
		//board[cur_x] = board[cur_x] | (1 << (COL - cur_y));
		board[cur_x] = board[cur_x] | (1 << (COL - 1 - cur_y));

		dot_write();
		
		return 1;
	}
	
	printk("Blocked\n");

	// blocked
	return 0;
}

// write to FND device
static int fnd_write(const unsigned short int value) {
	outw(value, (unsigned int)fnd_addr);
	return SUCCESS;
}

// write to DOT device
static int dot_write(void) {
	int i;
	unsigned short int value = 0;
	for(i = 0; i < 10; i++) {
		value = board[i] & 0x7F;
		outw(value,(unsigned int)dot_addr + i * 2);
	}
	return SUCCESS;
}

static void set_fnd_timer(void) {
    // set next timer
    fnd_timer.timer.expires = get_jiffies_64() + HZ;
	fnd_timer.timer.data = (unsigned long)&fnd_timer;
    fnd_timer.timer.function = fnd_timer_blink;
    // add next timer
	add_timer(&fnd_timer.timer);
}

static void set_dot_timer(void) {
    // set next timer
    dot_timer.timer.expires = get_jiffies_64() + HZ/2;
	dot_timer.timer.data = (unsigned long)&dot_timer;
    dot_timer.timer.function = dot_timer_blink;
    // add next timer
	add_timer(&dot_timer.timer);
}

static void fnd_timer_blink(unsigned long timeout) {
    cur_time++;
	//printk("current time : %d\n", cur_time);
	fnd_write((unsigned short int)cur_time);
    set_fnd_timer();
    return ;
}

static void dot_timer_blink(unsigned long timeout) {
	unsigned char temp = board[cur_x];
	board[cur_x] = board[cur_x] ^ (1 << (COL - 1 - cur_y));
	dot_write();
    set_dot_timer();
    return ;
}

static int dev_driver_open(struct inode *minode, struct file *mfile) {    
    int i, j;

	if(driver_usage != DRIVER_NOT_USED) {
		printk("dev_driver is already used\n");
		return -2;
	}
	
	driver_usage = DRIVER_OPENED;
	
	// clear board
	for(i = 0; i < ROW; i++) {
		board[i] = 0;
	}
	
	// initialize board
	init_maze();
	
	// initialize status variables
	cur_time = 0;
	cur_x = 0; cur_y = 0;
			
	dot_write();

	// start timer
	set_fnd_timer();
	set_dot_timer();

	printk("%s is successfully opened\n", DEV_DRIVER_NAME);

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

	return 0;
}

static int dev_driver_release(struct inode *minode, struct file *mfile) {
	int i;

	// clear variables
	driver_usage = DRIVER_NOT_USED;

	// clear board
	for(i = 0; i < ROW; i++) {
		board[i] = 0;
	}

	dot_write();

	fnd_write(0);

	// delete timer
	del_timer_sync(&fnd_timer.timer);
	del_timer_sync(&dot_timer.timer);

	printk("%s is released\n", DEV_DRIVER_NAME);

	return 0;
}

static long dev_driver_ioctl(struct file *mfile, 
			unsigned int ioctl_num, unsigned long ioctl_param) {
	int i, j, dir, result, ret = SUCCESS;

	switch(ioctl_num) {
		// invalid ioctl command
		case IOCTL_MOVE:
			result = copy_from_user(&dir, (void __user *)ioctl_param, sizeof(int));
			if(result) {
				printk("Set option error\n");
				return -EFAULT;
			}

			printk("cur %d %d dir : %d\n", cur_x, cur_y, dir);

			ret = move_maze(dir);

			break;
		default:
			printk("Invalid ioctl option\n");
			return -EFAULT;
	}

	return ret;
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
	fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
	dot_addr = ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
    
	// initialize timer
	init_timer(&(fnd_timer.timer));
	init_timer(&(dot_timer.timer));
	
    return 0;
}

void __exit dev_driver_exit(void)
{
	// release usage counter
	driver_usage = DRIVER_NOT_USED;

	// delete timer
	del_timer_sync(&fnd_timer.timer);
	del_timer_sync(&dot_timer.timer);

	// unmap register's physical address
	iounmap(fnd_addr);
	iounmap(dot_addr);

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