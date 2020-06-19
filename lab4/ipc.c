#include <stdbool.h>
#include <unistd.h>
#include <errno.h>

#include "child.h"

int send(void *self, local_id dest, const Message *message) {
    proc_t *this =(proc_t*)self;
    if (this->self_id == dest) return -1;

    int fd = this->io->fds[this->self_id][dest][WRITE_FD];
    write(fd, &message->s_header, sizeof(MessageHeader));
    
    if (message->s_header.s_payload_len) write(fd, message->s_payload, message->s_header.s_payload_len);
    return 0;
}

int send_multicast(void *self, const Message *message) {
    proc_t *this =(proc_t*)self;

    for (local_id i = 0; i <= this->io->procnum; i++) {
        send(self, i, message);
    }
    return 0;
}

int receive(void *self, local_id from, Message *message) {
    proc_t *this = (proc_t*)self;
    
    if (this->self_id == from) return -1;

    char *buff = (char*)message;
    ssize_t r = read(this->io->fds[from][this->self_id][READ_FD], buff, sizeof(MessageHeader));
    ssize_t r2 = 0;
    if (r > 0 && message->s_header.s_payload_len) {
        while ((r2 = read(this->io->fds[from][this->self_id][READ_FD], buff + sizeof(MessageHeader), message->s_header.s_payload_len)) < 0){}
    }
    if(r > 0){
		if(r2 >= 0){
			return 0;
		}else{
			return 1;
		}
	}else{
		return 1;
	}
    
}

int receive_any(void *self, Message *message) {
    proc_t *this = (proc_t*)self;

    for (local_id i = 0; i <= this->io->procnum; i++) {
        if (receive(self, i, message) == 0)
            return i;
    }
    return -1;   
}
