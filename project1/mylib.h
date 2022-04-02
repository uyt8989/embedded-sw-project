#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h> 
#include <sys/shm.h>

#define MODE_1 100
#define MODE_2 200
#define MODE_3 300
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
    int mode;




} cur_stat;
