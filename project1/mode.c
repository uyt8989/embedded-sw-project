/*****************************************
 *
 * Mode functions :
 *
 * 1. Clear all devices when mode is chaned
 * 2. Mode Handling
 * 3. Initialize devices to each initial state
 *
 *****************************************/

#include "mylib.h"

extern int current_mode;

void init_device(shm_out *shm_addr)
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

    init_device(shm_addr);

    switch (current_mode)
    {
    case MODE_1:
        init_clock(shm_addr);
        break;
    case MODE_2:
        init_counter(shm_addr);
        break;
    case MODE_3:
        init_text_editor(shm_addr);
        break;
    case MODE_4:
        init_draw_board(shm_addr);
        break;
    default:
        break;
    }

    return;
}

// 해당 모드들에 맞게 초기화
void init_clock(shm_out *shm_addr)
{
    printf("Mode Change : Clock\n");
}

void init_counter(shm_out *shm_addr)
{
    printf("Mode Change : Counter\n");
}

void init_text_editor(shm_out *shm_addr)
{
    printf("Mode Change : Text editor\n");
}

void init_draw_board(shm_out *shm_addr)
{
    printf("Mode Change : Draw board\n");
}

void clock_mode(shm_out *shm_addr)
{
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