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
#include "dot_font.h"
#include "stopwatch.h"

/* functions */
static int fnd_write(const unsigned short int value);
//static void update_device(struct work_struct *work);
//static void set_my_timer(struct work_struct* work)
static void kernel_timer_blink(unsigned long timeout);
irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_volup(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg);
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

// usage counter for driver
static char stopwatch_usage = DRIVER_NOT_USED;
static char stopwatch_on = STOPWATCH_OFF;
static char stopwatch_play = STOPWATCH_PAUSED;
// register address
static unsigned char *fnd_addr;
static unsigned int current_time;
// wait queue
wait_queue_head_t my_waitq;
DECLARE_WAIT_QUEUE_HEAD(my_waitq);
// work queue
static struct workqueue_struct *my_workq;
// timer
struct struct_my_timer my_timer;

// write to FND device
static int fnd_write(const unsigned short int value) {
	outw(value, (unsigned int)fnd_addr);
	return SUCCESS;
}

/*
// update device (bottom half)
static void update_device(struct work_struct* work) {
    unsigned short int calculated_time;
    unsigned int min, sec;
    min = (current_time / 10) / 60; sec = (current_time / 10) % 60;
    calculated_time = (min / 10) << 12 | (min % 10) << 8 | (sec / 10) << 4 | (sec % 10);
    fnd_write(calculated_time);
    return ;
}

// set next timer (bottom half)
static void set_my_timer(struct work_struct* work) {
    // set next timer
    my_timer.timer.expires = get_jiffies_64() + HZ / 10;
	my_timer.timer.data = (unsigned long)&my_timer;;
    my_timer.timer.function	= kernel_timer_blink;

    // add first timer
	add_timer(&my_timer.timer);
}
*/

// update device (bottom half)
static void update_device() {
    unsigned short int calculated_time;
    unsigned int min, sec;
    min = (current_time / 10) / 60; sec = (current_time / 10) % 60;
    calculated_time = (min / 10) << 12 | (min % 10) << 8 | (sec / 10) << 4 | (sec % 10);
    fnd_write(calculated_time);
    return ;
}

// set next timer (bottom half)
static void set_my_timer() {
    // set next timer
    my_timer.timer.expires = get_jiffies_64() + HZ / 10;
	my_timer.timer.data = (unsigned long)&my_timer;;
    my_timer.timer.function	= kernel_timer_blink;

    // add first timer
	add_timer(&my_timer.timer);
}

static void kernel_timer_blink(unsigned long timeout) {
    printk("blink\n");
    // top half
    current_time++;

    update_device();
    set_my_timer();

/*
    // bottom half
    struct work_struct work;
    INIT_WORK(&work, update_device);
    queue_work(my_workq, &work);
    INIT_WORK(&work, set_my_timer);
    queue_work(my_workq, &work);
*/
    return ;
}

irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg) {
	// top half
    printk("home key\n");
    
    if(stopwatch_on == STOPWATCH_ON) {
        printk("Stopwatch is already excuted\n");
        return IRQ_HANDLED;
    }
    stopwatch_on = STOPWATCH_ON;
    stopwatch_play = STOPWATCH_PLAY;

    set_my_timer();
/*
    my_timer.timer.expires = get_jiffies_64() + HZ / 10;
	my_timer.timer.data = (unsigned long)&my_timer;;
    my_timer.timer.function	= kernel_timer_blink;

    // add first timer
	add_timer(&my_timer.timer);
*/
    printk("Start stopwatch\n");

/*
    // bottom half
    struct work_struct work;
    INIT_WORK(&work, set_my_timer);
    queue_work(my_workq, &work);
*/
	return IRQ_HANDLED;
}

irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg) {
    printk("back key\n");
    printk("pause stopwatch\n");

    if(stopwatch_on == STOPWATCH_OFF) {
        printk("You should press Home button first\n");
        return IRQ_HANDLED;
    }

    if(stopwatch_play == STOPWATCH_PLAY) {
        // top half
        stopwatch_play = STOPWATCH_PAUSED;
        // erase next timer
        del_timer_sync(&my_timer);
    }

    else if(stopwatch_play == STOPWATCH_PAUSED) {
        // top half
        stopwatch_play = STOPWATCH_PLAY;
        set_my_timer();
        /*
        // bottom half
        struct work_struct work;
        INIT_WORK(&work, set_my_timer);
        queue_work(my_workq, &work);
        */
    }

/*
    if(stopwatch_play == STOPWATCH_PLAY) {
        // top half
        stopwatch_play = STOPWATCH_PAUSED;
        // erase next timer
        del_timer_sync(&my_timer);
    }

    else if(stopwatch_play == STOPWATCH_PAUSED) {
        // top half
        stopwatch_play = STOPWATCH_PLAY;
        // bottom half
        struct work_struct work;
        INIT_WORK(&work, set_my_timer);
        queue_work(my_workq, &work);
    }
*/
    return IRQ_HANDLED;
}

irqreturn_t inter_handler_volup(int irq, void* dev_id,struct pt_regs* reg) {
    // top half
    printk("volup key\n");
    if(stopwatch_on == STOPWATCH_OFF) {
        printk("You should press Home button first\n");
        return IRQ_HANDLED;
    }
    
    printk("reset stopwatch\n");
    current_time = 0;
    del_timer_sync(&my_timer);
    set_my_timer();
   
   /*
    // bottom half
    struct work_struct work;
    INIT_WORK(&work, update_device);
    queue_work(my_workq, &work);
*/
    return IRQ_HANDLED;
}

irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg) {
    // top half
    printk("voldown key\n");
    del_timer_sync(&my_timer);
    stopwatch_on = STOPWATCH_OFF;
    __wake_up(&my_waitq, 1, 1, NULL);
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

	printk("dev_driver is successfully opened\n");

	return 0;
}

static int dev_driver_release(struct inode *minode, struct file *mfile) {
	stopwatch_usage = DRIVER_NOT_USED;

    free_irq(gpio_to_irq(HOME_KEY), NULL);
	free_irq(gpio_to_irq(BACK_KEY), NULL);
	free_irq(gpio_to_irq(VOLUP_KEY), NULL);
	free_irq(gpio_to_irq(VOLDOWN_KEY), NULL);

	printk("dev_driver is released\n");

	return 0;
}

static long dev_driver_ioctl(struct file *mfile, 
			unsigned int ioctl_num, unsigned long ioctl_param) {
	int result, i;

	switch(ioctl_num) {
		case IOCTL_COMMAND:
            printk("Press Home button to start stopwatch...\n");
            if(stopwatch_on == STOPWATCH_OFF) {
                //stopwatch_on = INTERRUPT_ON;
                //current_stat = PLAY;
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
	printk("* you need to do mknod /dev/%s c %d 0\n", DEV_DRIVER_NAME, DEV_DRIVER_MAJOR);
	printk("********************************************\n");
	
	// map register's physical address
	fnd_addr = ioremap(IOM_FND_ADDRESS, 0x4);

    // initialize timer
	init_timer(&(my_timer.timer));

    // initialize time
    current_time = 0;
    update_device();

    // create work queue
    my_workq = create_workqueue("my workqueue");

	printk("init module\n");

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
    // flush
    flush_workqueue(my_workq);
    // destroy work queue
    destroy_workqueue(my_workq);

	printk("********************************************\n");
	printk("* dev_driver exit\n");
	printk("* don't forget rm /dev/dev_driver\n");
	printk("********************************************\n");
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);

MODULE_LICENSE ("GPL");
MODULE_AUTHOR ("author");