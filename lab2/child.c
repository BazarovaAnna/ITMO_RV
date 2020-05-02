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

	this->bal_hist.s_id = this->this_id;
	this->bal_hist.s_history_len = 1;
	for (timestamp_t timestamp = 1; time < MAX_T; time++){
		this->bal_hist.s_history[timestamp] = (BalanceState) { .s_balance = init_bal, .s_balance_pending_in = 0, .s_time = timestamp, };
	}


	Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED, }, };
	timestamp_t timestamp = get_physical_time();
	both_writer_with_messages(&message, log_started_fmt, timestamp, this->this_id, getpid(), getppid(), this->bal_hist.s_history[time].s_payload);
	//todo, do we need it?
	message.s_header.s_payload_len = strlen(message.s_payload);
	send_multicast(&me, &message);

	//todo by both writer + here message


	for (int i = 1; i <= COUNTER_OF_PROCESSES; i++) {
		Message message;
		if (i == this->this_id) {
			continue;
		}
		receive(&me, i, &message);
	}
	both_writer(log_received_all_started_fmt, this->this_id);


	//todo refactor futher

	bool loop = true;
	size_t not_ready = COUNTER_OF_PROCESSES - 2;

	while (loop) {
		//await stop or transfer
		Message message;
		receive_any(this, &message);
		MessageType message_type = message.s_header.s_type;

		if (message_type == TRANSFER) {
			//todo
			TransferOrder *transf_ord = (TransferOrder *) &(message->s_payload);
			timestamp_t time_of_transfer = get_physical_time();
			BalanceHistory *bal_history = &this->bal_hist;
			balance_t diff = 0;

			if (transf_ord->s_src == this->id) {
				// sending transfer
				diff = -transf_ord->s_amount;
				// send TRANSFER to receiver
				send(&me, transf_ord->s_dst, message);
				logprintf(log_transfer_out_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_dst);

			} else if (transf_ord->s_dst == this->this_id) {
				// receiving transfer
				diff = +transf_ord->s_amount;
				// answer ACK to parent
				Message ack;
				ack.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_local_time = time_of_transfer, .s_payload_len = 0, };
				send(&me, PARENT_ID, &ack);
				logprintf(log_transfer_in_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_src);
			}
			if (time_of_transfer >= bal_history->s_history_len) {
				bal_history->s_history_len = time_of_transfer + 1;
			}
			for (timestamp_t timestamp = time_of_transfer; timestamp <= MAX_T; timestamp++) {
				bal_history->s_history[timestamp].s_balance += diff;
			}

		}
		if (message_type == STOP) {
			loop = false;
		}
		if (message_type == DONE) {
			not_ready--;
		}

	}


	Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type=DONE,}, };
	timestamp_t timestamp = get_physical_time();
	both_writer_with_messages(&message, log_done_fmt, timestamp, this->this_id, getpid(), getppid(), this->bal_hist.s_history[time].s_payload);
	//sprintf(message.s_payload, log_done_fmt, this_id);
	//todo, do we need it?
	message.s_header.s_payload_len = strlen(message.s_payload);
	send_multicast(&me, &message);


	while (not_ready > 0) {
		DEBUG("Process %d is STOPPED and waiting for another TRANSFER or DONE\n", self->id);
		Message message;
		receive_any(self, &message);
		MessageType message_type = message.s_header.s_type;

		if (message_type == TRANSFER) {
			//todo
			TransferOrder *transf_ord = (TransferOrder *) &(message->s_payload);
			timestamp_t time_of_transfer = get_physical_time();
			BalanceHistory *bal_history = &this->bal_hist;
			balance_t diff = 0;

			if (transf_ord->s_src == this->id) {
				// sending transfer
				diff = -transf_ord->s_amount;
				// send TRANSFER to receiver
				send(&me, transf_ord->s_dst, message);
				logprintf(log_transfer_out_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_dst);

			} else if (transf_ord->s_dst == this->this_id) {
				// receiving transfer
				diff = +transf_ord->s_amount;
				// answer ACK to parent
				Message ack;
				ack.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_local_time = time_of_transfer, .s_payload_len = 0, };
				send(&me, PARENT_ID, &ack);
				logprintf(log_transfer_in_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_src);
			}
			if (time_of_transfer >= bal_history->s_history_len) {
				bal_history->s_history_len = time_of_transfer + 1;
			}
			for (timestamp_t timestamp = time_of_transfer; timestamp <= MAX_T; timestamp++) {
				bal_history->s_history[timestamp].s_balance += diff;
			}
		}
		if (message_type == DONE) {
			not_ready--;
		}

	}

	both_writer(log_received_all_done_fmt, this->this_id);

	// 0 < ++
	for (int i = 1; i <= this->bal_hist.s_history_len; i++) {
		BalanceState state = this->bal_hist.s_history[i];
	}

	this->bal_hist.s_history_len = get_physical_time() + 1;
	int hist_size = sizeof(local_id) + sizeof(uint8_t) +
							 this->bal_hist.s_history_len * sizeof(BalanceState);

	Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = BALANCE_HISTORY, .s_local_time = get_physical_time(), .s_payload_len = size_of_history, } };
	memcpy(&message.s_payload, &this->bal_hist, size_of_history);
	send(this, PARENT_ID, &message);
	

}

