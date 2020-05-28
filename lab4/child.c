#define _DEFAULT_SOURCE
#include <alloca.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include "lamport_time.h"
#include "common.h"
#include "ipc.h"
#include "io.h"
#include "pa2345.h"
#include "child.h"


/** Close unused file descriptors.
 * 
 * @param fds   Array of pipes' descriptors.
 * @param id    ID of the child process.
 * @param num   Number of processes.
 * 
 */ 
void
close_unsed_fds(IO *io, local_id id) {

    for (local_id i = 0; i <= io->procnum; i++) {
        for (local_id j = 0; j <= io->procnum; j++) {
            if (i != j) {
                if (i == id) {
                    close(io->fds[i][j][READ_FD]);
                    fprintf(io->pipes_log_stream, "ID %d closes read(%hhd -- %hhd)\n", id, i,j);
                }

                if (j == id) {
                    close(io->fds[i][j][WRITE_FD]);
                    fprintf(io->pipes_log_stream, "ID %d closes write(%hhd -- %hhd)\n", id, i,j);
                }

                if (i != id && j != id) {
                    fprintf(io->pipes_log_stream, "ID %d closes pipe(%hhd -- %hhd)\n", id, i,j);
                    close(io->fds[i][j][WRITE_FD]);
                    close(io->fds[i][j][READ_FD]);
                }
            }
        }
    }

    fprintf(io->pipes_log_stream, "ID %d closes all fds.\n", id);
}


/** Syncronization cycle.
 */
static void
sync_state(proc_t *p, MessageType type, char *payload, size_t payload_len) {
    Message msg;
    increase_lamport_time();
    msg.s_header = (MessageHeader) {
            .s_magic       = MESSAGE_MAGIC,
            .s_payload_len = payload_len,
            .s_type        = type,
            .s_local_time  = get_lamport_time()
    };
    strncpy(msg.s_payload, payload, payload_len);

    send_multicast((void*)p, (const Message *)&msg);
    int total = p->running_processes-1;
    while (total) {
        while(receive_any((void*)p, &msg) < 0);
        set_lamport_time(msg.s_header.s_local_time);
        if (type == msg.s_header.s_type) {
            total--;
        }
    }
}

void
work(proc_t *proc) {
    int loops = proc->self_id*5;

    if (proc->io->mutexl)
        request_cs((void*)proc);
    
    for (int i = 1; i <= loops; i++) {
        size_t slen = snprintf(NULL, 0, log_loop_operation_fmt, proc->self_id, i,
                               loops);
        char *str = (char*)alloca(slen+1);
        snprintf(str, slen+1, log_loop_operation_fmt, proc->self_id, i, loops);
        print(str);
    }

    if (proc->io->mutexl)
        release_cs((void*)proc);
}

/** Child main function.
 * 
 * @param io    I/O metadata.
 * @param id    ID of the child process.
 */ 
int
child(IO *io, local_id id) {

    proc_t p;
    char payload[MAX_PAYLOAD_LEN];
    size_t len;
    p = (proc_t){
        .io      = io,
        .queue   = queue_create(),
        .self_id = id,
        .running_processes = io->procnum
    };

    close_unsed_fds(io, id);
    /* Process starts. */
    len = sprintf(payload, log_started_fmt, get_lamport_time(), id, getpid(), getppid(), 0);
    fputs(payload, io->events_log_stream); 
    /* Proces sync with others. */
    sync_state(&p, STARTED, payload, len);
    fprintf(io->events_log_stream, log_received_all_started_fmt, get_lamport_time(), id);
    /* Work. */
    work(&p);
    
    /* Process's done. */
    len = sprintf(payload, log_done_fmt, get_lamport_time(), id, 0);
    fputs(payload, io->events_log_stream); 
    /* Process syncs wih ohers. */
    sync_state(&p, DONE, payload, len);
    fprintf(io->events_log_stream, log_received_all_done_fmt,get_lamport_time(), id);
    return 0;
}
