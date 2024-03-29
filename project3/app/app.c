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

	// open the device file
	dev_fd = open("/dev/stopwatch", O_WRONLY);
	if (dev_fd < 0){
		printf("Open Failed!\n");
		return -1;
	}

    // execute device
    ioctl(dev_fd, IOCTL_COMMAND);

	// close the device file
    close(dev_fd);

	return 0;
}

