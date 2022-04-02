#include "mylib.h"

int current_mode = 1;

int main(int argc, char **argv)
{
    pid_t p1 = 0, p2 = 0;

    p1 = fork();

    // parent process
    if (p1 > 0)
    {
        p2 = fork();

        if (p2 > 0)
        {
            // main process
        }

        else if (p2 == 0)
        {
            // input process
        }

        else
        {
            printf("Fork Failed\n");
            exit(-1);
        }
    }

    // child process
    else if (p1 == 0)
    {
        // output process
    }

    else
    {
        printf("Fork Failed\n");
        exit(-1);
    }

    return 0;
}