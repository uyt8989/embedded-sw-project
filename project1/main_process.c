#include "mylib.h"

int main_process(int shm_input_id, int shm_output_id) {
    int exit = FALSE;
    int sem_id;

    //Attach shared memory
    input_shm *shm_input_addr = (input_shm *)shmat(shm_input_id, (void *)0, 0);
    output_shm *shm_output_addr = (output_shm *)shmat(shm_output_id, (void *)0, 0);

    sem_id = seminit();

    semlock(sem_id);
    // Initialize shared memories
    shm_input_addr->exit = FALSE;
    shm_input_addr->key_code = -1;
    memset(shm_input_addr->sw, 0, sizeof(unsigned char) * MAX_BUTTON);

    shm_output_addr->exit = FALSE;
    //나머지도 초기화
    semunlock(sem_id);

    printf("Main process is successfully started\n");

    while(exit == FALSE) {
        /*
        if(shm_input_addr->exit == TRUE)
            exit = TRUE;
        */
       sleep(1);
       printf("maining...\n");
    }

    shmdt((char *)shm_input_addr);
    shmdt((char *)shm_output_addr);

    printf("Main process is successfully done\n");

    return 0;
}