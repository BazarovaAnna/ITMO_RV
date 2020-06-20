#include <signal.h>
#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "common.h"
#include "ipc.h"
#include "var_lib.h"

//added
#include <errno.h>
#include <stdbool.h>
#include "banking.h"
#include "child.h"
#include "parent.h"
#include "logwriter.h"
void pipes_log_writer(const char *message, ...);

static  FILE *pipes;
static const char * const pipes_log_mes_r = "Pipe from %i to %i is opened for reading\n";
static const char * const pipes_log_mes_w = "Pipe from %i to %i is opened for writing\n";
static const char * const pipes_log_mes_r_cl = "Pipe from %i to %i is closed for reading by %i\n";
static const char * const pipes_log_mes_w_cl = "Pipe from %i to %i is closed for writing by %i\n";

int main(int argc, char *argv[]) {

    //description (INIT)
    size_t COUNTER_OF_CHILDREN;//HOW MANY ACCS
    Proc *this = &me;
	mutex = 0;

    int opt = 1;
    while (opt < argc){
        if (strcmp(argv[opt], "-p") == 0){
            opt++;
            COUNTER_OF_CHILDREN = strtol(argv[opt], NULL, 10);
        } else if (strcmp(argv[opt], "--mutex") == 0){
                mutex=1;
        }
        opt++;
    }

    //IF SMTH GOES WRONG THIS if WORKS
    if (COUNTER_OF_CHILDREN==0){
        fprintf(stderr, "Error: you should use key like a '-p NUMBER_OF_CHILDREN CHILDREN!'\n");
        return 1;
    }
    mutex = 1;
	//opening pipe file
    pipes = fopen(pipes_log, "w");

    //creating descriptors to send and read from i to j
    for (int i=0; i<=COUNTER_OF_CHILDREN;i++){
        for (int j=0; j<=COUNTER_OF_CHILDREN;j++){
            if (i!=j) {//can't be child for itself
                int fields[2];
                pipe(fields);

                for (int q = 0; q < 2; ++q) {
                    unsigned int flags = fcntl(fields[i], F_GETFL, 0);
                    fcntl(fields[q], F_SETFL, flags | O_NONBLOCK);
                }

                reader_pipe[i][j] = fields[0];
                writer_pipe[i][j] = fields[1];
                //write to log file
                pipes_log_writer(pipes_log_mes_w, i, j);
                pipes_log_writer(pipes_log_mes_r, i, j);
            }
        }
    }
    this->lamp_time =0;
    fclose(pipes);
    log_open();
    proc_pidts[PARENT_ID] = getpid();
    
    //creating children processes
    for (int i=1; i<=COUNTER_OF_CHILDREN; i++){
        int this_child_pidt=fork();
        if (this_child_pidt==0) { //means child
        	this->this_id = i;
		break;
        }
        else { //means parent process
           this->this_id = PARENT_ID;
           proc_pidts[i]=this_child_pidt;
        }
    }

    COUNTER_OF_PROCESSES = COUNTER_OF_CHILDREN + 1;
    
	pipes = fopen(pipes_log, "a");
    //close descriptors, which don't used by this process, cause in other case waitpid in the end doesn't work
    for (int i=0; i<COUNTER_OF_CHILDREN+1;i++){
        for (int j=0; j<COUNTER_OF_CHILDREN+1;j++){
            if (i!=this->this_id && i!=j) {
                close(writer_pipe[i][j]);
                pipes_log_writer(pipes_log_mes_w_cl, i, j, this->this_id);
            }
            if (j!=this->this_id && i!=j) {
                    close(reader_pipe[i][j]);
                  pipes_log_writer(pipes_log_mes_r_cl, i, j, this->this_id);
            }
        }

    }

	if (this->this_id != PARENT_ID){
	    fclose(pipes);
		CHILD_PROC_START(this);
	} else {
		PARENT_PROC_START(this);
	}

    log_close();
    	return 0;
	
}

void pipes_log_writer(const char *message, ...){
    va_list list;
    va_start(list,message);
    vfprintf(pipes, message, list);
    va_end(list);
}

