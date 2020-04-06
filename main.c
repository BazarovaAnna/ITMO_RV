#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <stdio.h>
#include <stdarg.h>
#include "common.h"
#include "pa1.h"

#include "ipc.h"


void both_writer(const char *, ...);
static FILE *log;

int main(int argc, char const *argv[]) {

    //description
    //todo renaming!

    local_id this_id;
    long int COUNT_OF_PROCESSES; //TODO int?
    long int COUNT_OF_CHILDREN;
    int custom_reader[10][10];
    int custom_writer[10][10];


	//start, check key and count of children
    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
	COUNT_OF_CHILDREN = strtol(argv[2], NULL, 10);//TODO getopt
	if (COUNT_OF_CHILDREN > 10) {
        fprintf(stderr, "Error: you should input less than 10 children!\n");
        return 1;
    }
    }
	else
	{
		fprintf(stderr, "Error: you should use key like a '-p NUMBER_OF_CHILDREN!'\n");
        	return 1;
	}

    COUNT_OF_PROCESSES = COUNT_OF_CHILDREN + 1;//TODO, do we need it here?

    //creating descriptors to send from i to j
    for (int i=0; i<COUNT_OF_PROCESSES;i++){
        for (int j=0; j<COUNT_OF_PROCESSES;j++){
            if (i!=j) {
                int fields[2];
                pipe(fields); //TODO ?
                custom_reader[i][j] = fields[0];
                custom_writer[i][j] = fields[1];
            }
        }
    }

    //opening log files
    log = fopen(events_log, "w");

    //todo why pid_t? Changed to int
    //todo What's the fuck happens for next 20 strings?
    pid_t processes_pids[COUNT_OF_CHILDREN];
    processes_pids[PARENT_ID] = getpid();
    //creating children processes
    for (size_t i=1; i<=COUNT_OF_CHILDREN; i++){

        int child_pid=fork();
        //todo can error be here in fork?
        if (child_pid==0) { //means child
            this_id = i;
            //todo why break? don't work without
            break;
        }
        else{
            //means parent process
            this_id = PARENT_ID;
            processes_pids[i]=child_pid;
        }

    }

    //todo replace here creating descriptors without deleting, or maybe delete it at all?
    for (int i=0; i< COUNT_OF_PROCESSES; i++){
        for (int j=0; j<COUNT_OF_PROCESSES; j++){
            //todo changed here
            if(!(i==this_id && j==this_id)){
                if (i != this_id && j != this_id){
                    close(custom_reader[i][j]);
                    close(custom_writer[i][j]);
                }
                if (i == this_id && j != this_id) {
                    close(custom_reader[i][j]);
                }
                if (i!= this_id && j== this_id){
                    close(custom_writer[i][j]);
                }
            }
        }
    }

    //to send messages to all by this children
    if (this_id != PARENT_ID) {
        Message message = { .s_header = { .s_magic = MESSAGE_MAGIC, .s_type = STARTED}, };
        sprintf(message.s_payload, log_started_fmt, this_id, getpid(), getppid());
        message.s_header.s_payload_len = strlen(message.s_payload);
        send_multicast(NULL, &message);
        //todo delete it
        int pid = getpid();
        int parent_pid = getppid();
        both_writer(log_started_fmt, this_id, pid, parent_pid);
    }




    //here should be middle part of the work





    //to end this awful things
    if (this_id == PARENT_ID){
        for(int i=1; i<COUNT_OF_PROCESSES; i++){
            waitpid(processes_pids[i], NULL, 0);
        }
    }
    fclose(log);
    return 0;
}

void both_writer(const char *message, ...){
    va_list va;
    va_start (va, message);
    vprintf(message, va);
    va_end(va);

    va_start(va,message);
    vfprintf(log, message, va);
    va_end(va);
}

