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
//added
//static FILE *log; OLD
static  FILE *pipes;
static const char * const pipes_log_mes_r = "Pipe from %i to %i is opened for reading\n";
static const char * const pipes_log_mes_w = "Pipe from %i to %i is opened for writing\n";
static const char * const pipes_log_mes_r_cl = "Pipe from %i to %i is closed for reading by %i\n";
static const char * const pipes_log_mes_w_cl = "Pipe from %i to %i is closed for writing by %i\n";

int main(int argc, char *argv[]) {

    //description (INIT)
    size_t COUNTER_OF_CHILDREN;//HOW MANY ACCS
    Proc *this = &me;
	this->mutex = 0;

    int opt = 1;
    while (opt< argc){
        if (strcmp(argv[opt], "--mutex") == 0){
            this->mutex=1;
        } else if (strcmp(argv[opt], "-p") == 0){
            opt++;
            COUNTER_OF_CHILDREN = strtol(argv[opt], NULL, 10);
            // todo change order and add balances?
        }
        opt++;
    }

    //IF SMTH GOES WRONG THIS if WORKS
    if (COUNTER_OF_CHILDREN==0){
        fprintf(stderr, "Error: you should use key like a '-p NUMBER_OF_CHILDREN CHILDREN!'\n");
        return 1;
    }
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
    //don't need this anymore
    fclose(pipes);
    //opening log file
    //log = fopen(events_log, "a"); this was in lab1
    log_open();
    //create array with pidts and save parent's pid
    //replaced to var_lib.h
    //pid_t proc_pidts[COUNTER_OF_CHILDREN];
    proc_pidts[PARENT_ID] = getpid();
    
    //creating children processes
    for (int i=1; i<=COUNTER_OF_CHILDREN; i++){
        int this_child_pidt=fork();
        if (this_child_pidt==0) { //means child
            //this_id = i;
        	this->this_id = i;
		break;
        }
        else { //means parent process
           this->this_id = PARENT_ID;
           proc_pidts[i]=this_child_pidt;
        }
    }

    COUNTER_OF_PROCESSES = COUNTER_OF_CHILDREN + 1;//root+children
    
	pipes = fopen(pipes_log, "a");
    //close descriptors, which don't used by this process, cause in other case waitpid in the end doesn't work
    for (int i=0; i<COUNTER_OF_CHILDREN+1;i++){
        for (int j=0; j<COUNTER_OF_CHILDREN+1;j++){
			//make sure everything is closed (CHECK OUT)
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
	    pipes_log_writer("Child %i started \n", this->this_id);
	    fclose(pipes);
		CHILD_PROC_START(this);//BANK_ACCOUNTS[this->this_id]);
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

    // to sent TRANSFER to receiver
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
    //todo check header - no need
    receive(this, dst, &message);
    if (message.s_header.s_type==ACK) {
        if (this->lamp_time < message.s_header.s_local_time) this->lamp_time=message.s_header.s_local_time;
        this->lamp_time++;
    }

}

timestamp_t get_lamport_time(){
    return me.lamp_time;
}
/*
typedef struct {
    local_id id[128];
    timestamp_t timestamp[128];
    int queue_size;
} Queue;*/

int get_next_index(Queue *this) {
    int index = 0;
    for (int i =1; i< this->queue_size; i++){
        if (this->timestamp[i]>this->timestamp[index] || ((this->timestamp[i]==this->timestamp[index])&&(this->id[i]>this->id[index]))) index = i;
    }
    return index;
}

int request_cs(const Proc *this){
    Proc* q = ((Proc *) this);
    q->lamp_time++;
    Queue *this_queue = &q->queue;
    this_queue->timestamp[this_queue->queue_size+1] = q->lamp_time;
    this_queue->id[this_queue->queue_size+1] = q->this_id;
    //todo ?
    this_queue->queue_size++;
    Message req = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = CS_REQUEST, .s_local_time = get_lamport_time(), .s_payload_len = 0, }, .s_payload = "", };
    send_multicast(q, &req);
    int waited_msg = COUNTER_OF_PROCESSES - 2;
    int flag = 1;
    if (waited_msg == 0 && this_queue->id[get_next_index(this_queue)] == q->this_id) flag = 0;
    while (flag){
        Message mes;
        local_id some_id = receive_any(q, &mes);
        if (q->lamp_time < mes.s_header.s_local_time) q->lamp_time=mes.s_header.s_local_time;
        q->lamp_time++;
        int ind = get_next_index(this_queue);
        switch(req.s_header.s_type){
            case (CS_RELEASE):
                this_queue->id[ind] = this_queue->id[this_queue->queue_size-1];
                this_queue->timestamp[ind] = this_queue->timestamp[this_queue->queue_size-1];
                this_queue->queue_size--;
                break;
            case (CS_REPLY):
                waited_msg--;
                break;
            case (CS_REQUEST):
                this_queue->timestamp[this_queue->queue_size+1] = req.s_header.s_local_time;
                this_queue->id[this_queue->queue_size+1] = some_id;
                this_queue->queue_size++;
                q->lamp_time++;
                timestamp_t loctime = q->lamp_time;
                Message msg = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_local_time  = loctime, .s_type = CS_REPLY, .s_payload_len = 0, }, .s_payload="", };
                send(q,some_id,&msg);
                break;
            case (DONE):
                q->received_msg++;
                break;
            default:
                break;
        }


        if (waited_msg == 0 && this_queue->id[get_next_index(this_queue)] == q->this_id) flag = 0;
    }

    return 0;
}

int release_cs(const Proc *this){
    Proc* q = ((Proc *) this);
    Queue *this_queue = &q->queue;
    int ind = get_next_index(this_queue);
    this_queue->id[ind] = this_queue->id[this_queue->queue_size-1];
    this_queue->timestamp[ind] = this_queue->timestamp[this_queue->queue_size-1];
    this_queue->queue_size--;
    q->lamp_time++;
    Message msg = { .s_header= { .s_magic = MESSAGE_MAGIC, .s_local_time = get_lamport_time(), .s_type = CS_RELEASE, .s_payload_len = 0, }, .s_payload="", };
    send_multicast(q, &msg);
    return 0;
}

