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
#include <signal.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>


#include "../inc/queue_ioctl.h"

#define LOG_FILE	"/tmp/queued.log"
#define OUT_DIR		"/tmp/queue_output"

void log_msg(const char *msg, ...) {
	FILE *log;
	va_list argptr;
    va_start(argptr, msg);
	
	log=fopen(LOG_FILE,"a");

	if(log) {
		vfprintf(log, msg, argptr);
		fputs("\n", log);
		fclose(log);
	}
	va_end(argptr);
}

void signal_handler(int sig) {
	switch(sig) {
		case SIGTERM:
			log_msg("terminate signal catched");
			exit(0);
		break;
	}
}

int main(int argc, char *argv[]) {
    int queue_dev_fd;
    msg_data_t    msg = {};
    size_t size;
	uint32_t index = 1;
	int ret;

	log_msg("queued started");

    queue_dev_fd = open(QUEUE_DEVNAME_PATH, O_RDWR);;
    if (queue_dev_fd < 0) {
        log_msg("Can't open device file: "QUEUE_DEVNAME_PATH". Seems module is not loaded\n");
        return -1;
    }

	signal(SIGTERM,signal_handler);

	mkdir(OUT_DIR, 0777);

	daemon(0,0);

    while(1) {
		memset(&msg, 0, sizeof(msg_data_t));
		ret = ioctl(queue_dev_fd, IOCTL_READ_QUEUE_ITEM_MSG, &msg);
        if ( ret < 0) {
            log_msg("ioctl error: %s\n", strerror(errno));
            return -1;
        }

		if(msg.size > 0) {
			int fd;
			char tmp[PATH_MAX];

			sprintf(tmp, OUT_DIR"/%06d_item.dat", index);

			fd = open(tmp, O_RDWR|O_CREAT|O_TRUNC);						
			if(fd) {
				write(fd, msg.data, msg.size);
				close(fd);
				log_msg("[%06u] Received message with size %lu", index++, msg.size);
			} else {
				log_msg("Unable to create output file: %s", tmp);
			}
		}
        usleep(1000000);
    }

    close(queue_dev_fd);

    return 0;
}


