
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

void CHILD_PROC_START(Proc *this){//, balance_t init_bal) {

    //todo delete now?
	/*this->bal_hist.s_id = this->this_id;
	this->bal_hist.s_history_len = 1;
	for (timestamp_t timestamp = 0; timestamp <= 255; ++timestamp){
		this->bal_hist.s_history[timestamp] = (BalanceState) {
			.s_balance = init_bal,
			.s_balance_pending_in = 0,
			.s_time = timestamp,
		};
	}*/

	this->lamp_time++;

	//send this started
	Message message = {
		.s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED, .s_local_time = get_lamport_time(), },
	};

	//todo delete noe next 4?
	//timestamp_t timestamp = get_lamport_time();
    // both_writer_with_messages(&message, log_started_fmt, timestamp,
    //this->this_id, getpid(), getppid(), 0);
	//this->bal_hist.s_history[timestamp].s_balance);
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

	//both_writer(log_received_all_started_fmt,
	//	get_lamport_time(), this->this_id);

	if (this->mutex==1) request_cs(this);
	for (int i=1; i<= this->this_id*5; i++) {
        //both_writer(log_loop_operation_fmt, this->this_id, i, this->this_id*5);
		size_t slen = snprintf(NULL, 0, log_loop_operation_fmt, this->this_id, i, this->this_id*5);
		char *str = (char*)alloca(slen+1);
		snprintf(str, slen+1, log_loop_operation_fmt, this->this_id, i, this->this_id*5);
		print(str);
	}
    if (this->mutex==1) release_cs(this);


    //todo delete now?
	/*size_t Im_Not_Ready = COUNTER_OF_PROCESSES - 2;
	bool flag = true;

	while (flag) {
		//await, stop or transfer
		Message mesg;

		receive_any(this, &mesg);
		if (this->lamp_time < mesg.s_header.s_local_time) this->lamp_time=mesg.s_header.s_local_time;
		this->lamp_time++;

		MessageType message_type = mesg.s_header.s_type;

		if (message_type == TRANSFER) {

			TransferOrder *transf_ord = (TransferOrder *) mesg.s_payload;
			timestamp_t time_transf = mesg.s_header.s_local_time;//get_physical_time();

			BalanceHistory *bal_hist = &this->bal_hist;
			balance_t res = 0;

			if (transf_ord->s_src == this->this_id) {
				// sending transfer
				if (this->lamp_time < time_transf) this->lamp_time=time_transf;
				this->lamp_time++;
				timestamp_t send_time = get_lamport_time();
				for (timestamp_t i = send_time; i<=255; i++){
					bal_hist->s_history[i].s_balance -= transf_ord->s_amount;
				}
				//res = -transf_ord->s_amount;
				// send TRANSFER to receiver
				mesg.s_header.s_local_time = send_time;
				send(&me, transf_ord->s_dst, &mesg);
				both_writer(log_transfer_out_fmt, send_time,
					this->this_id, transf_ord->s_amount, transf_ord->s_dst);

			} else if (transf_ord->s_dst == this->this_id) {
				// receiving transfer
				if (this->lamp_time < time_transf) this->lamp_time=time_transf;
				this->lamp_time++;

				//todo delete it here?
				res = +transf_ord->s_amount;

				timestamp_t rec_time = get_lamport_time();

				for (timestamp_t i = time_transf; i<rec_time; i++){
					bal_hist->s_history[i].s_balance_pending_in += res;//transf_ord->s_amount;
				}
				for (timestamp_t i = rec_time; i<=255; i++){
					bal_hist->s_history[i].s_balance += res;//transf_ord->s_amount;
				}

				// answer ACK to parent
				Message ack;
				ack.s_header = (MessageHeader) {
					.s_magic = MESSAGE_MAGIC,
					.s_type = ACK,
					.s_local_time = get_lamport_time(),
					.s_payload_len = 0,
				};
				send(&me, PARENT_ID, &ack);
				both_writer(log_transfer_in_fmt, get_lamport_time(),
					this->this_id, transf_ord->s_amount, transf_ord->s_src);
			}

		}
		if (message_type == STOP) {
			flag = false;
			if (this->lamp_time < mesg.s_header.s_local_time) this->lamp_time=mesg.s_header.s_local_time;
			//this->lamp_time++;
		}
		if (message_type == DONE) {
			Im_Not_Ready--;
			if (this->lamp_time < mesg.s_header.s_local_time) this->lamp_time=mesg.s_header.s_local_time;
			//this->lamp_time++;
		}

	}*/

	//Message
	this->lamp_time++;
	Message somemsg = {
		.s_header = {
			.s_magic = MESSAGE_MAGIC,
			.s_type=DONE,
			.s_local_time=get_lamport_time(),
		}, };
	//timestamp_t timestamp = get_lamport_time();
	//both_writer_with_messages(&somemsg, log_done_fmt, timestamp,
	//	this->this_id, this->bal_hist.s_history[timestamp].s_balance);

	somemsg.s_header.s_payload_len = strlen(somemsg.s_payload);
	send_multicast(&me, &somemsg);

	/*while (Im_Not_Ready > 0) {

		Message newmsg;
		receive_any(this, &newmsg);

		if (this->lamp_time < newmsg.s_header.s_local_time) this->lamp_time=newmsg.s_header.s_local_time;
		this->lamp_time++;


		MessageType message_type = newmsg.s_header.s_type;

		if (message_type == TRANSFER) {
            //process_transfer_order(this, &newmsg);
			TransferOrder *transf_ord = (TransferOrder *) newmsg.s_payload;
			timestamp_t time_transf = newmsg.s_header.s_local_time;//get_physical_time();

			BalanceHistory *bal_hist = &this->bal_hist;
			balance_t res = 0;

			if (transf_ord->s_src == this->this_id) {
				// sending transfer
				if (this->lamp_time < time_transf) this->lamp_time=time_transf;
				this->lamp_time++;
				timestamp_t send_time = get_lamport_time();
				//res = -transf_ord->s_amount;
				for (timestamp_t i = send_time; i<=MAX_T; i++){
					bal_hist->s_history[i].s_balance -= transf_ord->s_amount;
				}

				// send TRANSFER to receiver
				newmsg.s_header.s_local_time = send_time;
				send(&me, transf_ord->s_dst, &newmsg);
				both_writer(log_transfer_out_fmt, send_time,
							this->this_id, transf_ord->s_amount, transf_ord->s_dst);

			} else if (transf_ord->s_dst == this->this_id) {
				// receiving transfer
				if (this->lamp_time < time_transf) this->lamp_time=time_transf;
				this->lamp_time++;

				//todo delete it here?
				res = +transf_ord->s_amount;

				timestamp_t rec_time = get_lamport_time();

				for (timestamp_t i = time_transf; i<rec_time; i++){
					bal_hist->s_history[i].s_balance_pending_in += res;//transf_ord->s_amount;
				}
				for (timestamp_t i = rec_time; i<=255; i++){
					bal_hist->s_history[i].s_balance += res;//transf_ord->s_amount;
				}

				// answer ACK to parent
				Message ack;
				ack.s_header = (MessageHeader) {
						.s_magic = MESSAGE_MAGIC,
						.s_type = ACK,
						.s_local_time = get_lamport_time(),
						.s_payload_len = 0,
				};
				send(&me, PARENT_ID, &ack);
				both_writer(log_transfer_in_fmt, get_lamport_time(),
							this->this_id, transf_ord->s_amount, transf_ord->s_src);
			}
		}
		if (message_type == DONE) {
			Im_Not_Ready--;
			if (this->lamp_time < newmsg.s_header.s_local_time) this->lamp_time=newmsg.s_header.s_local_time;
		}

	}*/
	while(!this->received_msg >= COUNTER_OF_PROCESSES-2){
	    Message message1;
	    receive_any(this, &message1);
        if (this->lamp_time < message1.s_header.s_local_time) this->lamp_time=message1.s_header.s_local_time;
        this->lamp_time++;
        //MessageType message_type = message1.s_header.s_type;
        this->received_msg++;
	}

	/*both_writer(log_received_all_done_fmt,
		get_lamport_time(), this->this_id);

	this->lamp_time++;
	this->bal_hist.s_history_len = get_lamport_time() + 1;
	int hist_size = 2 * sizeof(uint8_t) +
					this->bal_hist.s_history_len * sizeof(BalanceState);

    Message res = {
		.s_header = {
			.s_magic = MESSAGE_MAGIC,
			.s_type = BALANCE_HISTORY,
			.s_local_time = get_lamport_time(),
			.s_payload_len = hist_size,
		} };
	memcpy(&res.s_payload, &this->bal_hist, hist_size);
	send(this, PARENT_ID, &res);*/
}

