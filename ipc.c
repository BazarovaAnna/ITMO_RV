#include "ipc.h"
#include <unistd.h>
#include "io.h"
#include "stdio.h"

static size_t read_exact(size_t fd, void *buf, size_t nbytes);

typedef enum {
    INVALID_PEER = 1,
    INVALID_MAGIC,
} IPC_Error;

int send(void *self, local_id dst, const Message *msg) {
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
