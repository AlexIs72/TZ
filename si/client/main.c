#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


#include "../inc/queue_ioctl.h"

int get_random_in_range(int min, int max) {
	return (rand() % (max + 1 - min)) + min;
}

size_t get_random_buf(char *buf) {
	size_t buf_size = get_random_in_range(1, BUF_SIZE);
	size_t i;

	for(i=0; i<buf_size; i++) {
		buf[i] = get_random_in_range(0x20, 0x7E);
	}

	return buf_size;
}

int main(int argc, char *argv[]) {
	int queue_dev_fd;
//	queue_item_t	queue_item;
	msg_data_t	msg;
	size_t size;
	uint32_t index = 1;
	useconds_t	seconds[] = {250000, 500000, 1000000, 2000000};

	queue_dev_fd = open(QUEUE_DEVNAME_PATH, O_RDWR);
	if (queue_dev_fd < 0) {
		printf("Can't open device file: "QUEUE_DEVNAME_PATH". Seems module is not loaded\n");
		return -1;
	}

	while(1) {
		msg.size = get_random_buf(&msg.data[0]);
  		if (ioctl(queue_dev_fd, IOCTL_SEND_QUEUE_ITEM_MSG, &msg) < 0) {
     		printf ("ioctl error: %s\n", strerror(errno));
     		return -1;
  		}
		printf("[%06u] Send %lu bytes ... \n", index, msg.size);
/*		if(index % 50 == 0) {
			usleep(seconds[2]);
		} else if(index % 20 == 0) {
			usleep(seconds[1]);
		} else {*/
			usleep(seconds[0]);
//		}
		index++;
	}

	close(queue_dev_fd);

	return 0;
}

