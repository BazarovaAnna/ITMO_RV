#include "ipc.h"
#include <unistd.h>
#include <stdbool.h>
#include <fcntl.h>

#include <stdio.h>

#include "var_lib.h"
#include <time.h>

int send(void *self, local_id dst, const Message *message) {
	Proc *this = self;    
	if (dst >= COUNTER_OF_PROCESSES) {
        return 1;
    }
    if (message->s_header.s_magic != MESSAGE_MAGIC) {
        return 1;
    }
    write(writer_pipe[this->this_id][dst], &message->s_header, sizeof(MessageHeader));
    write(writer_pipe[this->this_id][dst], &message->s_payload, message->s_header.s_payload_len);
    return 0;
}

int send_multicast(void *self, const Message *message) {
	Proc *this = self;    
	for (local_id lid=0; lid < COUNTER_OF_PROCESSES; lid++) {
        if (lid != this->this_id) {
            int transact = send(this,lid,message);
            if (transact > 0) return transact;
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *message) {
    Proc *this = self;
    if(from >= COUNTER_OF_PROCESSES) {
        return 1;
    }

    size_t src_file = reader_pipe[from][this->this_id];
    unsigned int flags = fcntl(src_file, F_GETFL, 0);
    fcntl(src_file, F_SETFL, flags & !O_NONBLOCK);


    read(src_file, &message->s_header, sizeof(MessageHeader));
    fcntl(src_file, F_SETFL, flags | O_NONBLOCK);
    if (message->s_header.s_magic != MESSAGE_MAGIC) {
        return 1;
    }

    unsigned int flags1 = fcntl(src_file, F_GETFL, 0);
    fcntl(src_file, F_SETFL, flags1 & !O_NONBLOCK);
    read(reader_pipe[from][this->this_id], &message->s_payload, message->s_header.s_payload_len);//убрать отдельное считывание тела?
    fcntl(src_file, F_SETFL, flags1 | O_NONBLOCK);
    return 0;
}

int receive_any(void *self, Message *message) {
    Proc *this = (Proc *) self;
    int to_whom = this->this_id;
    while (true) {
	//todo why here ++to_whom? need change if we can *I CHANGED TO -> TO_WHOM JUST NAME REPLACING
        if ( ++to_whom  == this->this_id) to_whom++;
        if (to_whom >= COUNTER_OF_PROCESSES) {
            to_whom -= COUNTER_OF_PROCESSES;
        }

        size_t src_file = reader_pipe[to_whom][this->this_id];
        unsigned int flags = fcntl(src_file, F_GETFL, 0);
        fcntl(src_file, F_SETFL, flags | O_NONBLOCK);
        int num_bytes_read = read(src_file, &message->s_header, 1); 
        switch (num_bytes_read) { 
            case -1: 
                // Would block, go to next 
                //nanosleep((const struct timespec[]) {{0, 1000L}}, NULL); 
                //sleep(1); 
                continue; 
                break; 
            case 0: { 
                // EOF reached 
                //nanosleep((const struct timespec[]) {{0, 1000L}}, NULL); 
                //sleep(1); 
                continue; 
                break; 
            } 
            default: 
                // One byte read, continue reading 
                break; 

        } 

        fcntl(src_file, F_SETFL, flags & !O_NONBLOCK); 

        read(src_file, ((char *) &message->s_header) + 1, sizeof(MessageHeader) - 1);
        read(src_file, message->s_payload, message->s_header.s_payload_len);

        fcntl(src_file, F_SETFL, flags | O_NONBLOCK);

        return 0;
    }
}

