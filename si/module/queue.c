#ifdef __KERNEL__
	#include <linux/kernel.h>
	#include <linux/slab.h>
    #include <linux/semaphore.h>
#else
	#include <stdlib.h>
	#include <stdio.h>
	#include <memory.h>
#endif

#include "queue.h"

queue_t* queue_init(size_t size) {
    queue_t *queue = NULL;

#ifdef __KERNEL__
	queue = kmalloc( sizeof(queue_t), GFP_KERNEL );
	memset(queue, 0, sizeof(queue_t));
#else
    queue = calloc(1, sizeof(queue_t));
#endif

	queue->size = size;

    return queue;
}

static void destroy_item(queue_item_t **item) {
    if((*item)->data) {
#ifdef __KERNEL__
		kfree((*item)->data);
#else
        free((*item)->data);
#endif
    }
#ifdef __KERNEL__
	kfree(*item);
#else
    free(*item);
#endif
} 

void queue_destroy(queue_t **queue) {
    queue_item_t *item, *tmp;

    if(*queue != NULL) {
        item = (*queue)->head;

        while(item != NULL) {
            tmp = item->next;
            destroy_item(&item);
            item = tmp;
            (*queue)->index--;
        }
#ifdef __KERNEL__
		kfree(*queue);
#else
        free(*queue);
#endif
    }
}

int queue_push_back(queue_t *queue, msg_data_t *data) {
    queue_item_t *item;

    if(queue == NULL) {
        return -1;
    }

	if(/*queue->index >= queue->size*/queue_is_full(queue)) {
		return -1;
	}

#ifdef __KERNEL__
	item = kmalloc( sizeof(queue_item_t), GFP_KERNEL );
	memset(item, 0, sizeof(queue_item_t));
#else
	item = calloc(1, sizeof(queue_item_t));
#endif

    if(item == NULL) {
        return -1;
    }
#ifdef __KERNEL__
	item->data = kmalloc(sizeof(msg_data_t), GFP_KERNEL);
	memset(item->data, 0, sizeof(msg_data_t));
#else
    item->data = calloc(1, sizeof(msg_data_t));
#endif
    if(item->data == NULL) {
#ifdef __KERNEL__
        kfree(item);
#else
        free(item);
#endif
        return -1;
    }

    memcpy(item->data, data, sizeof(msg_data_t));

    if(queue->head == NULL) {
        queue->head = item;
    } 

    if(queue->tail != NULL) {
        queue->tail->next = item;
        item->prev = queue->tail;
    }
    
    queue->tail = item;
    queue->index++;

    return 0;
}

int queue_pop_front(queue_t *queue, msg_data_t *data) {
    queue_item_t *item = NULL;

    if(queue == NULL) {
        return -1;
    }

    item = queue->head;

    if(queue->size == 0 || queue->index == 0) {
        return -1;
    }

    memcpy(data, item->data, sizeof(msg_data_t));

    queue->head = item->next;
    if(queue->head) {
        queue->head->prev = NULL;
    }        
    destroy_item(&item);
    queue->index--;

    item = queue->head;

    return 0;
}

int queue_is_full(queue_t *queue) {
    if(queue == NULL) {
        return 0;
    }
    return (queue->index >= queue->size);
}

int queue_is_empty(queue_t *queue) {
    if(queue == NULL) {
        return 1;
    }
    return  (queue->size == 0 || queue->index == 0);
}

