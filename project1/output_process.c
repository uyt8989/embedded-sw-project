#include "mylib.h"

int output_process(int shm_id) {
    //Attach shared memory
    output_shm *shm_addr = (output_shm *)shmat(shm_id, (void *)0, 0);


    printf("Output process is successfully started\n");



    printf("Output process is successfully done\n");

    return 0;
}