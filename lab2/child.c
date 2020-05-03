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
#include "banking.h"
#include "ipc.h"
#include "pa2345.h"
#include "child.h"
#include "var_lib.h"
#include "parent.h"
#include "logwriter.h"

//static  FILE *log;
void process_transfer_order(Proc *self, Message *message);
void CHILD_PROC_START(Proc *this, balance_t init_bal) {





	this->bal_hist.s_id = this->this_id;
	this->bal_hist.s_history_len = 1;
	for (timestamp_t timestamp = 0; timestamp <= MAX_T; ++timestamp){
		this->bal_hist.s_history[timestamp] = (BalanceState) { .s_balance = init_bal, .s_balance_pending_in = 0, .s_time = timestamp, };
	}


	Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED, }, };
	timestamp_t timestamp = get_physical_time();
    message.s_header.s_payload_len = strlen(message.s_payload);
    both_writer_with_messages(&message, log_started_fmt, timestamp, this->this_id, getpid(), getppid(), this->bal_hist.s_history[timestamp].s_balance);
	//todo, do we need it?

	send_multicast(&me, &message);

	//todo by both writer + here message


	for (int i = 1; i <= COUNTER_OF_PROCESSES-1; i++) {
		Message mes;
		if (i == this->this_id) {
			continue;
		}
		receive(&me, i, &mes);
	}
	both_writer(log_received_all_started_fmt, get_physical_time(), this->this_id);


	//todo refactor futher

	bool loop = true;
	size_t not_ready = COUNTER_OF_PROCESSES - 2;

	while (loop) {
		//await stop or transfer
		Message mess;
		receive_any(this, &mess);
		MessageType message_type = mess.s_header.s_type;

		if (message_type == TRANSFER) {
			//todo
			TransferOrder *transf_ord = (TransferOrder *) mess.s_payload;
			timestamp_t time_of_transfer = get_physical_time();
			BalanceHistory *bal_history = &this->bal_hist;
			balance_t diff = 0;

			if (transf_ord->s_src == this->this_id) {
				// sending transfer
				diff = -transf_ord->s_amount;
				// send TRANSFER to receiver
				send(&me, transf_ord->s_dst, &mess);
				both_writer(log_transfer_out_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_dst);

			} else if (transf_ord->s_dst == this->this_id) {
				// receiving transfer
				diff = +transf_ord->s_amount;
				// answer ACK to parent
				Message ack;
				ack.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_local_time = time_of_transfer, .s_payload_len = 0, };
				send(&me, PARENT_ID, &ack);
				both_writer(log_transfer_in_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_src);
			}
			if (time_of_transfer >= bal_history->s_history_len) {
				bal_history->s_history_len = time_of_transfer + 1;
			}
			for (timestamp_t time = time_of_transfer; time <= MAX_T; time++) {
				bal_history->s_history[time].s_balance += diff;
			}

		}
		if (message_type == STOP) {
			loop = false;
		}
		if (message_type == DONE) {
			not_ready--;
		}

	}


	//Message
	Message somemsg = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type=DONE,}, };
	//timestamp_t
	timestamp = get_physical_time();
	both_writer_with_messages(&somemsg, log_done_fmt, timestamp, this->this_id, this->bal_hist.s_history[timestamp].s_balance);
	//sprintf(message.s_payload, log_done_fmt, this_id);
	//todo, do we need it?
	somemsg.s_header.s_payload_len = strlen(somemsg.s_payload);
	send_multicast(&me, &somemsg);


	while (not_ready > 0) {
		Message newmsg;
		receive_any(this, &newmsg);
		MessageType message_type = newmsg.s_header.s_type;

		if (message_type == TRANSFER) {
            process_transfer_order(this, &newmsg);
			//todo
			/*TransferOrder *transf_ord = (TransferOrder *) &(message->s_payload);
			timestamp_t time_of_transfer = get_physical_time();
			BalanceHistory *bal_history = &this->bal_hist;
			balance_t diff = 0;

			if (transf_ord->s_src == this->this_id) {
				// sending transfer
				diff = -transf_ord->s_amount;
				// send TRANSFER to receiver
				send(&me, transf_ord->s_dst, &message);
				both_writer(log_transfer_out_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_dst);

			} else if (transf_ord->s_dst == this->this_id) {
				// receiving transfer
				diff = +transf_ord->s_amount;
				// answer ACK to parent
				Message ack;
				ack.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_local_time = time_of_transfer, .s_payload_len = 0, };
				send(&me, PARENT_ID, &ack);
				both_writer(log_transfer_in_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_src);
			}
			if (time_of_transfer >= bal_history->s_history_len) {
				bal_history->s_history_len = time_of_transfer + 1;
			}
			for (timestamp_t timest = time_of_transfer; timest <= MAX_T; timest++) {
				bal_history->s_history[timest].s_balance += diff;
			}*/

		}
		if (message_type == DONE) {
			not_ready--;
		}

	}

	both_writer(log_received_all_done_fmt, get_physical_time(), this->this_id);

	// 0 < ++
	//for (int i = 1; i <= this->bal_hist.s_history_len; i++) {
	//	BalanceState state = this->bal_hist.s_history[i];
	//}

	this->bal_hist.s_history_len = get_physical_time() + 1;
	int hist_size = sizeof(local_id) + sizeof(uint8_t) +
							 this->bal_hist.s_history_len * sizeof(BalanceState);

    Message res = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = BALANCE_HISTORY, .s_local_time = get_physical_time(), .s_payload_len = hist_size, } };
	memcpy(&res.s_payload, &this->bal_hist, hist_size);
	send(this, PARENT_ID, &res);


}







void process_transfer_order(Proc *this, Message *message) {
    TransferOrder *transf_ord = (TransferOrder *) &(message->s_payload);
    timestamp_t time_of_transfer = get_physical_time();
    BalanceHistory *bal_history = &this->bal_hist;
    balance_t diff = 0;

    if (transf_ord->s_src == this->this_id) {
        // sending transfer
        diff = -transf_ord->s_amount;
        // send TRANSFER to receiver
        send(&me, transf_ord->s_dst, message);
        both_writer(log_transfer_out_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_dst);

    } else if (transf_ord->s_dst == this->this_id) {
        // receiving transfer
        diff = +transf_ord->s_amount;
        // answer ACK to parent
        Message ack;
        ack.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_local_time = time_of_transfer, .s_payload_len = 0, };
        send(&me, PARENT_ID, &ack);
        both_writer(log_transfer_in_fmt, get_physical_time(), this->this_id, transf_ord->s_amount, transf_ord->s_src);
    }
    if (time_of_transfer >= bal_history->s_history_len) {
        bal_history->s_history_len = time_of_transfer + 1;
    }
    for (timestamp_t timest = time_of_transfer; timest <= MAX_T; timest++) {
        bal_history->s_history[timest].s_balance += diff;
    }
}
#endif
