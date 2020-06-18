#ifndef __CHILD__
#define __CHILD__

#include <stdio.h>
#include "queue.h"
#include "banking.h"
#include "common.h"
#include "pa2345.h"

#define MAX_PROC    10
#define MAX_PIPES   110
#define NUM_FD      2
#define READ_FD     0
#define WRITE_FD    1

typedef struct {
    local_id procnum;
   
    FILE *events_log_stream;
    FILE *pipes_log_stream;

    int fds[MAX_PROC+1][MAX_PROC+1][NUM_FD];

    int mutexl;
} IO;

typedef struct {
    const IO *io;              
    queue_t *queue;            
    local_id self_id;          
    local_id running_processes; 
} proc_t;

int child(IO *io, local_id id);

void close_unsed_fds(IO *io, local_id id);

void set_l_t(timestamp_t time);

timestamp_t get_l_t(void);

void inc_l_t(void);
#endif 
