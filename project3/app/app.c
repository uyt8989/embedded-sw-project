#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "../module/stopwatch.h"

int main(int argc, char **argv)
{
	int dev_fd;

	dev_fd = open("/dev/stopwatch", O_WRONLY);
	if (dev_fd < 0){
		printf("Open Failed!\n");
		return -1;
	}

    // execute command
    ioctl(dev_fd, IOCTL_COMMAND);

    close(dev_fd);

	return 0;
}

