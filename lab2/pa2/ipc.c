#include "stdio.h"
#include "ipc.h"
#include <unistd.h>
#include <stdbool.h>
#include "var_lib.h"

/*local_id this_id;
size_t COUNTER_OF_PROCESSES;
size_t reader_pipe[10][10];
size_t writer_pipe[10][10];*/

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
            int transact = send(self,lid,message);
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
    read(reader_pipe[from][this->this_id], &message/*->s_header*/, sizeof(MessageHeader)+message->s_header.s_payload_len);
    sleep(1);
    if (message->s_header.s_magic != MESSAGE_MAGIC) {
        return 1;
    }
    //read(reader_pipe[from][this->id], &message->s_payload, message->s_header.s_payload_len);//убрать отдельное считывание тела
    //sleep(1);
    return 0;
}

int receive_any(void *self, Message *message) {
    Proc *this = (Proc *) self;
    int to = this->this_id;
    while (true) {
        //todo refactor
        if ( to + 1 == this->this_id) to++;
        if (to >= COUNTER_OF_PROCESSES) {
            to -= COUNTER_OF_PROCESSES;
        }
        read(reader_pipe[to][this->this_id], &message->s_header, 1);
        read(reader_pipe[to][this->this_id], ((char *) &message->s_header) + 1, sizeof(MessageHeader) - 1);
        read(reader_pipe[to][this->this_id], message->s_payload, message->s_header.s_payload_len);
        return 0;
    }
}

