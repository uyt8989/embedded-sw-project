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
extern counter_s counter_stat;
extern text_s text_stat;
extern draw_s draw_stat;

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

    counter_stat.cur_mode = M2_DEC_MODE;

    setFnd(shm_addr, 0);
    setLed(shm_addr, 0b01000000);
}

void init_text_editor_mode(shm_out *shm_addr)
{
    printf("Current Mode : Text editor\n");

    text_stat.count = 0;
    text_stat.cur_mode = M3_ALPHA_MODE;
    text_stat.cursor = 0;
    text_stat.last_sw = 0;

    setFnd(shm_addr, 0);
    setLcd(shm_addr, "");
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
        if(clock_stat.cur_mode == M1_CHANGE_MODE)
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
        {
            setFnd(shm_addr, shm_addr->fnd + 1);
            if (shm_addr->fnd == 60)
            {
                shm_addr->fnd += 40;
            }
        }

        // Blink LED for each seconds
        if (clock_stat.blink == M1_BLINK)
            setLed(shm_addr, 0b00100000);
        else
            setLed(shm_addr, 0b00010000);
        break;
    default:
        break;
    }

    int i;
    int fnd_value = shm_addr->fnd;
    for (i = MAX_DIGIT - 1; i >= 0; i--)
    {
        shm_addr->digit[i] = fnd_value % 10;
        fnd_value /= 10;
    }
}
void counter_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
    int digit, i, fnd_value = shm_addr->fnd;

    // Change mode when no.1 switch is pushed
    if (sw_buff[0])
    {
        counter_stat.cur_mode =
            (counter_stat.cur_mode + 1) % M2_MODES;
    }

    // Change LED light according to current mode
    switch (counter_stat.cur_mode)
    {
    case M2_BIN_MODE:
        printf("Change notation to Binary\n");
        digit = 2;
        setLed(shm_addr, 0b10000000);
        break;
    case M2_DEC_MODE:
        printf("Change notation to Decimal\n");
        digit = 10;
        setLed(shm_addr, 0b01000000);
        break;
    case M2_OCT_MODE:
        printf("Change notation to Octal\n");
        digit = 8;
        setLed(shm_addr, 0b00100000);
        break;
    case M2_QUA_MODE:
        printf("Chage notation to Quaternary\n");
        digit = 4;
        setLed(shm_addr, 0b00010000);
        break;
    default:
        break;
    }

    // Add 1 to hundreds when no.2 switch is pushed
    if (sw_buff[1])
    {
        fnd_value += digit * digit;
    }
    // Add 1 to tens when no.3 switch is pushed
    if (sw_buff[2])
    {
        fnd_value += digit;
    }
    // Add 1 to units when no.4 switch is pushed
    if (sw_buff[3])
    {
        fnd_value += 1;
    }

    // Update FND value
    shm_addr->fnd = fnd_value;

    // Change FND numbers to each notations
    for (i = MAX_DIGIT - 1; i >= 0; i--)
    {
        shm_addr->digit[i] = fnd_value % digit;
        fnd_value /= digit;
    }
    if (counter_stat.cur_mode == M2_DEC_MODE)
        shm_addr->digit[0] = 0;
}
void text_editor_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
    int i, sw_num, flag = FALSE;

    // Clear LCD when no.2 and no.3 switches are pushed
    if (sw_buff[1] && sw_buff[2])
    {
        flag = TRUE;
        text_stat.last_sw = 0;
        text_stat.count++;

        // lcd 초기화
    }
    // Change input mode when no.5 and no.6 switches are pushed
    if (sw_buff[4] && sw_buff[5])
    {
        flag = TRUE;
        text_stat.last_sw = 0;
        text_stat.count++;
        if (text_stat.cur_mode == M3_ALPHA_MODE)
            text_stat.cur_mode = M3_NUM_MODE;
        else
            text_stat.cur_mode = M3_ALPHA_MODE;
    }
    // Insert space when no.8 and no.9 switches are pused
    if (sw_buff[7] && sw_buff[8])
    {
        flag = TRUE;
        text_stat.last_sw = 0;
        text_stat.count++;

        // 공백 문자 삽입
    }

    switch (text_stat.cur_mode)
    {
    case M3_ALPHA_MODE:
        // dot 에 A 그리기
        break;
    case M3_NUM_MODE:
        // dot에 B 그리기
        break;
    default:
        break;
    }

    // 두개 이상의 스위치가 눌린 경우
    if (flag == FALSE)
        return;

    sw_num = 0;
    for (i = 0; i < MAX_BUTTON; i++)
    {
        if (sw_buff[i] == TRUE)
            sw_num = i + 1;
    }

    // 이번에 스위치가 눌리지 않은 경우
    if (sw_num < 1)
        return;

    switch (text_stat.cur_mode)
    {
    case M3_ALPHA_MODE:

        break;
    case M3_NUM_MODE:

        break;
    default:
        break;
    }

    text_stat.last_sw = sw_num;
}
void draw_board_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
}