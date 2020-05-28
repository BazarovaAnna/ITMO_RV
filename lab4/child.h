
#ifndef __CHILD__
#define __CHILD__

#include <stdio.h>
#include "ipc.h"
#include "io.h"
#include "queue.h"

typedef struct {
    const IO *io;              /**< Contrainter with I/O metadata. */
    queue_t *queue;            /**< */
    local_id self_id;          /**< ID for the process. */
    local_id running_processes; /**< Number of current processes*/ 
} proc_t;

/** Child main function.
 * 
 * @param id    ID of the child process.
 */ 
int child(IO *io, local_id id);

/** Close unused file descriptors.
 * 
 * @param IO    Contrainter with I/O metadata; contains fds array and procnum. 
 * @param id    ID of the child process.
 * 
 */ 
void close_unsed_fds(IO *io, local_id id);
#endif 
