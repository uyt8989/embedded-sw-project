/*****************************************
 *
 * util functions :
 *
 * 1. Set exit flag
 * 2. Check exit flag
 * 3. Get board's current time
 *
 *****************************************/

#include "mylib.h"

// Set exit flag of shared memory
int setExit(shm_in *addr, int sem_id)
{
	semlock(sem_id, INPUT_SEMA);
	addr->exit = TRUE;
	semunlock(sem_id, INPUT_SEMA);

	return TRUE;
}

// Check exit flag of shared memory
int checkExit(shm_in *addr, int sem_id)
{
	int ret = FALSE;

	semlock(sem_id, INPUT_SEMA);
	ret = addr->exit;
	semunlock(sem_id, INPUT_SEMA);

	return ret;
}

// Return board's current time
int get_cur_time()
{
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	return 100 * tm.tm_hour + tm.tm_min;
}