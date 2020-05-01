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
#include "banking.h"
#include "ipc.h"
#include "pa2345.h"
#include "child.h"
#include "var_lib.h"
#include "parent.h"

void CHILD_PROC_START(Proc *this, balance_t init_bal) {

	this->bal_hist.s_id = this->id;
	this->bal_hist.s_history_len = 1;
	for (timestamp_t timestamp = 1; time < MAX_T; time++){
		this->bal_hist.s_history[timestamp] = (BalanceState) { .s_balance = init_bal, .s_balance_pending_in = 0, .s_time = timestamp, };
	}


	Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED, }, };
	timestamp_t timestamp = get_physical_time();
	both_writer_with_messages(&message, log_started_fmt, timestamp, this->id, getpid(), getppid(), this->bal_hist.s_history[time].s_payload);
	send_multicast(&me, &message);

	//////

	receive started from all
	
	transact

	all done
	
	

}

