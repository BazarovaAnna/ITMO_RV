#define _DEFAULT_SOURCE
#include <errno.h>
#include <unistd.h>
#include <stdbool.h>

#include "io.h"
#include "ipc.h"
#include "child.h"

int
send(void *self, local_id dst, const Message *msg) {
    proc_t *p   = (proc_t*)self;
    local_id src  = p->self_id;

    if (src == dst)
        return -1;

    int    fd   = p->io->fds[src][dst][WRITE_FD];
    write(fd, &msg->s_header, sizeof(MessageHeader));
    if (msg->s_header.s_payload_len)
        write(fd, msg->s_payload, msg->s_header.s_payload_len);
    return 0;
}

int
send_multicast(void *self, const Message *msg) {
    proc_t   *p   = (proc_t*)self;
    local_id   pnum = p->io->procnum;

    for (local_id i = 0; i <= pnum; i++) {
        send(self, i, msg);
    }
    return 0;
}

int
receive(void *self, local_id from, Message *msg) {
    proc_t *p   = (proc_t*)self;
    local_id to = p->self_id;
    if (to == from)
        return -1;
    int    fd   = p->io->fds[from][to][READ_FD];

    char *buf = (char*)msg;
    ssize_t r = read(fd, buf, sizeof(MessageHeader));
    ssize_t r2 = 0;
    if (r > 0 && msg->s_header.s_payload_len) {
        while ((r2 = read(fd, buf + sizeof(MessageHeader), msg->s_header.s_payload_len)) < 0){}
    }
    return r > 0 ? (r2 >= 0 ? 0 : -1) : - 1;
}

/** Receive message from the process.
 * 
 * @return  ID of sender process or -1.
 */
int
receive_any(void *self, Message *msg) {
    proc_t *p = (proc_t*)self;
    local_id pnum = p->io->procnum;

    for (local_id i = 0; i <= pnum; i++) {
        if (receive(self, i, msg) == 0)
            return i;
    }
    return -1;   
}
