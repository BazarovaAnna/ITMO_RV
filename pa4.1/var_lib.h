#ifndef __IFMO_DISTRIBUTED_CLASS_PA2_ALLEGORY__VAR__LIB__H
#define __IFMO_DISTRIBUTED_CLASS_PA2_ALLEGORY__VAR__LIB__H

#include <fcntl.h>
#include "banking.h"
#include "ipc.h"

typedef struct {
    local_id id[128];
    timestamp_t timestamp[128];
    int queue_size;
} Queue;

typedef struct{
	//BalanceHistory bal_hist;
	local_id this_id;
	//AllHistory all_hist;
	timestamp_t lamp_time;
	int mutex;
	int received_msg;
	Queue queue;
} Proc;

//added
Proc me;
pid_t proc_pidts[10];

//todo delete
//balance_t BANK_ACCOUNTS[10];

//was
size_t reader_pipe[10][10];
size_t writer_pipe[10][10];
size_t COUNTER_OF_PROCESSES;

#endif
