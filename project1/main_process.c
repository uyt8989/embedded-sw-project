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

int getSwitch(shm_in *shm_addr, int sem_id)
{
	int i = 0;
	int flag = FALSE;

	semlock(sem_id);
	for (i = 0; i < MAX_BUTTON; i++)
	{
		if (shm_addr->sw[i] == 1)
		{
			flag = TRUE;
		}
	}
	semulock(sem_id);

	return flag;
}

int main_process(int shm_input_id, int shm_output_id)
{
	int exit = FALSE;
	int sem_id;
	int prev_key = BOARD_KEY_DEFAULT;
	int cur_key = BOARD_KEY_DEFAULT;
	int sw_flag = FALSE;
	int i;

	// Attach shared memory
	shm_in *shm_input_addr = (shm_in *)shmat(shm_input_id, (void *)0, 0);
	shm_out *shm_output_addr = (shm_out *)shmat(shm_output_id, (void *)0, 0);

	sem_id = seminit();

	printf("Main process is successfully started\n");

	init_device(shm_output_addr);

	while (exit == FALSE)
	{
		usleep(200000);

		// Check key inputs
		cur_key = getKeycode(shm_input_addr, sem_id);
		if (prev_key != cur_key)
		{
			printf("prev : %d     cur : %d\n", prev_key, cur_key);
			// Mode change
			switch (cur_key)
			{
			case BOARD_KEY_BACK:
				// Set exit signal to terminate other processes
				exit = setExit(shm_input_addr, sem_id);
				break;
			case BOARD_KEY_VOL_UP:
				mode_handler(shm_output_addr, CHANGE_UP);
				break;
			case BOARD_KEY_VOL_DOWN:
				mode_handler(shm_output_addr, CHANGE_DOWN);
				break;
			default:
				break;
			}
		}

		prev_key = cur_key;
		
		// Check the swtich inputs
		if (getSwitch(shm_input_addr, sem_id) == TRUE)
		{
			printf("switch: ");
			for (i = 0; i < MAX_BUTTON; i++)
			{
				printf("%d ", shm_input_addr->sw[i]);
			}
			printf("\n");


			// Handle switch input 
			switch (current_mode)
			{
			case MODE_1:
				clock_mode(shm_output_addr);
				break;
			case MODE_2:
				counter_mode(shm_output_addr);
				break;
			case MODE_3:
				text_editor_mode(shm_output_addr);
				break;
			case MODE_4:
				draw_board_mode(shm_output_addr);
				break;
			default:
				break;
			}
		}
	}

	// Detach shared memory
	shmdt(shm_input_addr);
	shmdt(shm_output_addr);

	printf("Main process is successfully done\n");

	return 0;
}