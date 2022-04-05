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
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <dirent.h>
#include <signal.h>
#include <linux/input.h>

#define TRUE 1
#define FALSE 0

#define SHARED_MEM_KEY_IN 5678
#define SHARED_MEM_KEY_OUT 5679

#define SEMA_KEY 6789

#define BUFF_SIZE 64
#define MAX_BUTTON 9

#define BOARD_KEY_BACK 158
#define BOARD_KEY_PROG 116
#define BOARD_KEY_VOL_UP 115
#define BOARD_KEY_VOL_DOWN 114

//Clock mode
#define MODE_1 100

//Counter mode
#define MODE_2 200

//Text editor mode
#define MODE_3 300

//Draw board mode
#define MODE_4 400

//MOD1
#define M1_CHANGE_TIME -100
#define M1_NO_CHANGE_TIME -101

//MOD2
#define M2_DEC_MODE -102
#define M2_OCT_MODE -103
#define M2_QUA_MODE -104
#define M2_BIN_MODE -105

//MOD3
#define M3_NUM_MODE -106
#define M3_ALPHA_MODE -107

//MOD4


typedef struct _CUR_STAT {
    int cur_mode;



} cur_stat;

typedef struct _INPUT_SHARED_MEM {
    int exit;
    int key_code;
    unsigned char sw[MAX_BUTTON];
} input_shm;

typedef struct _OUTPUT_SHARED_MEM {
    int exit;
    int fnd;
    char text[32];
    unsigned char led;
    unsigned char dot[10];
    unsigned char init_flag;
} output_shm;

int seminit();
int semlock(int semid);
int semunlock(int semid);

int input_process(int shm_id);
int main_process(int shm_input_id, int shm_output_id);
int output_process(int shm_id);
int checkExit(input_shm *addr, int sem_id);