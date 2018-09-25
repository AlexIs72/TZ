#ifndef __QUEUE_IOCTL_H__
#define __QUEUE_IOCTL_H__

#ifdef __KERNEL__
	#include <linux/ioctl.h>
#else
	#include <sys/ioctl.h>
	#include <stdint.h>
#endif

#define MAGIC_NUM	0x81

#define BUF_SIZE	64000

#define QUEUE_DEVNAME 		"si_tz_queue"
#define QUEUE_DEVNAME_PATH 	"/dev/"QUEUE_DEVNAME


typedef struct {
	size_t	size;
#ifdef __KERNEL__
	u8	data[BUF_SIZE];
#else
	uint8_t	data[BUF_SIZE];
#endif	
} msg_data_t;

#define IOCTL_SEND_QUEUE_ITEM_MSG _IOW(MAGIC_NUM, 0, msg_data_t *)
#define IOCTL_READ_QUEUE_ITEM_MSG _IOR(MAGIC_NUM, 1, msg_data_t *)


#endif