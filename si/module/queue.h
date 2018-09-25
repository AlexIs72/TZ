#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifndef __KERNEL__
	#include <stdint.h>
#endif

#include "../inc/queue_ioctl.h"


struct _queue_item;

typedef struct _queue_item {
    msg_data_t   *data;
    struct _queue_item *prev;
    struct _queue_item *next;
} queue_item_t;

typedef struct {
    size_t size;
	size_t index;
    queue_item_t *head;
    queue_item_t *tail;
} queue_t;


int         queue_push_back(queue_t *, msg_data_t *);
int         queue_pop_front(queue_t *, msg_data_t *); 
//size_t      queie_get_index(queue_t *); 
//size_t      queue_get_size(queue_t *);
int         queue_is_full(queue_t *);
int         queue_is_empty(queue_t *);
queue_t*    queue_init(size_t);
void        queue_destroy(queue_t **);

#endif
