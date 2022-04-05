/*****************************************
 *
 * Main process :
 *
 * 1. Get input from shared memory
 * 2. Process input
 * 3. Transfer result to output process
 *
 *****************************************/

#include "mylib.h"

int getKeycode(shm_in *shm_addr, int sem_id) 
{
	int value = BOARD_KEY_DEFAULT;
	semlock(sem_id);
	value = shm_addr->key_code;	
	semunlock(sem_id);
	return value;
}

int main_process(int shm_input_id, int shm_output_id)
{
	int exit = FALSE;
	int sem_id;
	int prev_key = BOARD_KEY_DEFAULT;
	int cur_key = BOARD_KEY_DEFAULT;

	// Attach shared memory
	shm_in *shm_input_addr = (shm_in *)shmat(shm_input_id, (void *)0, 0);
	shm_out *shm_output_addr = (shm_out *)shmat(shm_output_id, (void *)0, 0);

	sem_id = seminit();

	printf("Main process is successfully started\n");

	while (exit == FALSE)
	{
		sleep(1);
		printf("maining...\n");

		cur_key = getKeycode(shm_input_addr, sem_id);
		if(prev_key != cur_key) {
			switch (cur_key)
			{
			case BOARD_KEY_BACK :
				exit = setExit(shm_input_addr, sem_id);
				break;
			case BOARD_KEY_VOL_UP : 
				break;
			case BOARD_KEY_VOL_DOWN : 
				break;
			default:
				break;
			}
		}
	}

	// Send exit signal to output process
	shm_output_addr->exit = exit;
	
	// Detach shared memory
	shmdt(shm_input_addr);
	shmdt(shm_output_addr);

	printf("Main process is successfully done\n");

	return 0;
}

