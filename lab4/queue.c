#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "queue.h"
#include "ipc.h"

node_t * make_node(local_id id, timestamp_t time) {
	
    node_t *node = (node_t*)malloc(sizeof(node_t));
    if (node == NULL) {
        perror("malloc");//wut is dis
        exit(-1);
    }
    //назначаем поля ноды
    node->next = NULL;//наша нода -последняя
    node->id = id;//тот самый айдишник
    node->time = time;//указанное время
    return node;
}

queue_t * make_queue(void) {
	
    queue_t *queue = (queue_t*)malloc(sizeof(queue_t));
    if (queue == NULL) {
        perror("malloc");//ошибка с выделением памяти
        exit(-1);
    }
    
    queue->len = 0;//длина 0 потому что очередь пустая
    queue->start = NULL;//в ней ничего не лежит
    return queue;
}

void del_queue(queue_t *queue) {
	
    node_t *node;
    while (queue->start) {
        node = queue->start->next;
        free(queue->start);
        queue->start = node;
    }//проходим по всем нодам и освобождаем их, передвигая начало очереди дальше
    free(queue);//все ноды оосвобождены, очередь пустая, очищаем и ее
}

void insert_into_queue(queue_t *queue, node_t *node) {
    /* Search for the place to insert. */
    node_t *current  = NULL;
    node_t *previous = NULL;
    size_t len = queue->len;
    if (queue->start == NULL) {
        queue->start = node;
        return;
    }

    for (current = queue->start; current; previous = current, current = current->next) {
        if (current->time > node->time || (current->time == node->time && current->id > node->id)) {        
            node->next = current;
            if (previous) 
                previous->next = node;
            else if (current == queue->start)
                queue->start = node;

            queue->len++;
            break;
        }
    }
    
    if (len == queue->len) {
        previous->next = node;
    }
}

node_t * first_of_queue(queue_t *queue) {
    return queue->start;
}

void del_first_of_queue(queue_t *queue) {
    node_t *next = queue->start->next;
    free(queue->start);
    queue->start = next;
}
//выводим по очереди все процессы из очереди
void print_queue(queue_t *queue, int id) {
    node_t *node = queue->start;
    for (int i = 0; node; node = node->next, i++) {
        fprintf(stderr, "Process %d #%d: ID[%d] TIME[%d]\n", id, i, node->id, node->time);
    }
}

