#ifndef LAB2_PARENT_H
#define LAB2_PARENT_H

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <zconf.h>
#include "ipc.h"
#include "banking.h"
#include "var_lib.h"
#include "parent.h"
#include "pa2345.h"
#include "logwriter.h"

//static FILE * log;
void PARENT_PROC_START(Proc *this){

   // log = fopen(events_log, "w");
    for (size_t i=1; i<= COUNTER_OF_PROCESSES - 1; i++){
        Message msg;
        if (i!= this->this_id) receive(NULL,i,&msg);
    }
    both_writer(log_received_all_started_fmt, this->this_id);

	bank_robbery(this, COUNTER_OF_PROCESSES - 1 );
	
	Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STOP, .s_payload_len = 0, .s_local_time = get_physical_time(), }, };
	send_multicast(&me, &message);

	for (int i = 1; i<=COUNTER_OF_PROCESSES - 1; i++){
        Message message;
        if (i != this->this_id) receive(NULL, i, &message);
    }
    //write to log & comm line
    both_writer(log_received_all_done_fmt, this->this_id);
	
	this->all_hist.s_history_len = COUNTER_OF_PROCESSES - 1;
	for (int i = 1; i <= COUNTER_OF_PROCESSES - 1; i++) {
		Message message;
		receive(&me, i, &message);
		int16_t message_type = message.s_header.s_type;
		if (message_type == BALANCE_HISTORY) {
		BalanceHistory *children_hist = (BalanceHistory *) &message.s_payload;
		this->all_hist.s_history[i-1] = *children_hist;		
		}
	
	}
	
	for(int i=1; i<COUNTER_OF_PROCESSES; i++){
            waitpid(proc_pidts[i], NULL, 0);
        }
	print_history(&this->all_hist);
	//fclose(log);
}


/*void both_writer(const char *message, ...){
    va_list list;
    //write to event log file
    va_start(list,message);
    vfprintf(log, message, list);
    va_end(list);

    //write to console
    va_start (list, message);
    vprintf(message, list);
    va_end(list);
}

void both_writer_with_messages(Message *const message, const char *frmt, ...){
    va_list list;
    //write to event log file
    va_start(list,frmt);
    vfprintf(log, frmt, list);
    va_end(list);

    //write to console
    va_start (list, frmt);
    vprintf(frmt, list);
    va_end(list);

    va_start(list, frmt);
    size_t payload_lenght = sprintf(message -> s_payload, frmt, list);
    message->s_header.s_payload_len = payload_lenght;
    va_end(list);
}
*/
#endif
