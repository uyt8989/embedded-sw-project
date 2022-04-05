#include "mylib.h"

int output_process(int shm_id)
{
    // Attach shared memory
    shm *shm_addr = (shm *)shmat(shm_id, (void *)0, 0);

    printf("Output process is successfully started\n");

    while (1)
    {
        sleep(1);
        printf("outputing...\n");
    }

    printf("Output process is successfully done\n");

    shmdt(shm_addr);

    return 0;
}