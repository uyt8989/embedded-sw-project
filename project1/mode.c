/*****************************************
 *
 * Mode functions :
 *
 * 1. Clear all devices when mode is chaned
 * 2. Mode Handling
 * 3. Initialize shared memory to each initial state
 * 4. Process inputs for each modes
 *
 *****************************************/

#include "mylib.h"
#include "fpga_dot_font.h"

extern int current_mode;
extern clock_s clock_stat;
extern counter_s counter_stat;
extern text_s text_stat;
extern draw_s draw_stat;

const static char keypad[MAX_BUTTON][M3_KEYPAD] = {
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

    setFnd(shm_addr, get_cur_time(), 10);
    setLed(shm_addr, 0b10000000);
}
void init_counter_mode(shm_out *shm_addr)
{
    printf("Current Mode : Counter\n");

    counter_stat.cur_mode = M2_DEC_MODE;
    counter_stat.count = 0;

    setFnd(shm_addr, 0, 10);
    setLed(shm_addr, 0b01000000);
}
void init_text_editor_mode(shm_out *shm_addr)
{
    int i;

    printf("Current Mode : Text editor\n");

    text_stat.cur_mode = M3_ALPHA_MODE;
    text_stat.count = 0;
    text_stat.cursor = -1;
    text_stat.last_sw = SW_NULL;
    text_stat.keypad_idx = 0;

    for (i = 0; i < LCD_MAX_BUFF; i++)
        text_stat.buff[i] = 0;

    for (i = 0; i < MAX_DOT_BUFF; i++)
        text_stat.dot[i] = dot_font[M3_DOT_FONT_A][i];

    setFnd(shm_addr, 0, 10);
    setLcd(shm_addr, text_stat.buff);
    setDot(shm_addr, text_stat.dot);
}
void init_draw_board_mode(shm_out *shm_addr)
{
    int i;

    printf("Current Mode : Draw board\n");

    draw_stat.cur_mode = M4_ON_CURSOR_MODE;
    draw_stat.count = 0;
    draw_stat.cursor[0] = 0;
    draw_stat.cursor[1] = MAX_DOT_Y - 1;
    draw_stat.time = 0;

    for (i = 0; i < MAX_DOT_BUFF; i++) {
        draw_stat.real_dot[i] = 0;
        draw_stat.dot[i] = 0;
    }

    setFnd(shm_addr, 0, 10);
    setDot(shm_addr, draw_stat.dot);
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
    setFnd(shm_addr, clock_stat.hour * 100 + clock_stat.min, 10);
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

    // Change FND numbers to each notations
    setFnd(shm_addr, counter_stat.count, digit);
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

        text_stat.cursor = -1;
        for (i = 0; i < LCD_MAX_BUFF; i++)
            text_stat.buff[i] = 0;
    }

    // Change input mode when no.5 and no.6 switches are pushed
    if (sw_buff[SW5] && sw_buff[SW6])
    {
        special_flag = TRUE;

        printf("Change input mode\n");
        if (text_stat.cur_mode == M3_ALPHA_MODE)
        {
            text_stat.cur_mode = M3_NUM_MODE;
            for (i = 0; i < MAX_DOT_BUFF; i++)
                text_stat.dot[i] = dot_font[M3_DOT_FONT_1][i];
        }
        else
        {
            text_stat.cur_mode = M3_ALPHA_MODE;
            text_stat.last_sw = SW_NULL;
            text_stat.keypad_idx = 0;
            for (i = 0; i < MAX_DOT_BUFF; i++)
                text_stat.dot[i] = dot_font[M3_DOT_FONT_A][i];
        }
    }

    // Insert space when no.8 and no.9 switches are pused
    if (sw_buff[SW8] && sw_buff[SW9])
    {
        special_flag = TRUE;

        // If buffer is fulled
        if (text_stat.cursor == LCD_MAX_BUFF - 1)
        {
            for (i = 0; i < LCD_MAX_BUFF - 1; i++)
            {
                text_stat.buff[i] = text_stat.buff[i + 1];
            }
            text_stat.buff[text_stat.cursor] = ' ';
        }
        else
        {
            text_stat.buff[++text_stat.cursor] = ' ';
        }
    }

    // No more processing when two switches are pushed
    if (special_flag == TRUE)
    {
        // text_stat.last_sw = SW_NULL;
        text_stat.count += 1;
        goto skip;
    }

    // Get pushed switch number
    sw_num = SW_NULL;
    for (i = 0; i < MAX_BUTTON; i++)
    {
        if (sw_buff[i] == TRUE)
            sw_num = i;
    }

    // There is no pushed switch
    if (sw_num == SW_NULL)
        goto skip;

    switch (text_stat.cur_mode)
    {
    case M3_ALPHA_MODE:
        printf("last switch %d current %d\n", text_stat.last_sw, sw_num);
        if (text_stat.last_sw == sw_num)
        {
            text_stat.keypad_idx = (text_stat.keypad_idx + 1) % M3_KEYPAD;
            text_stat.buff[text_stat.cursor] = keypad[sw_num][text_stat.keypad_idx];
        }
        else
        {
            text_stat.keypad_idx = 0;

            if (text_stat.cursor == LCD_MAX_BUFF - 1)
            {
                for (i = 0; i < LCD_MAX_BUFF - 1; i++)
                {
                    text_stat.buff[i] = text_stat.buff[i + 1];
                }
                text_stat.cursor--;
            }

            text_stat.buff[++text_stat.cursor] = keypad[sw_num][text_stat.keypad_idx];
        }

        text_stat.last_sw = sw_num;
        break;
    case M3_NUM_MODE:
        if (text_stat.cursor == LCD_MAX_BUFF - 1)
        {
            for (i = 0; i < LCD_MAX_BUFF - 1; i++)
            {
                text_stat.buff[i] = text_stat.buff[i + 1];
            }
            text_stat.cursor--;
        }
        text_stat.buff[++text_stat.cursor] = (sw_num + 1) + '0';
        break;
    default:
        break;
    }

    text_stat.count += 1;

