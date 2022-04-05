#include "mylib.h"

int main(int argc, char **argv)
{
    pid_t p1 = 0, p2 = 0;
    int process1, process2;
    int shm_id;

    // Create shared memory
    shm_id = shmget((key_t)SHARED_MEM_KEY, sizeof(shm), IPC_CREAT | 066);
    if (shm_id < 0)
    {
        printf("shmget error %d\n", shm_id);
        printf("shmget fail (errno = %s)",strerror(errno));
        exit(-1);
    }

    // Initialize shared memory
    shm *shm_addr = (shm *)shmat(shm_id, (void *)0, 0);

    shm_addr->key_code = BOARD_KEY_DEFAULT;
    shm_addr->exit = FALSE;
    memset(shm_addr, 0, sizeof(unsigned char) * MAX_BUTTON);

    p1 = fork();

    // parent process
    if (p1 > 0)
    {
        p2 = fork();

        if (p2 == 0)
        {
            printf("Calling input process\n");
            // input process
            input_process(shm_id);
        }

        else if (p2 > 0)
        {
            printf("Calling main process\n");
            // main process
            main_process(shm_id);
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
        output_process(shm_id);
    }

    else
    {
        printf("Fork Failed\n");
        exit(-1);
    }

    // Wait for child process
    wait(NULL);
    wait(NULL);

    if (p1 && p2)
    {
        // Detach shared memory
        shmdt(shm_addr);

        // Erase shared memory
        shmctl(shm_id, IPC_RMID, (struct shmid_ds *)NULL);
    }

    printf("Successfully terminated(pid1=%d, pid2=%d)\n", p1, p2);

    return 0;
}