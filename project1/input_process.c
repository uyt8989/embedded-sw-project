/*****************************************
 *
 * Input process :
 *
 * 1. Get input from device key and switch
 * 2. Push inputs to main process
 *    by using shared memory and semaphore
 *
 *****************************************/

#include "mylib.h"

#define KEY_RELEASE 0
#define KEY_PRESS 1

void input_process(int shm_id)
{
    int dev_key, dev_sw, sem_id;
    input_shm *shm_addr;
    int exit = FALSE;

    // Open key device file
    char *key_file = "/dev/input/event0";
    if ((dev_key = open(key_file, O_RDONLY)) < 0)
    {
        printf("%s is not a vaild device\n", key_file);
        return ;
    }

    // Open switch device file
    char *sw_file = "/dev/fpga_push_switch";
    if ((dev_sw = open(sw_file, O_RDONLY)) < 0)
    {
        printf("%s is not a vaild device\n", sw_file);
        return ;
    }

    // Attach shared memory
    shm_addr = (input_shm *)shmat(shm_id, (void *)0, 0);

    // Create Semaphore
    sem_id = seminit();

    printf("Input process is successfully started\n");

    while (exit == FALSE)
    {
        // exit is chagned in main process
        if (shm_addr->exit == TRUE)
            exit = TRUE;
        // Key input
        key_action(dev_key, shm_addr, sem_id);
        // Switch input
        switch_action(dev_sw, shm_addr, sem_id);
    }

    // Detach shahred memory
    shmdt((char *)shm_addr);

    // Close device files
    close(dev_key);
    close(dev_sw);

    return ;
}

int key_action(int dev_key, input_shm *shm_addr, int sem_id)
{
    struct input_event ev[BUFF_SIZE];
    int key_size = sizeof(struct input_event);

    if (read(dev_key, ev, key_size * BUFF_SIZE) < 0)
    {
        printf("Key input error\n");
        return ;
    }

    // Accessing Critical Section
    semalock(sem_id);
    shm_addr->key_code = ev[0].code;
    semaunlock(sem_id);

    return ;
}

void switch_action(int dev_sw, input_shm *shm_addr, int sem_id)
{
    unsigned char push_sw_buff[MAX_BUTTON];
    unsigned char result[MAX_BUTTON];
    int sw_size = sizeof(push_sw_buff);

    memset(result, 0, sw_size);
    
    int i, j;
    // Processing when simultaneous input
    for (i = 0; i < 10000; i++)
    {
        if (read(dev_sw, &push_sw_buff, sw_size) < 0)
        {
            printf("Switch input error\n");
            return ;
        }

        for (j = 0; j < MAX_BUTTON; j++)
        {
            result[j] = result[j] | push_sw_buff[j];
        }
    }

    // Accessing ritical Section
    semalock(sem_id);
    memcpy(shm_addr->sw, result, sw_size);
    semaunlock(sem_id);

    return;
}