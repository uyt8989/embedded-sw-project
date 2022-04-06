#include "mylib.h"

int output_process(int shm_input_id, int shm_output_id)
{
    // Attach shared memory
    shm_in *shm_input_addr = (shm_in *)shmat(shm_input_id, (void *)0, 0);
    shm_out *shm_addr = (shm_out *)shmat(shm_output_id, (void *)0, 0);
    int exit = FALSE;
    int sem_id;

    printf("Output process is successfully started\n");

    sem_id = seminit();

    while (exit == FALSE)
    {   
        usleep(200000);

        // Check terminate condition
        exit = checkExit(shm_input_addr, sem_id);
    }

    printf("Output process is successfully done\n");

    shmdt(shm_addr);

    return 0;
}