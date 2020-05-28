#ifndef __IFMO_DISTCOMP_QUEUE_H__
#define __IFMO_DISTCOMP_QUEUE_H__

typedef struct _node_t {
    struct _node_t *next;
    local_id id;
    timestamp_t time;
} node_t;

typedef struct _queue_t {
    node_t *head;
    size_t len;
} queue_t;

node_t *node_create(local_id id, timestamp_t time);

queue_t *queue_create(void);

void queue_destroy(queue_t *q);

node_t *queue_first(queue_t *q);
void queue_delete_first(queue_t *q);
void queue_insert(queue_t *q, node_t *n);
void queue_print(queue_t *q, int id);


#endif /* __IFMO_DISTCOMP_QUEUE_H__ */
