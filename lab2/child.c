#ifndef LAB2_CHILD_H
#define LAB2_CHILD_H

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
#include "ipc.h"
#include "banking.h"
#include "pa2345.h"
#include "child.h"
#include "var_lib.h"
#include "parent.h"
#include "logwriter.h"

void CHILD_PROC_START(Proc *this, balance_t init_bal) {

	this->bal_hist.s_id = this->this_id;
	this->bal_hist.s_history_len = 1;
	for (timestamp_t timestamp = 0; timestamp <= 255; ++timestamp){
		this->bal_hist.s_history[timestamp] = (BalanceState) { .s_balance = init_bal, .s_balance_pending_in = 0, .s_time = timestamp, };
	}
	Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED, }, };
	timestamp_t timestamp = get_physical_time();
    message.s_header.s_payload_len = strlen(message.s_payload);
	
	//!!  todo not both_writer_with_message, but both_writer + here message отдельно?
    both_writer_with_messages(&message, log_started_fmt, timestamp, this->this_id, getpid(), getppid(), this->bal_hist.s_history[timestamp].s_balance);
	//todo, do we need it - yes

	send_multicast(&me, &message);

	for (int i = 1; i <= COUNTER_OF_PROCESSES-1; i++) {
		Message msg;
		if (i != this->this_id) receive(&me, i, &msg);
	}
	both_writer(log_received_all_started_fmt, get_physical_time(), this->this_id);

	size_t Im_Not_Ready = COUNTER_OF_PROCESSES - 2;
	bool flag = true;
	
	while (flag) {
		//await, stop or transfer
		Message mesg;
		receive_any(this, &mesg);
		
		MessageType message_type = mesg.s_header.s_type;

		if (message_type == TRANSFER) {
			
			TransferOrder *transf_ord = (TransferOrder *) mesg.s_payload;
			timestamp_t time_transf = get_physical_time();
			
			BalanceHistory *bal_hist = &this->bal_hist;
			balance_t res = 0;

			if (transf_ord->s_src == this->this_id) {
				// sending transfer
				res = -transf_ord->s_amount;
				// send TRANSFER to receiver
				send(&me, transf_ord->s_dst, &mesg);
				both_writer(log_transfer_out_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_dst);

			} else if (transf_ord->s_dst == this->this_id) {
				// receiving transfer
				res = +transf_ord->s_amount;
				// answer ACK to parent
				Message ack;
				ack.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_local_time = time_transf, .s_payload_len = 0, };
				send(&me, 0, &ack);
				both_writer(log_transfer_in_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_src);
			}
			if (time_transf >= bal_hist->s_history_len) {
				bal_hist->s_history_len = time_transf + 1;
			}
			for (timestamp_t time = time_transf; time <= 255; time++) {
				bal_hist->s_history[time].s_balance += res;
			}

		}
		if (message_type == STOP) {
			flag = false;
		}
		if (message_type == DONE) {
			Im_Not_Ready--;
		}

	}

	Message somemsg = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type=DONE, }, };
	timestamp = get_physical_time();
	//todo again question about both_writer?
	both_writer_with_messages(&somemsg, log_done_fmt, timestamp, this->this_id, this->bal_hist.s_history[timestamp].s_balance);

	somemsg.s_header.s_payload_len = strlen(somemsg.s_payload);
	send_multicast(&me, &somemsg);

	while (Im_Not_Ready > 0) {
		
		Message newmsg;
		receive_any(this, &newmsg);
		
		MessageType message_type = newmsg.s_header.s_type;

		if (message_type == TRANSFER) {
            TransferOrder *transf_ord = (TransferOrder *) newmsg.s_payload;
			timestamp_t time_transf = get_physical_time();
			
			BalanceHistory *bal_hist = &this->bal_hist;
			balance_t res = 0;

			if (transf_ord->s_src == this->this_id) {
				// sending transfer
				res = -transf_ord->s_amount;
				// send TRANSFER to receiver
				send(&me, transf_ord->s_dst, &newmsg);
				both_writer(log_transfer_out_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_dst);

			} else if (transf_ord->s_dst == this->this_id) {
				// receiving transfer
				res = +transf_ord->s_amount;
				// answer ACK to parent
				Message ack;
				ack.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_local_time = time_transf, .s_payload_len = 0, };
				send(&me, PARENT_ID, &ack);
				both_writer(log_transfer_in_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_src);
			}
			if (time_transf >= bal_hist->s_history_len) {
				bal_hist->s_history_len = time_transf + 1;
			}
			for (timestamp_t time = time_transf; time <= 255; time++) {
				bal_hist->s_history[time].s_balance += res;
			}
		}
		if (message_type == DONE) {
			Im_Not_Ready--;
		}

	}

	both_writer(log_received_all_done_fmt, get_physical_time(), this->this_id);

	this->bal_hist.s_history_len = get_physical_time() + 1;
	int hist_size = sizeof(int8_t)/*local_id, it can be important in the future, don't del this comm*/ + sizeof(uint8_t) + this->bal_hist.s_history_len * sizeof(BalanceState);

    Message res = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = BALANCE_HISTORY, .s_local_time = get_physical_time(), .s_payload_len = hist_size, } };
	memcpy(&res.s_payload, &this->bal_hist, hist_size);
	send(this, PARENT_ID, &res);
}
#endif
