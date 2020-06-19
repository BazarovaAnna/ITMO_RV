#include <stdbool.h>

#include "queue.h"
#include "child.h"

int request_cs(const void * self) {
    proc_t *process = (proc_t*)self;
    Message message;
    inc_l_t();
    
    message.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type  = CS_REQUEST, .s_local_time = get_l_t(), .s_payload_len = 0
    };

    send_multicast((void*)process, &message);

    insert_into_queue(process->queue, make_node(process->self_id, get_l_t()));
    int wait_reply = process->io->procnum-1;
    
    while (wait_reply != 0 || (process->queue->len && process->queue->start->id != process->self_id) ) {
        int id;
        while ((id = receive_any((void*)process, &message)) < 0);
        set_l_t(message.s_header.s_local_time);
        switch (message.s_header.s_type) {
            case CS_REQUEST: {
                
                insert_into_queue(process->queue, make_node(id, message.s_header.s_local_time));
                inc_l_t();
                message.s_header.s_type = CS_REPLY;
                message.s_header.s_local_time = get_l_t();
                send((void*)process, id, &message);
                break;
            }
            case CS_REPLY: {
               
                wait_reply--;
                break;
            }
            case CS_RELEASE: {
                
                del_first_of_queue(process->queue);
                break;
            }
            case DONE: {
                
                process->running_processes--;
                break;
            }
            

        }
    }
    return 0;
}

int release_cs(const void * self) {
	
    proc_t *process = (proc_t*)self;
    Message message;
    
    inc_l_t();
    
    message.s_header = (MessageHeader) { .s_magic = MESSAGE_MAGIC, .s_type  = CS_RELEASE, .s_local_time = get_l_t(), .s_payload_len = 0 };
    
    send_multicast((void*)process, &message);
    return 0;
}
