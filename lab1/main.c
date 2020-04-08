#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>

#include "common.h"
#include "pa1.h"
#include "ipc.h"
#include "io.h"

local_id this_id;
size_t COUNT_OF_PROCESSES;
size_t reader_from_pipe[10][10];
size_t writer_to_pipe[10][10];

void both_writer(const char *, ...);
void pipes_log_writer(const char *message, ...);

static FILE *log;
static  FILE *pipes;
static const char * const pipes_log_mes_r = "Pipe from %i to %i is opened for reading\n";
static const char * const pipes_log_mes_w = "Pipe from %i to %i is opened for writing\n";
static const char * const pipes_log_mes_r_cl = "Pipe from %i to %i is closed for reading by %i\n";
static const char * const pipes_log_mes_w_cl = "Pipe from %i to %i is closed for writing by %i\n";

int main(int argc, char *argv[]) {

    //description
    int opt=0;
    size_t COUNT_OF_CHILDREN;

	//start, check key and count of children
    while ((opt=getopt(argc, argv, "p:"))!=-1){
        switch (opt) {
            case 'p':
                COUNT_OF_CHILDREN = strtol(optarg,NULL,10);
                if (COUNT_OF_CHILDREN > 10 || COUNT_OF_CHILDREN < 1) {
                    fprintf(stderr, "Error: you should input more than 0 and less than 10 children!\n");
                    return 1;
                }
                break;
            default:
                fprintf(stderr, "Error: you should use key like a '-p NUMBER_OF_CHILDREN!'\n");
                return 1;
                break;
        }
    }
    if (COUNT_OF_CHILDREN==0){
        fprintf(stderr, "Error: you should use key like a '-p NUMBER_OF_CHILDREN!'\n");
        return 1;
    }
	
	//opening pipe file
    pipes = fopen(pipes_log, "w");

    //creating descriptors to send and read from i to j

    for (int i=0; i<COUNT_OF_CHILDREN+1;i++){
        for (int j=0; j<COUNT_OF_CHILDREN+1;j++){
            if (i!=j) {
                int fields[2];
                pipe(fields);
                reader_from_pipe[i][j] = fields[0];
                writer_to_pipe[i][j] = fields[1];
                pipes_log_writer(pipes_log_mes_w, i, j);
                pipes_log_writer(pipes_log_mes_r, i, j);
            }
        }
    }
    //opening log file
    log = fopen(events_log, "w");


    //create array with pidts and save parent's pid
    pid_t proc_pidts[COUNT_OF_CHILDREN];
    proc_pidts[PARENT_ID] = getpid();
    //creating children processes
    for (int i=1; i<=COUNT_OF_CHILDREN; i++){
        int this_child_pidt=fork();
        if (this_child_pidt==0) { //means child
            this_id = i;
            break;
        }
        else { //means parent process
            this_id = PARENT_ID;
            proc_pidts[i]=this_child_pidt;
        }
    }

    COUNT_OF_PROCESSES = COUNT_OF_CHILDREN + 1;

    //close descriptors, which don't used by this process, cause in other case waitpid in the end doesn't work
    for (int i=0; i<COUNT_OF_CHILDREN+1;i++){
        for (int j=0; j<COUNT_OF_CHILDREN+1;j++){
            if (i!=this_id && i!=j) {
                close(writer_to_pipe[i][j]);
                pipes_log_writer(pipes_log_mes_w_cl, i, j, this_id);
            }
            if (j!=this_id && i!=j) {
                    close(reader_from_pipe[i][j]);
                    pipes_log_writer(pipes_log_mes_r_cl, i, j, this_id);
            }
        }

    }


    //to send messages to all by this children about starting
    if (this_id != PARENT_ID) {
        Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED, }, };
        sprintf(message.s_payload, log_started_fmt, this_id, getpid(), getppid());
        message.s_header.s_payload_len = strlen(message.s_payload);
        //todo cycle?
        send_multicast(NULL, &message);
        both_writer(log_started_fmt, this_id, getpid(), getppid());
    }


    //await to receive all started messages
    //todo another counter maybe?
    for (size_t i=1; i<= COUNT_OF_CHILDREN; i++){
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
    for (int i = 1; i<=COUNT_OF_CHILDREN; i++){
        Message message;
        if (i != this_id) receive(NULL, i, &message);
    }
    both_writer(log_received_all_done_fmt, this_id);

    //to end this awful things, await ends of processes
    if (this_id == PARENT_ID){
        for(int i=1; i<COUNT_OF_PROCESSES; i++){
            waitpid(proc_pidts[i], NULL, 0);
        }
    }
    fclose(log);
    fclose(pipes);
    return 0;
}

void both_writer(const char *message, ...){
//write to console
    va_list list;
    va_start (list, message);
    vprintf(message, list);
    va_end(list);
//write to event log file
    va_start(list,message);
    vfprintf(log, message, list);
    va_end(list);
}

void pipes_log_writer(const char *message, ...){
    va_list list;
    va_start(list,message);
    vfprintf(pipes, message, list);
    va_end(list);
}
