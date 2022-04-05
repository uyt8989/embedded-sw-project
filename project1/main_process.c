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

int setExit(shm_in *addr, int sem_id)
{
	int ret = FALSE;
	
	semlock(sem_id);
	if (addr->key_code == BOARD_KEY_BACK)
	{
		ret = TRUE;
		addr->exit = TRUE;
	}
	semunlock(sem_id);

	return ret;
}

int checkExit(shm_in *addr, int sem_id)
{
	int ret = FALSE;
	
	semlock(sem_id);
	ret = addr->key_code;
	semunlock(sem_id);

	return ret;
}

int main_process(int shm_input_id, int shm_output_id)
{
	int exit = FALSE;
	int sem_id;

	// Attach shared memory
	shm_in *shm_input_addr = (shm_in *)shmat(shm_input_id, (void *)0, 0);
	shm_out *shm_output_addr = (shm_out *)shmat(shm_output_id, (void *)0, 0);

	sem_id = seminit();

	printf("Main process is successfully started\n");

	while (exit == FALSE)
	{
		sleep(1);
		printf("maining...\n");

		exit = setExit(shm_input_addr, sem_id);
	}

	// Send exit signal to output process
	shm_output_addr->exit = exit;
	
	// Detach shared memory
	shmdt(shm_input_addr);
	shmdt(shm_output_addr);

	printf("Main process is successfully done\n");

	return 0;
}