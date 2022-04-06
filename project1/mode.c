/*****************************************
 *
 * Mode functions :
 *
 * 1.
 *
 *****************************************/


#include "mylib.h"

extern int current_mode;

void mode_handler(shm_out* shm_addr, int d) {
    int changed_mode = current_mode + d;

    if(changed_mode < MODE_1) changed_mode = MODE_4;
    else if(changed_mode > MODE_4) changed_mode = MODE_1;

    printf("Mode is changed to code:%d\n", changed_mode);

    current_mode = changed_mode;

    switch (current_mode)
    {
    case MODE_1 :
        init_clock();
        break;
    case MODE_2 :
        init_counter(); 
        break;
    case MODE_3 : 
        init_text_editor();
        break;
    case MODE_4 : 
        init_draw_board();
        break;
    default:
        break;
    }

    return ;
}

// 해당 모드들에 맞게 초기화
void init_clock() {
    printf("Mode Change : Clock\n");
}

void init_counter() {
    printf("Mode Change : Counter\n");
}

void init_text_editor() {
    printf("Mode Change : Text editor\n");
}

void init_draw_board() {
    printf("Mode Change : Draw board\n");
}
