#ifndef __IFMO_DISTRIBUTED_CLASS_PA2_ALLEGORY__VAR__LIB__H
#define __IFMO_DISTRIBUTED_CLASS_PA2_ALLEGORY__VAR__LIB__H

#include <fcntl.h>
#include "banking.h"
#include "ipc.h"

#define CONVERT_SELF_TO(ref_type, var_name) ref_type* this = ((ref_type *) var_name)

typedef struct {
	local_id id;
	timestamp_t timestamp;
} Element;

typedef struct {
	//timestamp_t timestamp[64];
	//local_id id[64];
    Element element[128];
	int queue_size;
} Queue;

typedef struct{
	local_id this_id;
	timestamp_t lamp_time;
	int received_msg;
	Queue queue;
} Proc;

//added
Proc me;
pid_t proc_pidts[10];
int mutex;
size_t reader_pipe[10][10];
size_t writer_pipe[10][10];
size_t COUNTER_OF_PROCESSES;

#endif
