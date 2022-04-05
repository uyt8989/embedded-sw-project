#include "mylib.h"

int setExit(shm_in *addr, int sem_id)
{
	int ret = FALSE;
	
	semlock(sem_id);
	if (addr->key_code == BOARD_KEY_BACK)
	{
		printf("back key is pressed\n");
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
	ret = addr->exit;
	semunlock(sem_id);

	return ret;
}