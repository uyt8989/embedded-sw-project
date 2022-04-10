/*****************************************
 *
 * Output process :
 *
 * 1. Processing outputs
 * 2. Set each devices ( FND, Dot, LCD, LED )
 *
 *****************************************/

#include "mylib.h"

extern int current_mode;

int output_process(int shm_input_id, int shm_output_id)
{
    // Attach shared memory
    shm_in *shm_input_addr = (shm_in *)shmat(shm_input_id, (void *)0, 0);
    shm_out *shm_output_addr = (shm_out *)shmat(shm_output_id, (void *)0, 0);
    int exit = FALSE, sem_id;
    int dev_fnd, dev_dot, dev_lcd, dev_mem;

    printf("Output process is successfully started\n");

    sem_id = seminit();

    // Open device files
    char *fnd_file = "/dev/fpga_fnd";
    char *dot_file = "/dev/fpga_dot";
    char *lcd_file = "/dev/fpga_text_lcd";
    char *mem_file = "/dev/mem";

    if ((dev_fnd = open(fnd_file, O_WRONLY)) < 0)
    {
        printf("%s is not a vaild device\n", fnd_file);
        return -1;
    }

    if ((dev_dot = open(dot_file, O_WRONLY)) < 0)
    {
        printf("%s is not a vaild device\n", dot_file);
        return -1;
    }

    if ((dev_lcd = open(lcd_file, O_WRONLY)) < 0)
    {
        printf("%s is not a vaild device\n", lcd_file);
        return -1;
    }

    if ((dev_mem = open(mem_file, O_RDWR | O_SYNC)) < 0)
    {
        printf("%s is not a vaild device\n", mem_file);
        return -1;
    }

    unsigned long *fpga_addr = (unsigned long *)mmap(NULL, 4096,
                            PROT_READ | PROT_WRITE, MAP_SHARED, dev_mem, FPGA_BASE_ADDRESS);

    if (fpga_addr == MAP_FAILED)
    {
        printf("mmap error!\n");
    }

    unsigned char *led_addr = (unsigned char *)((void *)fpga_addr + LED_ADDR);

    while (exit == FALSE)
    {
        usleep(200000);

        // FND
        writeToFnd(shm_output_addr, dev_fnd);
        // Dot
        writeToDot(shm_output_addr, dev_dot);
        // LCD
        writeToLcd(shm_output_addr, dev_lcd);
        // LED
        writeToLed(shm_output_addr, led_addr);

        // Check terminate condition
        exit = checkExit(shm_input_addr, sem_id);
    }

    // Clear all the devices
    clear_out_shm(shm_output_addr);
    writeToFnd(shm_output_addr, dev_fnd);
    writeToDot(shm_output_addr, dev_dot);
    writeToLcd(shm_output_addr, dev_lcd);
    writeToLed(shm_output_addr, led_addr);

    // Detach shared memory
    shmdt(shm_input_addr);
    shmdt(shm_output_addr);

    // Close device files
    munmap(led_addr, 4096);
    close(dev_fnd);
    close(dev_dot);
    close(dev_lcd);
    close(dev_mem);

    printf("Output process is successfully done\n");

    return 0;
}

// Control the devices
int writeToFnd(shm_out *shm_addr, int fd)
{
    return write(fd, shm_addr->digit, sizeof(unsigned char) * MAX_DIGIT);
}
int writeToDot(shm_out *shm_addr, int fd)
{
    return write(fd, shm_addr->dot, sizeof(unsigned char) * MAX_DOT_BUFF);
}
int writeToLcd(shm_out *shm_addr, int fd)
{
    return write(fd, shm_addr->lcd, sizeof(unsigned char) * LCD_MAX_BUFF);
}    
void writeToLed(shm_out *shm_addr, unsigned char *led_addr)
{
    *led_addr = shm_addr->led;
}