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

static char keypad[9][3] = {
    {'.', 'Q', 'Z'}, {'A', 'B', 'C'}, {'D', 'E', 'F'}, {'G', 'H', 'I'}, {'J', 'K', 'L'}, {'M', 'N', 'O'}, {'P', 'R', 'S'}, {'T', 'U', 'V'}, {'W', 'X', 'Y'}};

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
    clock_stat.hour = get_cur_time() / 100;
    clock_stat.min = get_cur_time() % 100;
    clock_stat.blink = M1_BLINK;

    setFnd(shm_addr, get_cur_time());
    setLed(shm_addr, 0b10000000);
}
void init_counter_mode(shm_out *shm_addr)
{
    printf("Current Mode : Counter\n");

    counter_stat.cur_mode = M2_DEC_MODE;
    counter_stat.count = 0;

    setFnd(shm_addr, 0);
    setLed(shm_addr, 0b01000000);
}
void init_text_editor_mode(shm_out *shm_addr)
{
    int i;

    printf("Current Mode : Text editor\n");

    text_stat.cur_mode = M3_ALPHA_MODE;
    text_stat.cursor = 0;
    text_stat.last_sw = SW_NULL;
    text_stat.keypad_idx = 0;
    for (i = 0; i < LCD_MAX_BUFF; i++)
        text_stat.buff[i] = ' ';

    setFnd(shm_addr, 0);
    setLcd(shm_addr, "");
}
void init_draw_board_mode(shm_out *shm_addr)
{
    printf("Current Mode : Draw board\n");
}

