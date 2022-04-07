/*****************************************
 *
 * Semaphore functions :
 *
 * 1. initsem() : Initialize semaphore
 * 2. semlock(int semid) : P operation
 * 3. semunlock(int semid) : V operation
 *
 *****************************************/

#include "mylib.h"

//Semaphore union
union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

//Initialize semaphore
int seminit()
{
    union semun semunarg;
    int status = 0, semid;

    semid = semget((key_t)SEMA_KEY, 1, IPC_CREAT | IPC_EXCL | 0600);

    if (semid == -1)
    {
        // If there is a semphore already
        if (errno == EEXIST)
            semid = semget((key_t)SEMA_KEY, 1, 0);
    }
    else
    {
        // Set initial semaphore value to 1
        semunarg.val = 1;
        status = semctl(semid, 0, SETVAL, semunarg);
    }

    //Error occur
    if (semid == -1 || status == -1)
    {
       printf("seminit error\n");
       exit(-1);
    }

    return semid;
}

// P operation
int semlock(int semid)
{
    struct sembuf buf;

    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;

    if (semop(semid, &buf, 1) < 0)
    {
        printf("semlock error\n");
        exit(-1);
    }

    return 0;
}

// V operation
int semunlock(int semid)
{
    struct sembuf buf;

    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;

    if (semop(semid, &buf, 1) < 0)
    {
        printf("semunlock error\n");
        exit(-1);
    }

    return 0;
}