#include <stdbool.h>

#include "queue.h"
#include "child.h"

/*
 * Что мы хотим добиться:
 *          Q_i -- очередь из P_i;
 *          когда P_i нужен доступ к CS, он отправляет REQUEST(L_i, i)
 *          всем процессам и добавляет сообщение в его собственную очередь 
 *
 */
int request_cs(const void * self) {
    proc_t *process = (proc_t*)self;
    Message message;
    inc_l_t();
    
    message.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC,.s_type  = CS_REQUEST,.s_local_time = get_l_t(),.s_payload_len = 0 };

    send_multicast((void*)process, &message);

    insert_into_queue(process->queue, make_node(process->self_id, get_l_t()));
    int wait_reply = process->io->procnum-1;
    while (wait_reply != 0 || (process->queue->len && process->queue->start->id != process->self_id) ) {
        int id;
        while ((id = receive_any((void*)process, &message)) < 0);
        set_l_t(message.s_header.s_local_time);
        switch (message.s_header.s_type) {
            case CS_REQUEST: {
                //fprintf(stderr, "%d: process %d got request from %d\n", get_lamport_time(), process->self_id, id);
                
                insert_into_queue(process->queue, make_node(id, message.s_header.s_local_time));
                inc_l_t();
                message.s_header.s_type = CS_REPLY;
                message.s_header.s_local_time = get_l_t();
                send((void*)process, id, &message);
                break;
            }
            case CS_REPLY: {
                //fprintf(stderr, "%d: process %d got replay from %d\n", get_lamport_time(), process->self_id, id);
                wait_reply--;
                break;
            }
            case CS_RELEASE: {
                //fprintf(stderr, "%d: process %d got release from %d\n", get_lamport_time(), process->self_id, id);
                del_first_of_queue(process->queue);
                break;
            }
            case DONE: {
                //fprintf(stderr, "%d: process %d got DONE from %d\n", get_lamport_time(), process->self_id, id);
                process->running_processes--;
                break;
            }
            default: {
                //fprintf(stderr, "%d: process %d got unknown type: %d\n", get_lamport_time(), process->self_id, message.s_header.s_type);
            }

        }
    }
    return 0;
}

int release_cs(const void * self) {
	
    proc_t *process = (proc_t*)self;
    Message message;
    
    inc_l_t();
    
    message.s_header = (MessageHeader) {
        .s_magic = MESSAGE_MAGIC,
        .s_type  = CS_RELEASE,
        .s_local_time = get_l_t(),
        .s_payload_len = 0
    };
    
    send_multicast((void*)process, &message);
    return 0;
}
