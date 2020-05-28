#include <stdbool.h>
#include "io.h"
#include "ipc.h"
#include "queue.h"
#include "child.h"
#include "lamport_time.h"
#include "pa2345.h"


/**
 * 
 * @algo
 *      REQUEST:
 *       *)  Q_i -- queue of P_i;
 *          When P_i needs access to CS, it sends REQUEST(L_i, i)
 *          to all process and adds the message to it's own queue.
 *       *) 
 *
 */
int
request_cs(const void * self) {
    proc_t *p = (proc_t*)self;
    Message msg;
    increase_lamport_time();
    msg.s_header = (MessageHeader) {
        .s_magic = MESSAGE_MAGIC,
        .s_type  = CS_REQUEST,
        .s_local_time = get_lamport_time(),
        .s_payload_len = 0
    };

    send_multicast((void*)p, &msg);

    queue_insert(p->queue, node_create(p->self_id, get_lamport_time()));
    int wait_reply = p->io->procnum-1;
    while (wait_reply != 0 || (p->queue->len && p->queue->head->id != p->self_id) ) {
        int id;
        while ((id = receive_any((void*)p, &msg)) < 0);
        set_lamport_time(msg.s_header.s_local_time);
        switch (msg.s_header.s_type) {
            case CS_REQUEST: {
                fprintf(stderr, "%d: process %d got request from %d\n", get_lamport_time(), p->self_id, id);
                queue_insert(p->queue, node_create(id, msg.s_header.s_local_time));
                increase_lamport_time();
                msg.s_header.s_type = CS_REPLY;
                msg.s_header.s_local_time = get_lamport_time();
                send((void*)p, id, &msg);
                break;
            }
            case CS_REPLY: {
                fprintf(stderr, "%d: process %d got replay from %d\n", get_lamport_time(), p->self_id, id);
                wait_reply--;
                break;
            }
            case CS_RELEASE: {
                fprintf(stderr, "%d: process %d got release from %d\n", get_lamport_time(), p->self_id, id);
                queue_delete_first(p->queue);
                break;
            }
            case DONE: {
                fprintf(stderr, "%d: process %d got DONE from %d\n", get_lamport_time(), p->self_id, id);
                p->running_processes--;
                break;
            }
            default: {
                fprintf(stderr, "%d: process %d got unknown type: %d\n", get_lamport_time(), p->self_id, msg.s_header.s_type);
            }

        }
    }
    return 0;
}

int
release_cs(const void * self) {
    proc_t *p = (proc_t*)self;
    Message msg;
    increase_lamport_time();
    msg.s_header = (MessageHeader) {
        .s_magic = MESSAGE_MAGIC,
        .s_type  = CS_RELEASE,
        .s_local_time = get_lamport_time(),
        .s_payload_len = 0
    };
    send_multicast((void*)p, &msg);
    return 0;
}
