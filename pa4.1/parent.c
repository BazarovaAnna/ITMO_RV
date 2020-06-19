
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

#include <stdarg.h>
#include "ipc.h"
#include "banking.h"
#include "var_lib.h"
#include "parent.h"
#include "pa2345.h"
#include "logwriter.h"

void PARENT_PROC_START(Proc *this){

   // log = fopen(events_log, "w"); OLD
    for (size_t i=1; i<= COUNTER_OF_PROCESSES - 1; i++){
        Message msg;
        if (i!= this->this_id)
		{
        	receive(&me,i,&msg);
        	if (this->lamp_time < msg.s_header.s_local_time) this->lamp_time=msg.s_header.s_local_time;
			this->lamp_time++;
		}
    }
    //todo delete now?
    /*both_writer(log_received_all_started_fmt, get_lamport_time(), this->this_id);
	bank_robbery(this, COUNTER_OF_PROCESSES - 1 );

	this->lamp_time++;
	Message message = { 
		.s_header = { 
			.s_magic = MESSAGE_MAGIC, 
			.s_type = STOP, 
			.s_payload_len = 0, 
			.s_local_time = get_lamport_time(),
		}, };
	send_multicast(&me, &message);*/

	for (int i = 1; i<=COUNTER_OF_PROCESSES - 1; i++){
        Message msg;
        if (i != this->this_id) {
        	receive(&me, i, &msg);
        	if (this->lamp_time < msg.s_header.s_local_time) this->lamp_time=msg.s_header.s_local_time;
			this->lamp_time++;
        }

    }
    //write to log & comm line
    //todo delete now?
    /* both_writer(log_received_all_done_fmt, get_lamport_time(), this->this_id);
	
	this->all_hist.s_history_len = COUNTER_OF_PROCESSES - 1;
	for (int i = 1; i <= COUNTER_OF_PROCESSES - 1; i++) {
		Message mess;
		receive(&me, i, &mess);
		int16_t message_type = mess.s_header.s_type;
		if (message_type == BALANCE_HISTORY) {
			BalanceHistory *children_hist = (BalanceHistory *) &mess.s_payload;
			this->all_hist.s_history[i-1] = *children_hist;
			if (this->lamp_time < mess.s_header.s_local_time) this->lamp_time=mess.s_header.s_local_time;
			this->lamp_time++;
		}
	
	}*/

    //todo <=???
	for(int i=1; i<COUNTER_OF_PROCESSES; i++){
            waitpid(proc_pidts[i], NULL, 0);
        }
	//print_history(&this->all_hist);

}