skip:
    setFnd(shm_addr, text_stat.count, 10);
    setLcd(shm_addr, text_stat.buff);
    setDot(shm_addr, text_stat.dot);
}
void draw_board_mode(shm_out *shm_addr, unsigned char sw_buff[])
{
    int i, sw_num = SW_NULL;

    for (i = 0; i < MAX_BUTTON; i++)
    {
        if (sw_buff[i])
            sw_num = i;
    }

    switch (sw_num)
    {
    // Go to Initial state
    case SW1:
        for (i = 0; i < MAX_DOT_BUFF; i++)
            draw_stat.real_dot[i] = dot_set_blank[i];
        draw_stat.cur_mode = M4_ON_CURSOR_MODE;
        draw_stat.cursor[0] = 0;
        draw_stat.cursor[1] = MAX_DOT_Y - 1;
        draw_stat.count = 0;
        break;
    // Move up
    case SW2:
        draw_stat.cursor[0] = (draw_stat.cursor[0] + MAX_DOT_BUFF - 1) % MAX_DOT_BUFF;
        break;
    // Show cursor on/off
    case SW3:
        if (draw_stat.cur_mode == M4_ON_CURSOR_MODE)
            draw_stat.cur_mode = M4_OFF_CURSOR_MODE;
        else
            draw_stat.cur_mode = M4_ON_CURSOR_MODE;
        break;
    // Move left
    case SW4:
        draw_stat.cursor[1] = (draw_stat.cursor[1] + 1) % MAX_DOT_Y;
        break;
    // Select current position
    case SW5:
        draw_stat.real_dot[draw_stat.cursor[0]] |= (1 << draw_stat.cursor[1]);
        break;
    // Move right
    case SW6:
        draw_stat.cursor[1] = (draw_stat.cursor[1] + MAX_DOT_Y - 1) % MAX_DOT_Y;
        break;
    // Clear current drawing
    case SW7:
        for (i = 0; i < MAX_DOT_BUFF; i++)
            draw_stat.real_dot[i] = dot_set_blank[i];
        break;
    // Move down
    case SW8:
        draw_stat.cursor[0] = (draw_stat.cursor[0] + 1) % MAX_DOT_BUFF;
        break;
    // Reverse current drawing
    case SW9:
        for (i = 0; i < MAX_DOT_BUFF; i++)
            draw_stat.real_dot[i] = ~(draw_stat.real_dot[i]);
        break;

    default:
        break;
    }

    draw_stat.time++;

    for (i = 0; i < MAX_DOT_BUFF; i++)
            draw_stat.dot[i] = dot_set_blank[i];
            
    if (draw_stat.time > 3)
    {
        // Blink dot per each second
        draw_stat.time = 0;
        if (draw_stat.cur_mode == M4_ON_CURSOR_MODE)
        {
            draw_stat.dot[draw_stat.cursor[0]] 
                |= (1 << draw_stat.cursor[1]);
        }
    }

    if (sw_num < SW_NULL && sw_num > SW1)
        draw_stat.count += 1;

    setFnd(shm_addr, draw_stat.count, 10);
    if(draw_stat.cur_mode == M4_ON_CURSOR_MODE)
        setDot(shm_addr, draw_stat.dot);
    setDot(shm_addr, draw_stat.real_dot);
}