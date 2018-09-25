#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioctl.h>
#include <linux/proc_fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h> 
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/syscalls.h>
#include <linux/kmod.h>

#include "../inc/queue_ioctl.h"
#include "queue.h"

MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");

#define QUEUE_SIZE	1024
#define CACHE_DIR	"/tmp/queue_cache/"

static queue_t *_queue = NULL;
volatile static u32 _cache_index = 0;
volatile static u32 _restore_index = 0;
static struct rw_semaphore rw_sem = {};
static struct rw_semaphore rw_cache_sem = {};
static struct task_struct *queue_thread = NULL;

static int queue_open( struct inode *n, struct file *f ) { 
//	printk( KERN_INFO "Open device\n" );	
   	return 0; 
} 
 
static int queue_release( struct inode *n, struct file *f ) { 
//	printk( KERN_INFO "Close device\n" );	
   	return 0; 
} 

static void store_to_file(msg_data_t *msg) {
	struct file *f = NULL; 
	char path[128] = {};

	snprintf(path, sizeof(path), CACHE_DIR"%06d_item.dat", _cache_index);

	f = filp_open( path, O_RDWR|O_CREAT, S_IRWXU|S_IRWXG|S_IRWXO); 

    if( IS_ERR(f) ) {
 		printk( KERN_ERR "file open failed: %s\n", path);
 		return /*-ENOENT*/;
 	} 	

	kernel_write( f, msg->data, msg->size, 0);

	filp_close( f, NULL );
 
	_cache_index++;
}


static void load_from_files(void) {
	struct file *f = NULL; 
	char path[128] = {};
    msg_data_t *msg = NULL;
    int size, rc;
    char * envp[] = { "HOME=/", NULL } ;
    char * argv[] = { "/bin/rm", "-f", path, NULL };

    snprintf(path, sizeof(path), CACHE_DIR"%06d_item.dat", _restore_index);

	f = filp_open( path, O_RDONLY, 0); 

    if( IS_ERR(f) ) {
 		printk( KERN_ERR "file open failed: %s\n", path);
 		return;
 	} 	

    msg = kmalloc(sizeof(msg_data_t), GFP_KERNEL);
    if(!msg) {
        printk( KERN_ERR "Unable to alloc memory for reading cache file\n");
        return;
    }

	size = kernel_read( f, 0, msg->data, sizeof(msg->data));
    filp_close( f, NULL );

    if(size > 0) {
        msg->size = size;
        down_write( &rw_sem ); 
        rc = queue_push_back(_queue, msg);
        up_write( &rw_sem );
        if(rc != 0) {
        	printk( KERN_ERR "Queue is full again\n");
            return;
        }
    }

    down_write( &rw_cache_sem );
    call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC );
    up_write( &rw_cache_sem );

    _restore_index++;    
}

static long queue_ioctl(struct file *filp, unsigned int cmd, unsigned long argp) {
	int ret = -EFAULT;
   	
	if( ( _IOC_TYPE( cmd ) != MAGIC_NUM ) ) {
    	return -ENOTTY; 
	}

   	switch( cmd ) {
		case IOCTL_SEND_QUEUE_ITEM_MSG:
            {
                int ret2 = 0;
			    msg_data_t *msg = kmalloc(sizeof(msg_data_t), GFP_KERNEL);
			    if(msg) {
                    ret = copy_from_user(msg, ((void *)argp), sizeof(msg_data_t));
				    if(ret == 0) {
					    printk(KERN_INFO "received = %lu ... ", msg->size );
                        down_write( &rw_sem ); 
                        ret2 = queue_push_back(_queue, msg);
                        up_write( &rw_sem );
					    if(ret2 != 0) {
                            down_write( &rw_cache_sem );
						    store_to_file(msg);
                            up_write( &rw_cache_sem );
						    printk(KERN_INFO "stored to file\n");
                        }
				    }
				    kfree(msg);
                }
			}			
			break; 
		case IOCTL_READ_QUEUE_ITEM_MSG:
            {
                msg_data_t *msg = kmalloc(sizeof(msg_data_t), GFP_KERNEL);
                if(msg) {
                    ret = 0;
                    memset(msg, 0, sizeof(msg_data_t));

                    down_read( &rw_sem );

                    if(queue_pop_front(_queue, msg) == 0) {
                        ret = copy_to_user(((void *)argp), msg, sizeof(msg_data_t));
                        if(ret == 0) {
                            ret = msg->size;
                        }
                    }
                    up_read( &rw_sem );    
                    kfree(msg);
                }
            }
			break; 
   	} 

   	return ret; 
} 

