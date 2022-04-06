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

void key_action(int dev_key, shm_in *shm_addr, int sem_id)
{
    struct input_event ev[BUFF_SIZE];
    int key_size = sizeof(struct input_event);
    int value;

    if (read(dev_key, ev, key_size * BUFF_SIZE) < 0)
    {
        value = BOARD_KEY_DEFAULT;
    }
    else
    {
        value = ev[0].code;
        
    }

    //Accessing critical section
    semlock(sem_id);
    shm_addr->key_code = value;
    semunlock(sem_id);

    return;
}

void switch_action(int dev_sw, shm_in *shm_addr, int sem_id)
{
    unsigned char push_sw_buff[MAX_BUTTON];
    unsigned char result[MAX_BUTTON];
    int sw_size = sizeof(push_sw_buff);
    int i, j;

    memset(result, 0, sw_size);

    // Get simultaneous input
    for (i = 0; i < 10000; i++)
    {
        if (read(dev_sw, &push_sw_buff, sw_size) < 0)
        {
            printf("Switch input error\n");
            continue;
        }

        for (j = 0; j < MAX_BUTTON; j++)
        {
            result[j] = result[j] | push_sw_buff[j];
        }
    }

    // Accessing critical section
    semlock(sem_id);
    for (i = 0; i < MAX_BUTTON; i++)
        shm_addr->sw[i] = result[i];
    semunlock(sem_id);

    return;
}

int input_process(int shm_id)
{
    int dev_key, dev_sw, sem_id;
    shm_in *shm_addr;
    int exit = FALSE;

    // Open key device file
    char *key_file = "/dev/input/event0";
    if ((dev_key = open(key_file, O_RDONLY | O_NONBLOCK)) < 0)
    {
        printf("%s is not a vaild device\n", key_file);
        return -1;
    }

    // Open switch device file
    char *sw_file = "/dev/fpga_push_switch";
    if ((dev_sw = open(sw_file, O_RDONLY)) < 0)
    {
        printf("%s is not a vaild device\n", sw_file);
        return -1;
    }

    // Attach shared memory
    shm_addr = (shm_in *)shmat(shm_id, (void *)0, 0);

    // Create Semaphore
    sem_id = seminit();

    printf("Input process is successfully started\n");

    while (exit == FALSE)
    {
        usleep(200000);

        // Check terminate condition
        exit = checkExit(shm_addr, sem_id);

        // Get key input
        key_action(dev_key, shm_addr, sem_id);

        // Get switch input
        switch_action(dev_sw, shm_addr, sem_id);

        printf("switch: ");
        int i;
			for (i = 0; i < MAX_BUTTON; i++)
			{
				printf("%d ", shm_addr->sw[i]);
			}
			printf("\n");
    }

    // Detach shahred memory
    shmdt(shm_addr);

    // Close device files
    close(dev_key);
    close(dev_sw);

    printf("Input process is successfully done\n");

    return 0;
}