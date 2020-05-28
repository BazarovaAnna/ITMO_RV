#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include "ipc.h"
#include "io.h"
#include "child.h"

int send(void *self, local_id dest, const Message *message) {
    proc_t *p =(proc_t*)self;
    local_id sorc = p->self_id;

    if (sorc == dest) return -1;

    int fd = p->io->fds[sorc][dest][WRITE_FD];//TODO AFTER IO
    write(fd, &message->s_header, sizeof(MessageHeader));//TODO AFTER IO
    
    if (message->s_header.s_payload_len) write(fd, message->s_payload, message->s_header.s_payload_len);
    return 0;
}

int send_multicast(void *self, const Message *message) {
    proc_t *p =(proc_t*)self;
    local_id pnum = p->io->procnum;//TODO AFTER IO

    for (local_id i = 0; i <= pnum; i++) {//TODO AFTER IO
        send(self, i, message);
    }
    return 0;
}

int receive(void *self, local_id from, Message *message) {
    proc_t *p = (proc_t*)self;//TODO CHECK SELF
    local_id to = p->self_id;
    
    if (to == from) return -1;
    
    int fd = p->io->fds[from][to][READ_FD];//TODO AFTER IO

    char *buff = (char*)message;
    ssize_t r = read(fd, buff, sizeof(MessageHeader));//TODO AFTER IO
    ssize_t r2 = 0;
    if (r > 0 && message->s_header.s_payload_len) {
        while ((r2 = read(fd, buff + sizeof(MessageHeader), message->s_header.s_payload_len)) < 0){}
    }
    return r > 0 ? (r2 >= 0 ? 0 : -1) : - 1;//TODO REMAKE
}

//Receive message from the process.
//return  ID of sender process or -1.
int receive_any(void *self, Message *message) {
    proc_t *p = (proc_t*)self;
    local_id pnum = p->io->procnum;//TODO AFTER IO

    for (local_id i = 0; i <= pnum; i++) {//TODO AFTER IO
        if (receive(self, i, message) == 0)
            return i;
    }
    return -1;   
}
