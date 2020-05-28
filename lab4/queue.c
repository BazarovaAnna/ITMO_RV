#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "ipc.h"
#include "queue.h"

node_t *
node_create(local_id id, timestamp_t time) {
    node_t *node = (node_t*)malloc(sizeof(node_t));
    if (node == NULL) {
        perror("malloc");
        exit(-1);
    }
    node->next = NULL;
    node->id = id;
    node->time = time;
    return node;
}

queue_t *
queue_create(void) {
    queue_t *q = (queue_t*)malloc(sizeof(queue_t));
    if (q == NULL) {
        perror("malloc");
        exit(-1);
    }
    q->len = 0;
    q->head = NULL;
    return q;
}

void
queue_destroy(queue_t *q) {
    node_t *node;
    while (q->head) {
        node = q->head->next;
        free(q->head);
        q->head = node;
    }
    free(q);
}

void
queue_insert(queue_t *q, node_t *n) {
    /* Search for the place to insert. */
    node_t *cur  = NULL;
    node_t *prev = NULL;
    size_t len = q->len;
    if (q->head == NULL) {
        q->head = n;
        return;
    }

    for (cur = q->head; cur; prev = cur, cur = cur->next) {
        if (cur->time > n->time || (cur->time == n->time && cur->id > n->id)) {        
            n->next = cur;
            if (prev) 
                prev->next = n;
            else if (cur == q->head)
                q->head = n;

            q->len++;
            break;
        }
    }
    if (len == q->len) {
        prev->next = n;
    }
}

node_t *
queue_first(queue_t *q) {
    return q->head;
}

void
queue_delete_first(queue_t *q) {
    node_t *next = q->head->next;
    free(q->head);
    q->head = next;
}

void
queue_print(queue_t *q, int id) {
    node_t *t = q->head;
    for (int i = 0; t; t = t->next, i++) {
        fprintf(stderr, "Process %d #%d: ID[%d] TIME[%d]\n", id, i, t->id, t->time);
    }
}

/*
int
main(int argc, char *argv[]) {

    queue_t *q = queue_create();

    queue_insert(q, node_create(2,1));
    queue_insert(q, node_create(3,2));
    queue_insert(q, node_create(4,1));
    queue_insert(q, node_create(1,2));
    queue_insert(q, node_create(5,1));
    queue_insert(q, node_create(6,2));
    queue_insert(q, node_create(0,1));

    queue_print(q);
    
    queue_destroy(q);
    return 0;
}
*/
