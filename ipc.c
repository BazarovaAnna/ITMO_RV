#include "ipc.h"
#include <unistd.h>
#include "stdio.h"

local_id this_id;
size_t COUNT_OF_PROCESSES;
size_t reader_from_pipe[10][10];
size_t writer_to_pipe[10][10];

int send(void *self, local_id dst, const Message *msg) {
    if (dst >= COUNT_OF_PROCESSES) {
        return 1;
    }
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return 1;
    }
    write(writer_to_pipe[this_id][dst], &msg->s_header, sizeof(MessageHeader));
    write(writer_to_pipe[this_id][dst], &msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}

int send_multicast(void *self, const Message *msg) {
    for (local_id lid=0; lid < COUNT_OF_PROCESSES; lid++) {
        if (lid != this_id) {
            int transact = send(self,lid,msg);
            if (transact > 0) return transact;
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    if(from >= COUNT_OF_PROCESSES) {
        return 1;
    }
    read(reader_from_pipe[from][this_id], &msg->s_header, sizeof(MessageHeader));
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return 1;
    }
    read(reader_from_pipe[from][this_id], &msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}

int receive_any(void *self, Message *msg) {
    return 0;
}

