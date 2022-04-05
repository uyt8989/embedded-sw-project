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

    //Initialize shared memory
    input_shm *shm_input_addr = (input_shm *)shmat(shm_input_id, (void *)0, 0);
    output_shm *shm_output_addr = (output_shm *)shmat(shm_output_id, (void *)0, 0);

    shm_input_addr->key_code = BOARD_KEY_DEFAULT;
    shm_input_addr->exit = FALSE;
    memset(shm_input_addr, 0, sizeof(unsigned char) * MAX_BUTTON);


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
        // Detach shared memory
        shmdt(shm_input_addr);
        shmdt(shm_output_addr);
        
        // Erase shared memory
        shmctl(shm_input_id, IPC_RMID, (struct shmid_ds *)NULL);
        shmctl(shm_output_id, IPC_RMID, (struct shmid_ds *)NULL);
    }

    printf("Successfully terminated(pid1=%d, pid2=%d)\n", p1, p2);

    return 0;
}