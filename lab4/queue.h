#ifndef __QUEUE__
#define __QUEUE__

#include "ipc.h"

typedef struct _node_t {
    struct _node_t *next;
    local_id id;
    timestamp_t time;
} node_t;

typedef struct _queue_t {
    node_t *start;
    size_t len;
} queue_t;

node_t *make_node(local_id id, timestamp_t time);
queue_t *make_queue(void);

void del_queue(queue_t *queue);

node_t *first_of_queue(queue_t *queue);

void del_first_of_queue(queue_t *queue);
void insert_into_queue(queue_t *queue, node_t *node);
void print_queue(queue_t *queue, int id);

#endif
