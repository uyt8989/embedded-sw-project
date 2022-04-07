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

// 해당 모드들에 맞게 초기화
void init_clock_mode(shm_out *shm_addr)
{
    printf("Mode Change : Clock\n");
    setFnd(shm_addr, get_cur_time());
    setLed(shm_addr, 0b10000000);
}

void init_counter_mode(shm_out *shm_addr)
{
    printf("Mode Change : Counter\n");
}

void init_text_editor_mode(shm_out *shm_addr)
{
    printf("Mode Change : Text editor\n");
}

void init_draw_board_mode(shm_out *shm_addr)
{
    printf("Mode Change : Draw board\n");
}

void clock_mode(shm_out *shm_addr, int sw_buff[])
{
    //현재 어떤 모드냐에 따라서 스위치 입력 관리
    
    //시간 바꾸는 모드가 아닌 경우에
    //1번 스위치 입력 시 시간 바꾸는 모드로 바꿈
    //1번 led만 점등

    //시간 바꾸는 모드라면
    //1번 스위치 입력 시 시간 안 바꾸는 모드로 바꿈
    //2번 스위치 입력 시 보드 시간으로 초기화
        if(sw_buff[1]) setFnd(shm_addr, get_cur_time());
    //3번 스위치 입력 시 시 1 증가
        if(sw_buff[2]) setFnd(shm_addr, shm_addr->fnd + 100);
    //4번 스위치 입력 시 분 1 증가
        if(sw_buff[3]) setFnd(shm_addr, shm_addr->fnd + 1);
    //매초 led 깜빡거림
        if(shm_addr->led == 0b0010000) shm_addr->led == 0b00010000;
        else shm_addr->led = 0b0010000;
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