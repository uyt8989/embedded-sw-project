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

extern int current_mode;

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
		//sleep(1);
		//printf("maining...\n");

		// Check key inputs
		cur_key = getKeycode(shm_input_addr, sem_id);
		if(prev_key != cur_key) {
			// Mode change
			init_board(shm_output_addr);

			switch (cur_key)
			{
			case BOARD_KEY_BACK :
				exit = setExit(shm_input_addr, sem_id);
				break;
			case BOARD_KEY_VOL_UP : 
				mode_handler(shm_output_addr, 1);
				break;
			case BOARD_KEY_VOL_DOWN :
				mode_handler(shm_output_addr, -1); 
				break;
			default:
				break;
			}
		}
		prev_key = cur_key;
		
		usleep(100000);

		//모드에 맞게 입력을 처리하고 output process로 넘긴다
		//output process에서는 디바이스 파일 열어서 디바이스 상태만 바꿔주면 됨
		switch(current_mode) {

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

void init_board(shm_out *shm_addr) {
	memset(shm_addr, 0, sizeof(shm_out));
}
