#include "ipc.h"
#include <unistd.h>
#include "stdio.h"

//what is it? static size_t read_exact(size_t fd, void *buf, size_t nbytes);

local_id this_id;
size_t custom_reader[10][10];
size_t custom_writer[10][10];

int send(void *self, local_id dst, const Message *msg) {
    if (dst >= 10) {
        return 1;
    }
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return 1;
    }
    write(custom_writer[this_id][this_id], &msg->s_header, sizeof(MessageHeader));
    write(custom_writer[this_id][this_id], &msg->s_payload, msg->s_header.s_payload_len);
    sleep(100);
    return 0;
}

int send_multicast(void *self, const Message *msg) {
    return 0;
}

int receive(void *self, local_id from, Message *msg) {
    return 0;
}

int receive_any(void *self, Message *msg) {
    return 0;
}