static const struct file_operations queue_fops = { 
   .owner = THIS_MODULE, 
   .open = queue_open, 
   .release = queue_release, 
//   .read  = queue_read, 
   .unlocked_ioctl = queue_ioctl 
}; 

static int queue_major = -1;
static struct cdev queue_cdev;
static struct class *queue_class = NULL;

static char *queue_devnode(struct device *dev, umode_t *mode) {
	if (!mode) {
		return NULL;
	}
    if (dev->devt == queue_major) {
    	*mode = 0666;
	}
    return NULL;
}

static int remove_device(int is_device_exists, int ret_code) {
    if (is_device_exists) {
        device_destroy(queue_class, queue_major);
        cdev_del(&queue_cdev);
    }

    if (queue_class) {
        class_destroy(queue_class);
	}

    if (queue_major != -1) {
        unregister_chrdev_region(queue_major, 1);
	}

	return ret_code;
}

int queue_thread_func(void *data) {
    int do_scan = 0;
    while(!kthread_should_stop()) {
        down_read( &rw_sem );
        if(_cache_index > 0 && _restore_index < _cache_index) {
            do_scan = 1;        
        }
        up_read( &rw_sem );

        if(do_scan) {
            load_from_files();
        }
        do_scan = 0;

        msleep( 100 );
        schedule();
    }
    
    return 0;
}

static int __init mod_queue_init(void) {
    init_rwsem( &rw_sem );
    init_rwsem( &rw_cache_sem );

	_queue = queue_init(QUEUE_SIZE);
	if(_queue == NULL) {
		printk( KERN_ERR QUEUE_DEVNAME": Unable to alloc queue\n" );
		return remove_device(0, -1);
	}

    printk( KERN_INFO QUEUE_DEVNAME": queue allocated with size: %d\n", QUEUE_SIZE );

    queue_thread = kthread_run(&queue_thread_func,NULL,"queue_thread");
    if(queue_thread == NULL) {
        printk( KERN_ERR QUEUE_DEVNAME": Unable to start queue thread\n" );
        return remove_device(0,-1);
    }
    printk( KERN_INFO QUEUE_DEVNAME": queue thread started\n");

    /* cat /proc/devices */
    if (alloc_chrdev_region(&queue_major, 0, 1, QUEUE_DEVNAME) < 0) {
		printk( KERN_ERR QUEUE_DEVNAME": Unable to alloc region\n" );
        return -1;
	}
    /* ls /sys/class */
    if ((queue_class = class_create(THIS_MODULE, QUEUE_DEVNAME)) == NULL) {
		printk( KERN_ERR QUEUE_DEVNAME": Unable to create class\n" );
        return remove_device(0,-1);
	}
	queue_class->devnode = queue_devnode;

    /* ls /dev/ */
    if (device_create(queue_class, NULL, queue_major, NULL, QUEUE_DEVNAME) == NULL) {
		printk( KERN_ERR QUEUE_DEVNAME": Unable to create device\n" );
        return remove_device(0,-1);
	}
    cdev_init(&queue_cdev, &queue_fops);
    if (cdev_add(&queue_cdev, queue_major, 1) == -1) {
		printk( KERN_ERR QUEUE_DEVNAME": Unable to add device\n" );
        return remove_device(1,-1);
	}

	printk( KERN_INFO QUEUE_DEVNAME": device created\n" );

/*	f = filp_open( CACHE_DIR, O_DIRECTORY|O_CREAT, S_IRWXU);	
	if(IS_ERR(f)) {
		printk( KERN_ERR QUEUE_DEVNAME": Unable to create cache dir \n" );
		return remove_device(1,-1);
	} 

	filp_close(f, NULL);
*/

    return 0;
}

static void __exit mod_queue_exit(void) {
    if(queue_thread) {
        kthread_stop(queue_thread);
        printk( KERN_INFO QUEUE_DEVNAME": queue thread_stopped\n" );
    }
    if(_queue) {
	    queue_destroy(&_queue);
	    printk( KERN_INFO QUEUE_DEVNAME": queue destroyed\n" );
    }
	remove_device(1, 0);
	printk( KERN_INFO QUEUE_DEVNAME": device removed\n" );
}

module_init(mod_queue_init);
module_exit(mod_queue_exit);



