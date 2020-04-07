#include "ipc.h"
#include <unistd.h>
#include "io.h"
#include "stdio.h"

/*
local_id this_id;
size_t COUNT_OF_PROCESSES;
size_t custom_reader[10][10];
size_t custom_writer[10][10];*/


//static size_t read_exact(size_t fd, void *buf, size_t nbytes);

typedef enum {
    INVALID_PEER = 1,
    INVALID_MAGIC,
} IpcError;

int send(void *self, local_id dst, const Message *msg) {
    if (dst >= COUNT_OF_PROCESSES) {
        return INVALID_PEER;
    }
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return INVALID_MAGIC;
    }
    write(custom_writer[this_id][dst], &msg->s_header, sizeof(MessageHeader));
    write(custom_writer[this_id][dst], &msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}

int send_multicast(void *self, const Message *msg) {
    for (local_id dst=0; dst < COUNT_OF_PROCESSES; dst++) {
        if (dst != this_id) {
            int transact = send(self,dst,msg);
            if (transact > 0) return transact;
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    if(from >= COUNT_OF_PROCESSES) {
        return INVALID_PEER;
    }
    read(custom_reader[from][this_id], &msg->s_header, sizeof(MessageHeader));
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return INVALID_MAGIC;
    }
    read(custom_reader[from][this_id], &msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}

int receive_any(void *self, Message *msg) {
    return 0;
}

/*
static size_t read_exact(size_t fd, void *buf, size_t num_bytes) {
    size_t offset = 0;
    size_t remaining = num_bytes;

    while (remaining > 0) {
        int num_bytes_read = read(fd, ((char *)buf) + offset, remaining);
        if (num_bytes_read > 0) {
            remaining -= num_bytes_read;
            offset += num_bytes_read;
        } else {
            return -1;
        }
    }

    return offset;
}*/
