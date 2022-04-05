#include "mylib.h"

int main(int argc, char **argv)
{
    pid_t p1 = 0, p2 = 0;
    int process1, process2;
    int shm_input_id, shm_output_id;
    
    // Create shared memory
    shm_input_id = shmget((key_t)SHARED_MEM_KEY_IN, sizeof(input_shm), IPC_CREAT | 0666);
    if (shm_input_id < 0)
    {
        printf("input shmget error\n");
        exit(-1);
    }

    shm_output_id = shmget((key_t)SHARED_MEM_KEY_OUT, sizeof(output_shm), IPC_CREAT | 0666);
    if (shm_output_id < 0)
    {
        printf("output shmget error\n");
        exit(-1);
    }

    p1 = fork();

    // parent process
    if (p1 > 0)
    {
        p2 = fork();

        if (p2 == 0)
        {
            printf("Calling input process\n");
            // input process
            input_process(shm_input_id);
        }

        else if (p2 > 0)
        {
            printf("Calling main process\n");
            // main process
            main_process(shm_input_id, shm_output_id);
        }

        else
        {
            printf("Fork Failed\n");
            exit(-1);
        }
    }

    // child process
    else if (p1 == 0)
    {
        printf("Calling output process\n");
        // output process
        output_process(shm_output_id);
    }

    else
    {
        printf("Fork Failed\n");
        exit(-1);
    }

    // Wait for child process
    wait(NULL);
    wait(NULL);

    if(p1 && p2) {
        // Erase shared memory
        shmctl(shm_input_id, IPC_RMID, (struct shmid_ds *)NULL);
        shmctl(shm_output_id, IPC_RMID, (struct shmid_ds *)NULL);
    }
    
    printf("Successfully terminated(pid1=%d, pid2=%d)\n", p1, p2);

    return 0;
}