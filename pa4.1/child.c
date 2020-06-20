
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <alloca.h>
#include "ipc.h"
#include "banking.h"
#include "pa2345.h"
#include "child.h"
#include "var_lib.h"
#include "parent.h"
#include "logwriter.h"

void CHILD_PROC_START(Proc *this){

	this->lamp_time++;
	//send this started
	Message message = {
		.s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED, .s_local_time = get_lamport_time(), },
	};
    message.s_header.s_payload_len = strlen(message.s_payload);
	send_multicast(&me, &message);

	//awaiting other started
	for (int i = 1; i <= COUNTER_OF_PROCESSES-1; i++) {
		Message msg;
		if (i != this->this_id) {
			receive(&me, i, &msg);
			if (this->lamp_time < msg.s_header.s_local_time) this->lamp_time=msg.s_header.s_local_time;
			this->lamp_time++;
		}
	}
	if (mutex==1) request_cs(this);
    for (int i=1; i<= this->this_id*5; ++i) {
		size_t slen = snprintf(NULL, 0, log_loop_operation_fmt, this->this_id, i, this->this_id*5);
		char *str = (char*)alloca(slen+1);
		snprintf(str, slen+1, log_loop_operation_fmt, this->this_id, i, this->this_id*5);
		print(str);
	}
    if (mutex==1) release_cs(this);
	//Message
	this->lamp_time++;
	Message somemsg = {
		.s_header = { .s_magic = MESSAGE_MAGIC, .s_type=DONE, .s_local_time=get_lamport_time(), },
	};
	somemsg.s_header.s_payload_len = strlen(somemsg.s_payload);
	send_multicast(&me, &somemsg);

	while(this->received_msg < COUNTER_OF_PROCESSES-2){
	    Message message1;
	    receive_any(this, &message1);
        if (this->lamp_time < message1.s_header.s_local_time) this->lamp_time=message1.s_header.s_local_time;
        this->lamp_time++;
        //here was switch
        this->received_msg++;
	}
}

