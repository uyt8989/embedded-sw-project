/*****************************************
 *
 * Mode functions :
 *
 * 1. Clear all devices when mode is chaned
 * 2. Mode Handling
 * 3. Initialize shared memory to each initial state
 * 4. Process each modes
 *
 *****************************************/

#include "mylib.h"

extern int current_mode;
extern clock_s clock_stat;

void clear_out_shm(shm_out *shm_addr)
{
    memset(shm_addr, 0, sizeof(shm_out));
}

void mode_handler(shm_out *shm_addr, int d)
{
    int changed_mode = current_mode + d;

    if (changed_mode < MODE_1)
        changed_mode = MODE_4;
    else if (changed_mode > MODE_4)
        changed_mode = MODE_1;

    printf("Mode is changed to %d\n", changed_mode);

    current_mode = changed_mode;

    clear_out_shm(shm_addr);

    switch (current_mode)
    {
    case MODE_1:
        init_clock_mode(shm_addr);
        break;
    case MODE_2:
        init_counter_mode(shm_addr);
        break;
    case MODE_3:
        init_text_editor_mode(shm_addr);
        break;
    case MODE_4:
        init_draw_board_mode(shm_addr);
        break;
    default:
        break;
    }

    return;
}

void init_clock_mode(shm_out *shm_addr)
{
    printf("Current Mode : Clock\n");

    clock_stat.cur_mode = M1_DEFAULT_MODE;
    clock_stat.blink = M1_BLINK;
    clock_stat.time = 0;

    setFnd(shm_addr, get_cur_time());
    setLed(shm_addr, 0b10000000);
}

void init_counter_mode(shm_out *shm_addr)
{
    printf("Current Mode : Counter\n");
}

void init_text_editor_mode(shm_out *shm_addr)
{
    printf("Current Mode : Text editor\n");
}

void init_draw_board_mode(shm_out *shm_addr)
{
    printf("Current Mode : Draw board\n");
}

void clock_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
    clock_stat.time++;
    if (clock_stat.time > 3)
    {   
        // Blink LED per each second
        printf("Clock LED blick!\n");
        clock_stat.time = 0;
        if (clock_stat.blink == M1_BLINK)
            clock_stat.blink = M1_UNBLINK;
        else
            clock_stat.blink = M1_BLINK;
    }

    switch (clock_stat.cur_mode)
    {
    // Default mode
    case M1_DEFAULT_MODE:
        // Change mode when no.1 switch is pushed
        if (sw_buff[0])
        {
            printf("Clock mode is changed to change mode\n");
            clock_stat.cur_mode = M1_CHANGE_MODE;
        }
        // Light on only first LED
        setLed(shm_addr, 0b10000000);
        break;

    // Time change mode
    case M1_CHANGE_MODE:
        // Change mode when no.1 switch is pushed
        if (sw_buff[0])
        {
            printf("Clock mode is changed to default mode\n");
            clock_stat.cur_mode = M1_DEFAULT_MODE;
        }
        // Initialize to board time when no.2 switch is pushed
        if (sw_buff[1])
            setFnd(shm_addr, get_cur_time());
        // Add 1 hour when no.3 switch is pushed
        if (sw_buff[2])
            setFnd(shm_addr, shm_addr->fnd + 100);
        // Add 1 minute when no.4 switch is pushed
        if (sw_buff[3])
            setFnd(shm_addr, shm_addr->fnd + 1);
        // Blink every second
        if (clock_stat.blink == M1_BLINK)
            setLed(shm_addr, 0b00100000);
        else
            setLed(shm_addr, 0b00010000);
        break;
    default:
        break;
    }
}
void counter_mode(shm_out *shm_addr)
{
}
void text_editor_mode(shm_out *shm_addr)
{
}
void draw_board_mode(shm_out *shm_addr)
{
}