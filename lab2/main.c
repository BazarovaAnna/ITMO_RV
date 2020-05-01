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
#include "pa1.h"
#include "ipc.h"
#include "var_lib.h"

//added
#include <errno.h>
#include <stdbool.h>
#include "banking.h"


//local_id this_id;
//оtправлено в var_lib.h

//size_t bank_accounts
//int* BANK_ACCOUNTS;
void both_writer(const char *, ...);
void pipes_log_writer(const char *message, ...);
//added
void init_hist(Proc *this, balance_t init_bal);

static FILE *log;
static  FILE *pipes;
static const char * const pipes_log_mes_r = "Pipe from %i to %i is opened for reading\n";
static const char * const pipes_log_mes_w = "Pipe from %i to %i is opened for writing\n";
static const char * const pipes_log_mes_r_cl = "Pipe from %i to %i is closed for reading by %i\n";
static const char * const pipes_log_mes_w_cl = "Pipe from %i to %i is closed for writing by %i\n";

int main(int argc, char *argv[]) {


    //description (INIT)
    int opt=0;
    size_t COUNTER_OF_CHILDREN;//HOW MANY ACCS

	//start, check key and count of children
    while ((opt=getopt(argc, argv, "p:"))!=-1){
        switch (opt) {
			//if the key is p - OK
            case 'p':
                COUNTER_OF_CHILDREN = strtol(optarg,NULL,10);
                if (COUNTER_OF_CHILDREN < 1) {
                    fprintf(stderr, "Error: you should input more than 0 children!\n");
                    return 1;
                }
                if((argc-3)!=COUNTER_OF_CHILDREN){
					fprintf(stderr, "Error: you should input ");
					fprintf(stderr,"%li", COUNTER_OF_CHILDREN);
					fprintf(stderr, " numbers!\n");
                    return 1;
				}
				BANK_ACCOUNTS = (int*)malloc(COUNTER_OF_CHILDREN * sizeof(long));
				for(int i=3;i<COUNTER_OF_CHILDREN+3;i++){
					BANK_ACCOUNTS[i-3]=strtol(argv[i],NULL,10);
				}
				
                break;
            //if we have anything else: WRONG INPUT
            default:
                fprintf(stderr, "Error: you should use key like a '-p NUMBER_OF_CHILDREN'!\n");
                return 1;
                break;
        }
    }
    //IF SMTH GOES WRONG THIS if WORKS
    if (COUNTER_OF_CHILDREN==0){
        fprintf(stderr, "Error: you should use key like a '-p NUMBER_OF_CHILDREN CHILDREN!'\n");
        return 1;
    }
	
	//opening pipe file
    pipes = fopen(pipes_log, "w");
	Proc *this = &me;

    //creating descriptors to send and read from i to j
    for (int i=0; i<=COUNTER_OF_CHILDREN;i++){
        for (int j=0; j<=COUNTER_OF_CHILDREN;j++){
            if (i!=j) {//can't be child for itself
                int fields[2];
                pipe(fields);
                reader_pipe[i][j] = fields[0];
                writer_pipe[i][j] = fields[1];
                //write to log file
                pipes_log_writer(pipes_log_mes_w, i, j);
                pipes_log_writer(pipes_log_mes_r, i, j);
            }
        }
    }
    //don't need this anymore
    fclose(pipes);
    //opening log file
    log = fopen(events_log, "w");

    //create array with pidts and save parent's pid
    //replaced to var_lib.h
    //pid_t proc_pidts[COUNTER_OF_CHILDREN];
    proc_pidts[PARENT_ID] = getpid();
    
    //creating children processes
    for (int i=1; i<=COUNTER_OF_CHILDREN; i++){
        int this_child_pidt=fork();
        if (this_child_pidt==0) { //means child
            //this_id = i;
        	this->id = i;    
		break;
        }
        else { //means parent process
            //this_id = PARENT_ID;
            //proc_pidts[i]=this_child_pidt;
		this->id = PARENT_ID;
		proc_pidts[i]=this_child_pidt;
        }
    }

    COUNTER_OF_PROCESSES = COUNTER_OF_CHILDREN + 1;//root+children
    
	pipes = fopen(pipes_log, "a");
    //close descriptors, which don't used by this process, cause in other case waitpid in the end doesn't work
    for (int i=0; i<COUNTER_OF_CHILDREN+1;i++){
        for (int j=0; j<COUNTER_OF_CHILDREN+1;j++){
			//make sure everything is closed (CHECK OUT)
            if (i!=this_id && i!=j) {
                close(writer_pipe[i][j]);
                pipes_log_writer(pipes_log_mes_w_cl, i, j, this_id);
            }
            if (j!=this_id && i!=j) {
                    close(reader_pipe[i][j]);
                    pipes_log_writer(pipes_log_mes_r_cl, i, j, this_id);
            }
        }

    }


	if (this->id != PARENT_ID){
		CHILD_PROC_START(this, BANK_ACCOUNTS[this->id]);	
	} else {
		PARENT_PROC_START(this);
	}

	fclose(pipes);
    	fclose(log);
    
    	return 0;
	
}

//replaced to parent and child 
/*
    //to send messages to all by this children about start
    if (this_id != PARENT_ID) {
        Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED, }, };
        sprintf(message.s_payload, log_started_fmt, this_id, getpid(), getppid());
        message.s_header.s_payload_len = strlen(message.s_payload);
        //todo cycle? - no it's better
        send_multicast(NULL, &message);
        both_writer(log_started_fmt, this_id, getpid(), getppid());
    }


    //await to receive all started messages
    //todo another counter maybe? - no need
    for (size_t i=1; i<= COUNTER_OF_CHILDREN; i++){
        Message msg;
        if (i!= this_id) receive(NULL,i,&msg);
    }
    both_writer(log_received_all_started_fmt, this_id);

   //await to send all done messages
    if (this_id !=PARENT_ID) {
        Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type=DONE,}, };
        sprintf(message.s_payload, log_done_fmt, this_id);
        message.s_header.s_payload_len = strlen(message.s_payload);
        send_multicast(NULL, &message);
        both_writer(log_done_fmt,this_id);
    }

    //await to receive all done messages
    for (int i = 1; i<=COUNTER_OF_CHILDREN; i++){
        Message message;
        if (i != this_id) receive(NULL, i, &message);
    }
    //write to log & comm line
    both_writer(log_received_all_done_fmt, this_id);

    //to end this awful things, await ends of processes
    if (this_id == PARENT_ID){
        for(int i=1; i<COUNTER_OF_PROCESSES; i++){
            waitpid(proc_pidts[i], NULL, 0);
        }
    }
    //don't need this anymore
    fclose(pipes);
    fclose(log);
    
    return 0;
}*/

void both_writer(const char *message, ...){
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
	message->s_header.s_payload_len = payload_length;
	va_end(list);
}

void pipes_log_writer(const char *message, ...){
    va_list list;
    va_start(list,message);
    vfprintf(pipes, message, list);
    va_end(list);
}

void init_hist(Proc *this, balance_t init_bal){
	this->bal_hist.s_id = this->id;
	this->bal_hist.s_history_len = 1;
	for (timestamp_t timestamp = 1; time < MAX_T; time++){
		this->bal_hist.s_history[timestamp] = (BalanceState) { .s_balance = init_bal, .s_balance_pending_in = 0, .s_time = timestamp, };
	}
}