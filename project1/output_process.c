#include "mylib.h"

void output_process(int shm_id) {
    //Attach shared memory
    output_shm *shm_addr = (output_shm *)shmat(shm_id, (void *)0, 0);

}