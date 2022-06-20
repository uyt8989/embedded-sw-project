#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include "../module/maze.h"

int dx[] = {-1, 0, 1, 0}; 
int dy[] = {0, 1, 0, -1};
int visit[10][7];

void dfs(int fd, int x, int y) {
	int i, result;
	for(i = 0; i < 4; i++) {
		if(visit[x+dx[i]][y+dy[i]] == 1) continue; 
		result = ioctl(fd, IOCTL_MOVE, &i);
		if(result == 1) {
			visit[x+dx[i]][y+dy[i]] = 1;
			dfs(fd, x + dx[i], y + dy[i]);
		}
	}
}

int main(int argc, char **argv)
{
	int dev_fd;

	// open the device file
	dev_fd = open("/dev/maze", O_WRONLY);
	if (dev_fd < 0){
		printf("Open Failed!\n");
		return -1;
	}

    // execute device
    ioctl(dev_fd, IOCTL_START);

	visit[0][0] = 1;
	dfs(dev_fd, 0, 0);

	int i, j;
	for(i = 0; i < 10; i++) {
		for(j = 0; j < 7; j++) {
			printf("%d ", visit[i][j]);
		}
		printf("\n");
	}

	// close the device file
    close(dev_fd);

	return 0;
}