void transfer(void *parent_data, local_id src, local_id dst, balance_t amount) {
    Proc *this = parent_data;

    Message message;
    {
        this->lamp_time++;
        message.s_header = (MessageHeader) { 
			.s_local_time = get_lamport_time(),
			.s_magic =MESSAGE_MAGIC,
			.s_type=TRANSFER,
			.s_payload_len = sizeof(TransferOrder), 
		};
        TransferOrder order = { 
			.s_src = src, 
			.s_dst = dst, 
			.s_amount = amount, 
		};
        memcpy(&message.s_payload, &order, sizeof(TransferOrder));
        send(this, src, &message);
    }
    // to await ACK answer like a proof
    receive(this, dst, &message);
    if (message.s_header.s_type==ACK) {
        if (this->lamp_time < message.s_header.s_local_time) this->lamp_time=message.s_header.s_local_time;
        this->lamp_time++;
    }

}

timestamp_t get_lamport_time(){
    return me.lamp_time;
}

int request_cs(const Proc *self) {
    Proc *this = (Proc *) self;
    this->lamp_time++;
    Queue *locqu = &this->queue;
    locqu->element[locqu->queue_size++] = (Element){.id = this->this_id, .timestamp = get_lamport_time()};

    {
        Message request_msg = {
                .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = CS_REQUEST, .s_local_time = get_lamport_time(), .s_payload_len = 0, }, .s_payload = "",
        };
        send_multicast(this, &request_msg);
    }

    int replies_left = COUNTER_OF_PROCESSES - 2;
    int flag = 1;
    while (flag) {

        int index = 0;
        for (int i = 1; i < this->queue.queue_size; ++i) {
            if ((this->queue.element[i].timestamp<this->queue.element[index].timestamp) ||
                ((locqu->element[i].timestamp==locqu->element[index].timestamp)&&(locqu->element[i].id>this->queue.element[index].id))) index = i;
        }
        if (replies_left == 0 && this->queue.element[index].id == this->this_id) {
            break;
        }
        Message received_msg;
        local_id peer = receive_any(this, &received_msg);
        if (this->lamp_time < received_msg.s_header.s_local_time) this->lamp_time=received_msg.s_header.s_local_time;
        this->lamp_time++;
        index = 0;
        switch (received_msg.s_header.s_type) {
            case CS_RELEASE:
                for (int i = 1; i < this->queue.queue_size; ++i) {
                    if ((this->queue.element[i].timestamp<this->queue.element[index].timestamp) ||
                        ((locqu->element[i].timestamp==locqu->element[index].timestamp)&&(locqu->element[i].id>this->queue.element[index].id))) index = i;
                }
                this->queue.element[index] = this->queue.element[this->queue.queue_size - 1];
                this->queue.queue_size--;
                break;
            case DONE:
                this->received_msg++;
                break;
            case CS_REQUEST:
                locqu->element[locqu->queue_size++] = (Element){.id = peer, .timestamp = received_msg.s_header.s_local_time,};

                this->lamp_time++;
                timestamp_t local_time = get_lamport_time();
                Message message = {
                        .s_header = { .s_magic = MESSAGE_MAGIC, .s_local_time = local_time, .s_type = CS_REPLY, .s_payload_len = 0, }
                };
                send(this, peer, &message);
                break;
            case CS_REPLY:
                replies_left--;
                break;
        }
    }
    return 0;
}

int release_cs(const Proc *self) {
    Proc *this = (Proc *) self;
    Queue *localQueue = &this->queue;
    int index = 0;
    for (int i = 1; i < this->queue.queue_size; ++i) {
        if ((this->queue.element[i].timestamp<this->queue.element[index].timestamp) ||
            ((localQueue->element[i].timestamp==localQueue->element[index].timestamp)&&(localQueue->element[i].id>this->queue.element[index].id))) index = i;
    }
    localQueue->element[index] = localQueue->element[localQueue->queue_size - 1];
    localQueue->queue_size--;
    this->lamp_time++;
    Message message = {
            .s_header = { .s_magic = MESSAGE_MAGIC, .s_local_time = get_lamport_time(), .s_type = CS_RELEASE, .s_payload_len = 0, }, .s_payload = "",
    };
    send_multicast(this, &message);
    return 0;
}


