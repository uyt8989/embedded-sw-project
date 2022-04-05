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

int setExit(shm *addr, int sem_id)
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

int checkExit(shm *addr, int sem_id)
{
	int ret = FALSE;
	
	semlock(sem_id);
	ret = addr->key_code;
	semunlock(sem_id);

	return ret;
}

int main_process(int shm_id)
{
	int exit = FALSE;
	int sem_id;

	// Attach shared memory
	shm *shm_addr = (shm *)shmat(shm_id, (void *)0, 0);

	sem_id = seminit();

	printf("Main process is successfully started\n");

	while (exit == FALSE)
	{
		sleep(1);
		printf("maining...\n");

		exit = setExit(shm_addr, sem_id);

	}

	// Send exit signal to other process
	semlock(sem_id);
	shm_addr->exit = exit;
	semunlock(sem_id);

	// Detach shared memory
	shmdt(shm_addr);

	printf("Main process is successfully done\n");

	return 0;
}