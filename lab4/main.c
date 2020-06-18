#define _GNU_SOURCE 
#include <fcntl.h> /* Определение констант O_* */
#include <unistd.h>
#include <sys/stat.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "child.h"

/** Get attributes of the process from the command line.
 * 
 * @param argc      Argument count.
 * @param argv      Argument vector.
 * @param procnum   Total count of the child processes. 
 */
static inline int
get_options(int argc, char *argv[], IO *io) {
    int opt_index = 0;
    int c;
    static struct option opts[] = {
        { "proc",   required_argument, 0, 'p'},
        { "mutexl", no_argument,       0, 'm'},
        { NULL, 0, 0, 0}
    };

    io->mutexl = 0;
    io->procnum= 0;

    while ((c = getopt_long(argc, argv, "p:m", opts, &opt_index)) > 0) {
        switch (c) {
            case 'm': {
                io->mutexl = 1;
                break;
            }
            case 'p': {
                char *endptr = NULL;
                io->procnum = strtol(optarg, &endptr, 10);   
                if (optarg == endptr || io->procnum > MAX_PROC || io->procnum <= 0) {
                    return -1;
                }
                break;
            }
            default:
                return -1;
        }
    }
    return io->procnum != 0 ? 0 : -1;
}

/** Creates pipes for IPC.
 * 
 * @param io    I/O description.
 */
static int
create_pipes(IO *io) {
    int count = 1;
    for (int i = 0; i <= io->procnum; i++) {
        for (int j = 0; j <= io->procnum; j++) {
            if (i == j) {
                io->fds[i][j][0] = -1;
                io->fds[i][j][1] = -1;
                continue;
            }
            fprintf(io->pipes_log_stream, "Created pipe number %d.\n", count++);

            if (pipe2(io->fds[i][j], O_NONBLOCK | O_DIRECT) < 0) {
               perror("pipe");
               return -1;
            }//FIXED
        }
    }
    return 0;
}

static void
wait_msg(proc_t *p, MessageType type) {
    Message msg = { {0} };
    int total = p->io->procnum;
    while (total) {
        while(receive_any((void*)p, &msg) < 0);
        if (type == msg.s_header.s_type) {
            total--;
            set_l_t(msg.s_header.s_local_time);
        }
    }
}

static void
wait_all(IO *io) {
    proc_t p = (proc_t) {
        .io = io,
        .self_id = 0
    };

    wait_msg(&p, STARTED);
    fprintf(io->events_log_stream, log_received_all_started_fmt, get_l_t(), PARENT_ID);

    wait_msg(&p, DONE);
    fprintf(io->events_log_stream, log_received_all_done_fmt, get_l_t(), PARENT_ID);
    while(wait(NULL) > 0);
}

int
main(int argc, char *argv[]) {
#define TRY_FOPEN(expr)     \
    if ((expr) == NULL) {   \
        perror("fopen");    \
        return -1;          \
    }
    
    IO io = {0};

    if (get_options(argc, argv, &io) < 0)
        return -1;

    TRY_FOPEN(io.events_log_stream = fopen(events_log, "w+"))

    TRY_FOPEN(io.pipes_log_stream = fopen(pipes_log, "w"))


    if (create_pipes(&io) < 0)
        return -1;

    for (int i = 1; i <= io.procnum; i++) {
        pid_t pid = fork();
        if (0 > pid) {
            exit(EXIT_FAILURE);
        } else if (0 == pid) {
            /* Child. */
            int ret = child(&io, i);
            exit(ret);
        }
    }
    close_unsed_fds(&io, PARENT_ID);
    wait_all(&io);
    fclose(io.pipes_log_stream);
    fclose(io.events_log_stream);
    return 0;
}
