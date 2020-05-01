#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include "ipc.h"
#include "banking.h"
#include "var_lib.h"
#include "parent.h"

void PARENT_PROC_START(Proc *this){

	for (size_t i=1; i<= COUNTER_OF_CHILDREN; i++){
        Message msg;
        if (i!= this_id) receive(NULL,i,&msg);
    }
    both_writer(log_received_all_started_fmt, this_id);

	bank_robbery(this, COUNTER_OF_PROCESSES - 1 );
	
	Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STOP, .s_payload_len = 0, .s_local_time = get_physical_time(), }, };
	send_multicast(&me, &message);

	for (int i = 1; i<=COUNTER_OF_CHILDREN; i++){
        Message message;
        if (i != this_id) receive(NULL, i, &message);
    }
    //write to log & comm line
    both_writer(log_received_all_done_fmt, this_id);
	
	this->all_hist.s_history_len = COUNTER_OF_CHILDREN;
	for (int i = 1; i <= COUNTER_OF_CHILDREN; i++) {
		Message message;
		receive(&me, i, &message);
		int16_t message_type = message.s_header.s_type;
		if (message_type == BALANCE_HISTORY) {
		BalanceHistory *children_hist = (BalanceHistory *) &message.s_payload;
		this->all_hist.s_history[i-1] = *children_hist;		
		}
		else {
			return 1;		
		}	
	
	}
	
	for(int i=1; i<COUNTER_OF_PROCESSES; i++){
            waitpid(proc_pidts[i], NULL, 0);
        }
	print_history(&this->all_hist);
}
