#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "../module/mydev.h"

struct my_struct input;

int main(int argc, char **argv)
{
	int dev_fd;

    // execution with insufficient arguments
	if(argc != 4) { 
		printf("Usage : TIMER_INTERVAL[1-100] TIMER_CNT[1-100] TIMER_INIT[0001-8000]\n");
		return -1;
	}

    // get arguments
    input.interval = atoi(argv[1]);
    input.cnt = atoi(argv[2]);
    int temp = atoi(argv[3]);

    printf("temp : %d\n", temp);

    // invalid interval
    if(1 > input.interval || input.interval > 100) {
        printf("Invalid TIMER_INTERVAL. Check the value of TIMER_INTERVAL\n");
        return -1;
    }

    // invalid cnt
    if(1 > input.cnt || input.cnt > 100) {
        printf("Invalid TIMER_CNT. Check the value of TIMER_CNT\n");
        return -1;
    }

    // invalid init
    if(1 > temp || temp > 8000) {
        printf("Invalid TIMER_INIT. Check the value of TIMER_INIT\n");
        return -1;
    }

    // initial state
    input.num = 100; input.pos = 100;
    
    // separate init to buffer
    int i, idx = 3;

    for(i = 0; i < 4; i++) {
        int temp_res = temp % 10;
        //printf("temp_res: %d\n", temp_res);
        //printf("%d %d\n", input.num, input.pos);

        if (temp_res != 0) {
            if(input.num == 100 && input.pos == 100) {           
                input.num = temp_res;
                input.pos = idx;
            }
            else {
                printf("Invalid TIMER_INIT. You must have only one number greater than 1\n");
                return -1;
            }
        }

        idx--;
        temp = temp / 10;
    }
    
    // init buffer must have exactly one number greater than 1
    if(input.num == 0 && input.pos == -1) {
        printf("Invalid TIMER_INIT. You must have exactly one number greater than 1\n");
        return -1;
    }

    /*
	dev_fd = open("/dev/dev_driver", O_WRONLY);
	if (dev_fd < 0){
		printf("Open Failed!\n");
		return -1;
	}*/
    
    // print user's inputs
	printf("input: %d %d %d %d\n", input.interval, input.cnt, input.num, input.pos);

    /*
    // set options
	ioctl(dev_fd, IOCTL_SET_OPTION, &input);

    // execute command
    ioctl(dev_fd, IOCTL_COMMAND);

    close(dev_fd);
    */

	return 0;
}

