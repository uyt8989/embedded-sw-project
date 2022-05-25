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
#include <linux/workqueue.h>
#include <linux/version.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/gpio.h>
#include <asm/irq.h>
#include <mach/gpio.h>
#include "stopwatch.h"
#include "dot_font.h"

/* functions */
// write value to fnd regiser
static int fnd_write(const unsigned short int value);
// calculate time and call write function
static void update_device(struct work_struct *work);
// add new timer
static void set_my_timer(void);
// increase time and set new timer
static void kernel_timer_blink(unsigned long timeout);
// interrupt handler functions
irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_volup(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg);
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

// timer struct
static struct struct_my_timer {
	struct timer_list timer;
};

// wait queue
wait_queue_head_t my_waitq;
DECLARE_WAIT_QUEUE_HEAD(my_waitq);
// work queue
static DECLARE_WORK(work, update_device);

/* global variables */
// usage counter for driver
static char stopwatch_usage = DRIVER_NOT_USED;
// stopwatch status variables
static char stopwatch_on = STOPWATCH_OFF;
static char stopwatch_play = STOPWATCH_PAUSED;
static char terminate_key = KEY_INIT;
static unsigned int pressed_time = 0;
// register address
static unsigned char *fnd_addr;
// stopwatch's time
static unsigned int current_time;
// my timer
struct struct_my_timer my_timer;

static int fnd_write(const unsigned short int value) {
	outw(value, (unsigned int)fnd_addr);
	return SUCCESS;
}

static void update_device(struct work_struct* work) {
    unsigned short int calculated_time;
    unsigned int min, sec;
    if(current_time % 10 == 0 && current_time != 0) {
        printk("Tiktok\n");
    }
    // calculate time to print
    min = (current_time / 10) / 60; sec = (current_time / 10) % 60;
    calculated_time = (min / 10) << 12 | (min % 10) << 8 | (sec / 10) << 4 | (sec % 10);
    // write to fnd
    fnd_write(calculated_time);
    return ;
}

static void set_my_timer(void) {
    // set next timer
    my_timer.timer.expires = get_jiffies_64() + HZ / 10;
	my_timer.timer.data = (unsigned long)&my_timer;;
    my_timer.timer.function	= kernel_timer_blink;

    // add first timer
	add_timer(&my_timer.timer);
}

static void kernel_timer_blink(unsigned long timeout) {
    /* top half */
    current_time++;
    set_my_timer();
    /* bottom half */
    schedule_work(&work);
    return ;
}

irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg) {
    /* top half */
    if(stopwatch_on == STOPWATCH_ON) {
        printk("Stopwatch is already on\n");
        return IRQ_HANDLED;
    }
    printk("Start stopwatch\n");
    // change status variables
    stopwatch_on = STOPWATCH_ON;
    stopwatch_play = STOPWATCH_PLAY;
    // add first timer
    set_my_timer();

    /* bottom half */
    schedule_work(&work);

	return IRQ_HANDLED;
}

irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg) {
    if(stopwatch_on == STOPWATCH_OFF) {
        printk("You should press Home button first\n");
        return IRQ_HANDLED;
    }

    if(stopwatch_play == STOPWATCH_PLAY) {
        /* top half */
        printk("Pause stopwatch\n");
        // change status variables
        stopwatch_play = STOPWATCH_PAUSED;
        // erase next timer
        del_timer_sync(&my_timer.timer);
    }

    else if(stopwatch_play == STOPWATCH_PAUSED) {
        /* top half */
        printk("Replay stopwatch\n");
        // change status variables
        stopwatch_play = STOPWATCH_PLAY;
        // set new timer 
        set_my_timer();
        /* bottom half */
        schedule_work(&work);
    }

    return IRQ_HANDLED;
}

irqreturn_t inter_handler_volup(int irq, void* dev_id,struct pt_regs* reg) {
    /* top half */
    if(stopwatch_on == STOPWATCH_OFF) {
        printk("You should press Home button first\n");
        return IRQ_HANDLED;
    }
    printk("Reset stopwatch\n");
    current_time = 0;

    // if stopwatch is playing, set timer again
    if(stopwatch_play == STOPWATCH_PLAY) {
        del_timer_sync(&my_timer.timer);
        set_my_timer();
    }

    /* bottom half */
    schedule_work(&work);

    return IRQ_HANDLED;
}

irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg) {
    /* top half */
    // save the time when button is pressed
    if(terminate_key == KEY_INIT) {
        printk("You need to press for 3 seconds\n");
        terminate_key = KEY_PRESSED;
        pressed_time = get_jiffies_64();
    }
    // when button is up
    else if(terminate_key == KEY_PRESSED) {
        unsigned int temp_time = get_jiffies_64();
        // if user pressed button while more than 3 seconds, terminate program
        if((temp_time - pressed_time >= 3 * HZ)) {
            printk("Stopwatch is terminated\n");
            // delete current timer
            del_timer_sync(&my_timer.timer);
            // clear status variables
            stopwatch_on = STOPWATCH_OFF;
            current_time = 0;
            // terminate program
            __wake_up(&my_waitq, 1, 1, NULL);
            /* bottom half */
            schedule_work(&work);
        }
        // if user pressed button while less than 3 seconds, continue program
        else {
            printk("You need to press button longer to terminate\n");
            terminate_key = KEY_INIT;
            pressed_time = 0;
        }
    }

    return IRQ_HANDLED;
}

static int dev_driver_open(struct inode *minode, struct file *mfile) {
	int ret, irq;
    
    if(stopwatch_usage != DRIVER_NOT_USED) {
		printk("dev_driver is already used\n");
		return -EBUSY;
	}
	
	stopwatch_usage = DRIVER_OPENED;
	
    // home key
	gpio_direction_input(HOME_KEY);
	irq = gpio_to_irq(HOME_KEY);
	ret=request_irq(irq, inter_handler_home, IRQF_TRIGGER_FALLING, "home", 0);

	// back key
	gpio_direction_input(BACK_KEY);
	irq = gpio_to_irq(BACK_KEY);
	ret=request_irq(irq, inter_handler_back, IRQF_TRIGGER_FALLING, "back", 0);

	// volup key
	gpio_direction_input(VOLUP_KEY);
	irq = gpio_to_irq(VOLUP_KEY);
	ret=request_irq(irq, inter_handler_volup, IRQF_TRIGGER_FALLING, "volup", 0);

	// voldown key
	gpio_direction_input(VOLDOWN_KEY);
	irq = gpio_to_irq(VOLDOWN_KEY);
	ret=request_irq(irq, inter_handler_voldown, 
                    IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "voldown", 0);

	printk("%s is successfully opened\n", DEV_DRIVER_NAME);

	return 0;
}

static int dev_driver_release(struct inode *minode, struct file *mfile) {
	// clear variables
    stopwatch_usage = DRIVER_NOT_USED;

    // free irqs
    free_irq(gpio_to_irq(HOME_KEY), NULL);
	free_irq(gpio_to_irq(BACK_KEY), NULL);
	free_irq(gpio_to_irq(VOLUP_KEY), NULL);
	free_irq(gpio_to_irq(VOLDOWN_KEY), NULL);

	printk("%s is released\n", DEV_DRIVER_NAME);

	return 0;
}

static long dev_driver_ioctl(struct file *mfile, 
			unsigned int ioctl_num, unsigned long ioctl_param) {
	switch(ioctl_num) {
		case IOCTL_COMMAND:
            // initialize time
            current_time = 0;
            // initialize stopwatch's status
            stopwatch_on = STOPWATCH_OFF;
            stopwatch_play = STOPWATCH_PAUSED;
            terminate_key = KEY_INIT;
            printk("********************************************\n");
            printk("* Press Home button to start stopwatch...\n");
            printk("********************************************\n");
            if(stopwatch_on == STOPWATCH_OFF) {
                // Sleep until home button is pressed
			    interruptible_sleep_on(&my_waitq);
            }
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
	fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);
    // initialize timer
	init_timer(&(my_timer.timer));
	return 0;
}

void __exit dev_driver_exit(void)
{
	// release usage counter
	stopwatch_usage = DRIVER_NOT_USED;
	// unmap register's physical address
	iounmap(fnd_addr);
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