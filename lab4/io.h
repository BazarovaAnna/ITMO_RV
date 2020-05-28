#ifndef __IO__
#define __IO__

#include <stdio.h>
#include "ipc.h"

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

#endif 
