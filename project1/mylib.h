#ifndef __MY_LIB__
#define __MY_LIB__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <linux/input.h>

#define TRUE 1
#define FALSE 0

#define SHARED_MEM_KEY_IN 5678
#define SHARED_MEM_KEY_OUT 5679
#define SEMA_KEY 6789

#define KEY_BUFF_SIZE 64
#define LCD_MAX_BUFF 32
#define MAX_BUTTON 9
#define MAX_DIGIT 4
#define MAX_DOT_BUFF 10
#define MAX_DOT_Y 7

#define BOARD_KEY_DEFAULT 117
#define BOARD_KEY_BACK 158
#define BOARD_KEY_PROG 116
#define BOARD_KEY_VOL_UP 115
#define BOARD_KEY_VOL_DOWN 114

#define CHANGE_UP 1
#define CHANGE_DOWN -1

#define MODS 4
#define MODE_1 0 // Clock mode
#define MODE_2 1 // Counter mode
#define MODE_3 2 // Text editor mode
#define MODE_4 3 // Draw board mode

#define FPGA_BASE_ADDRESS 0x08000000
#define LED_ADDR 0x16

enum
{
    SW1, SW2, SW3, SW4, SW5,
    SW6, SW7, SW8, SW9, SW_NULL
};

// MOD1
#define M1_DEFAULT_MODE -100
#define M1_CHANGE_MODE -101
#define M1_BLINK -102
#define M1_UNBLINK -103

// MOD2
#define M2_MODES 4
#define M2_DEC_MODE 0
#define M2_OCT_MODE 1
#define M2_QUA_MODE 2
#define M2_BIN_MODE 3

// MOD3
#define M3_ALPHA_MODE -106
#define M3_NUM_MODE -107
#define M3_KEYPAD 3
#define M3_DOT_FONT_1 1
#define M3_DOT_FONT_A 10

// MOD4
#define M4_ON_CURSOR_MODE -108
#define M4_OFF_CURSOR_MODE -109

typedef struct _SHARED_MEM_IN
{
    int exit;
    int key_code;
    unsigned char sw[MAX_BUTTON];
} shm_in;

typedef struct _SHARED_MEM_OUT
{
    int fnd;
    unsigned char digit[MAX_DIGIT];
    char lcd[LCD_MAX_BUFF];
    unsigned char led;
    unsigned char dot[MAX_DOT_BUFF];
} shm_out;

typedef struct _CLOCK_STAT
{
    int cur_mode;
    int hour;
    int min;
    int blink;
    int time;
} clock_s;

typedef struct _COUNTER_STAT
{
    int cur_mode;
    int count;
} counter_s;

typedef struct _TEXT_EDITOR_STAT
{
    int cur_mode;
    int count;
    int cursor;
    int last_sw;
    int keypad_idx;
    char buff[LCD_MAX_BUFF];
    unsigned char dot[MAX_DOT_BUFF];
} text_s;

typedef struct _DRAW_BOARD_STAT
{
    int cur_mode;
    int count;
    int cursor[2];
    int time;
    unsigned char dot[MAX_DOT_BUFF];
    unsigned char real_dot[MAX_DOT_BUFF];
} draw_s;

// sema.c
int seminit();
int semlock(int semid);
int semunlock(int semid);

// utils.c
int setExit(shm_in *addr, int sem_id);
int checkExit(shm_in *addr, int sem_id);
int get_cur_time();

// input_process.c
int input_process(int shm_id);
void key_action(int dev_key, shm_in *shm_addr, int sem_id);
void switch_action(int dev_sw, shm_in *shm_addr, int sem_id);

// main_process.c
int main_process(int shm_input_id, int shm_output_id);
int getKeycode(shm_in *shm_addr, int sem_id);
int getSwitch(shm_in *shm_addr, int sem_id, unsigned char sw_buff[]);

void setFnd(shm_out *, int, int);
void setDot(shm_out *, unsigned char *);
void setLcd(shm_out *, char *);
void setLed(shm_out *, unsigned char);

// output_process.c
int output_process(int shm_input_id, int shm_output_id);

int writeToFnd(shm_out *, int fd);
int writeToDot(shm_out *, int fd);
int writeToLcd(shm_out *, int fd);
void writeToLed(shm_out *, unsigned char *addr);

// mode.c
void clear_out_shm(shm_out *shm_addr);
void mode_handler(shm_out *shm_addr, int d);

void init_clock_mode(shm_out *shm_addr);
void init_counter_mode(shm_out *shm_addr);
void init_text_editor_mode(shm_out *shm_addr);
void init_draw_board_mode(shm_out *shm_addr);

void clock_mode(shm_out *shm_addr, unsigned char sw_buff[]);
void counter_mode(shm_out *shm_addr, unsigned char sw_buff[]);
void text_editor_mode(shm_out *shm_addr, unsigned char sw_buff[]);
void draw_board_mode(shm_out *shm_addr, unsigned char sw_buff[]);

#endif