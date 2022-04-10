/*****************************************
 *
 * Semaphore functions :
 *
 * 1. Initialize semaphores (input, output)
 * 2. Wait and Post operation
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
    int status = 0, semid, i;

    semid = semget((key_t)SEMA_KEY, SEMA_NUM, IPC_CREAT | IPC_EXCL | 0600);

    if (semid == -1)
    {
        // If there is a semphore already
        if (errno == EEXIST)
            semid = semget((key_t)SEMA_KEY, SEMA_NUM, 0);
    }
    else
    {
        // Set initial semaphore value to 1
        semunarg.val = 1;
        for(i = 0; i < SEMA_NUM; i++)
            status = semctl(semid, i, SETVAL, semunarg);
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
int semlock(int semid, int sem_num)
{
    struct sembuf buf;

    buf.sem_num = sem_num;
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
int semunlock(int semid, int sem_num)
{
    struct sembuf buf;

    buf.sem_num = sem_num;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;

    if (semop(semid, &buf, 1) < 0)
    {
        printf("semunlock error\n");
        exit(-1);
    }

    return 0;
}