#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct my_struct {
    char interval, cnt;
    char init[4];
};

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
    char temp = atoi(argv[3]);

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

    // separate init to buffer
    int i, idx = 0;
    for(i = 1000; i >= 1; i /= 10) 
        input.init[idx++] = temp / i;
    
    // init buffer must have only one number greater than 1
    char flag = 0;
    for(i = 0; i < 4; i++) {
        if(input.init[i] != 0) {
            // there are more than two numbers
            if(flag != 0) {
                printf("Invalid TIMER_INIT. You must have only one number greater than 1\n");
                return -1;
            }
            flag = 1;
        }
    }

	dev_fd = open("/dev/dev_driver", O_WRONLY);
	if (dev_fd < 0){
		printf("Open Failed!\n");
		return -1;
	}
    
    // print user's inputs
	printf("input: %d %d ", input.interval, input.cnt);
    for(i = 0; i < 4; i++) {
        printf("%d", input.init[i]);
    }
    printf("\n");

    // ioctl로 수정해야됨
	write(dev_fd, &input, sizeof(input));

    close(dev_fd);

	return 0;
}

