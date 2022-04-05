#include "mylib.h"

int output_process(int shm_id)
{
    // Attach shared memory
    shm_out *shm_addr = (shm_out *)shmat(shm_id, (void *)0, 0);

    printf("Output process is successfully started\n");

    while (1)
    {
        sleep(1);
        //printf("outputing...\n");
        if(shm_addr->exit == TRUE) break;
    }

    printf("Output process is successfully done\n");

    shmdt(shm_addr);

    return 0;
}