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

clock_s clock_stat;
counter_s counter_stat;
text_s text_stat;
draw_s draw_stat;

// Get key input from shared memory
int getKeycode(shm_in *shm_addr, int sem_id)
{
	int value = BOARD_KEY_DEFAULT;
	semlock(sem_id, INPUT_SEMA);
	value = shm_addr->key_code;
	semunlock(sem_id, INPUT_SEMA);
	return value;
}

// Get switch input from shared memory
// Return true if there is input
int getSwitch(shm_in *shm_addr, int sem_id, unsigned char sw_buff[])
{
	int i = 0;
	int flag = FALSE;

	semlock(sem_id, INPUT_SEMA);
	for (i = 0; i < MAX_BUTTON; i++)
	{
		sw_buff[i] = shm_addr->sw[i];
		if (shm_addr->sw[i] == 1)
		{
			flag = TRUE;
			shm_addr->sw[i] = 0;
		}
	}
	semunlock(sem_id, INPUT_SEMA);

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
	unsigned char sw_buff[MAX_BUTTON];

	// Attach shared memory
	shm_in *shm_input_addr = (shm_in *)shmat(shm_input_id, (void *)0, 0);
	shm_out *shm_output_addr = (shm_out *)shmat(shm_output_id, (void *)0, 0);
	clear_out_shm(shm_output_addr);

	// Initialize semaphore
	sem_id = seminit();

	printf("Main process is successfully started\n");

	// First is Clock mode
	init_clock_mode(shm_output_addr);

	while (exit == FALSE)
	{
		usleep(250000);

		memset(sw_buff, 0, sizeof(unsigned char) * MAX_BUTTON);

		// Check key inputs
		cur_key = getKeycode(shm_input_addr, sem_id);
		if (prev_key != cur_key && cur_key != BOARD_KEY_DEFAULT)
		{
			// Mode change
			switch (cur_key)
			{
			// Set exit signal to terminate other processes
			case BOARD_KEY_BACK:
				exit = setExit(shm_input_addr, sem_id);
				break;
			// Change mode
			case BOARD_KEY_VOL_UP:
				mode_handler(shm_output_addr, sem_id, CHANGE_UP);
				break;
			case BOARD_KEY_VOL_DOWN:
				mode_handler(shm_output_addr, sem_id, CHANGE_DOWN);
				break;
			default:
				break;
			}
		}

		// Save the previous key input to change mode
		prev_key = cur_key;

		// Check the swtich inputs
		if (getSwitch(shm_input_addr, sem_id, sw_buff) == TRUE)
		{
			// And print current input
		}

		// Handle switch input according to each mode's policy
		switch (current_mode)
		{
		case MODE_1:
			clock_mode(shm_output_addr, sem_id, sw_buff);
			break;
		case MODE_2:
			counter_mode(shm_output_addr, sem_id, sw_buff);
			break;
		case MODE_3:
			text_editor_mode(shm_output_addr, sem_id, sw_buff);
			break;
		case MODE_4:
			draw_board_mode(shm_output_addr, sem_id, sw_buff);
			break;
		default:
			break;
		}
	}

	// Detach shared memory
	shmdt(shm_input_addr);
	shmdt(shm_output_addr);

	printf("Main process is successfully done\n");

	return 0;
}

// Set shared memory's FND part
void setFnd(shm_out *shm_addr, int value, int digit)
{
	int i;
	shm_addr->fnd = value;
	for (i = MAX_DIGIT - 1; i >= 0; i--)
	{
		shm_addr->digit[i] = value % digit;
		value /= digit;
	}
}

// Set shared memory's Dot part
void setDot(shm_out *shm_addr, unsigned char *value)
{
	int i;
	for (i = 0; i < MAX_DOT_BUFF; i++)
		shm_addr->dot[i] = value[i];
}

// Set shared memory's LCD part
void setLcd(shm_out *shm_addr, char *value)
{
	int i;
	for (i = 0; i < MAX_LCD_BUFF; i++)
		shm_addr->lcd[i] = value[i];
}

// Set shared memory's LED part
void setLed(shm_out *shm_addr, unsigned char value)
{
	shm_addr->led = value;
}