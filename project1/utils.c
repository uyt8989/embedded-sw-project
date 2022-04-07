/*****************************************
 *
 * util functions :
 * 
 * 1. Set exit flag
 * 2. Check exit flag
 *
 *****************************************/

#include "mylib.h"

int setExit(shm_in *addr, int sem_id)
{
	printf("back key is pressed\n");
	semlock(sem_id);	
	addr->exit = TRUE;
	semunlock(sem_id);

	return TRUE;
}

int checkExit(shm_in *addr, int sem_id)
{
	int ret = FALSE;
	
	semlock(sem_id);
	ret = addr->exit;
	semunlock(sem_id);

	return ret;
}

int get_cur_time () {
    time_t t = time (NULL);
    struct tm tm = *localtime (&t);

    return 100 * tm.tm_hour + tm.tm_min;
}