void clock_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
    int i;
    unsigned char led;

    clock_stat.time++;
    if (clock_stat.time > 3)
    {
        // Blink LED per each second
        if (clock_stat.cur_mode == M1_CHANGE_MODE)
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
        if (sw_buff[SW1])
        {
            printf("Clock mode is changed to change mode\n");
            clock_stat.cur_mode = M1_CHANGE_MODE;
        }
        // Light on only first LED
        led = 0b10000000;
        break;

    // Time change mode
    case M1_CHANGE_MODE:
        // Change mode when no.1 switch is pushed
        if (sw_buff[SW1])
        {
            printf("Clock mode is changed to default mode\n");
            clock_stat.cur_mode = M1_DEFAULT_MODE;
        }
        // Initialize to board time when no.2 switch is pushed
        if (sw_buff[SW2])
        {
            clock_stat.hour = get_cur_time() / 100;
            clock_stat.min = get_cur_time() % 100;
        }
        // Add 1 hour when no.3 switch is pushed
        if (sw_buff[SW3])
            clock_stat.hour += 1;
        // Add 1 minute when no.4 switch is pushed
        if (sw_buff[SW4])
        {
            clock_stat.min += 1;
        }

        // Caculate hours and minutes
        if (clock_stat.min >= 60)
        {
            clock_stat.min = 0;
            clock_stat.hour += 1;
        }

        if (clock_stat.hour >= 24)
        {
            clock_stat.hour = 0;
        }

        // Blink LED for each seconds
        if (clock_stat.blink == M1_BLINK)
            led = 0b00100000;
        else
            led = 0b00010000;
        break;
    default:
        break;
    }

    // Set output shared memory
    int fnd_value = clock_stat.hour * 100 + clock_stat.min;
    for (i = MAX_DIGIT - 1; i >= 0; i--)
    {
        shm_addr->digit[i] = fnd_value % 10;
        fnd_value /= 10;
    }
    setLed(shm_addr, led);
}
void counter_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
    int digit, i, prev_mode, fnd_value;
    unsigned led;

    prev_mode = counter_stat.cur_mode;

    // Change mode when no.1 switch is pushed
    if (sw_buff[SW1])
    {
        counter_stat.cur_mode =
            (counter_stat.cur_mode + 1) % M2_MODES;
    }

    // Change LED light according to current mode
    switch (counter_stat.cur_mode)
    {
    case M2_BIN_MODE:
        if (prev_mode != M2_BIN_MODE)
            printf("Change notation to Binary\n");
        digit = 2;
        led = 0b10000000;
        // setLed(shm_addr, 0b10000000);
        break;
    case M2_DEC_MODE:
        if (prev_mode != M2_DEC_MODE)
            printf("Change notation to Decimal\n");
        digit = 10;
        led = 0b01000000;
        // setLed(shm_addr, 0b01000000);
        break;
    case M2_OCT_MODE:
        if (prev_mode != M2_OCT_MODE)
            printf("Change notation to Octal\n");
        digit = 8;
        led = 0b00100000;
        // setLed(shm_addr, 0b00100000);
        break;
    case M2_QUA_MODE:
        if (prev_mode != M2_QUA_MODE)
            printf("Chage notation to Quaternary\n");
        digit = 4;
        led = 0b00010000;
        // setLed(shm_addr, 0b00010000);
        break;
    default:
        break;
    }

    // Add 1 to hundreds when no.2 switch is pushed
    if (sw_buff[SW2])
    {
        counter_stat.count += digit * digit;
    }
    // Add 1 to tens when no.3 switch is pushed
    if (sw_buff[SW3])
    {
        counter_stat.count += digit;
    }
    // Add 1 to units when no.4 switch is pushed
    if (sw_buff[SW4])
    {
        counter_stat.count += 1;
    }

    // Update FND value
    shm_addr->fnd = counter_stat.count;

    // Change FND numbers to each notations
    fnd_value = counter_stat.count;
    for (i = MAX_DIGIT - 1; i >= 0; i--)
    {
        shm_addr->digit[i] = fnd_value % digit;
        fnd_value /= digit;
    }
    if (counter_stat.cur_mode == M2_DEC_MODE)
        shm_addr->digit[0] = 0;
    setLed(shm_addr, led);
}
void text_editor_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
    int i, sw_num, fnd_value, special_flag = FALSE;

    // Clear LCD when no.2 and no.3 switches are pushed
    if (sw_buff[SW2] && sw_buff[SW3])
    {
        special_flag = TRUE;
        text_stat.last_sw = SW_NULL;
        text_stat.count++;
        text_stat.cursor = 0;
        memset(text_stat.buff, 0, LCD_MAX_BUFF * sizeof(char));
    }

    // Change input mode when no.5 and no.6 switches are pushed
    if (sw_buff[SW5] && sw_buff[SW6])
    {
        special_flag = TRUE;
        text_stat.last_sw = SW_NULL;
        text_stat.count++;
        if (text_stat.cur_mode == M3_ALPHA_MODE)
            text_stat.cur_mode = M3_NUM_MODE;
        else
            text_stat.cur_mode = M3_ALPHA_MODE;
    }
    // Insert space when no.8 and no.9 switches are pused
    if (sw_buff[SW7] && sw_buff[SW9])
    {
        special_flag = TRUE;
        text_stat.last_sw = 0;
        text_stat.count++;

        // If buffer is fulled
        if (text_stat.cursor == LCD_MAX_BUFF)
        {
            for (i = 0; i < LCD_MAX_BUFF - 1; i++)
            {
                text_stat.buff[i] = text_stat.buff[i + 1];
            }
            text_stat.buff[text_stat.cursor - 1] = ' ';
        }
        else
            text_stat.buff[text_stat.cursor++] = ' ';
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

    // No more processing when two switches are pushed
    if (special_flag == TRUE)
        return;

    // Get pushed switch number
    sw_num = SW_NULL;
    for (i = 0; i < MAX_BUTTON; i++)
    {
        if (sw_buff[i] == TRUE)
            sw_num = i;
    }

    // There is no pushed switch
    if (sw_num == SW_NULL)
        return;

    switch (text_stat.cur_mode)
    {
    case M3_ALPHA_MODE:
        if (text_stat.last_sw == sw_num)
        {
            text_stat.keypad_idx = (text_stat.keypad_idx + 1) / M3_KEYPAD;
            text_stat.buff[text_stat.cursor] = keypad[sw_num][text_stat.keypad_idx];
        }

        else
        {
            if (text_stat.cursor == LCD_MAX_BUFF)
            {
                for (i = 0; i < LCD_MAX_BUFF - 1; i++)
                {
                    text_stat.buff[i] = text_stat.buff[i + 1];
                }
                text_stat.cursor--;
            }
            text_stat.keypad_idx = 0;
            text_stat.buff[text_stat.cursor++] = keypad[sw_num][0];
        }

        break;
    case M3_NUM_MODE:
        if (text_stat.cursor == LCD_MAX_BUFF)
        {
            for (i = 0; i < LCD_MAX_BUFF - 1; i++)
            {
                text_stat.buff[i] = text_stat.buff[i + 1];
            }
            text_stat.cursor--;
        }
        text_stat.buff[text_stat.cursor++] = (sw_num + 1) + '0';
        break;
    default:
        break;
    }

    text_stat.last_sw = sw_num;

    setLcd(shm_addr, text_stat.buff);
    fnd_value = counter_stat.count;
    for (i = MAX_DIGIT - 1; i >= 0; i--)
    {
        shm_addr->digit[i] = fnd_value % 10;
        fnd_value /= 10;
    }
    setDot(shm_addr, text_stat.dot);
}
void draw_board_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
}