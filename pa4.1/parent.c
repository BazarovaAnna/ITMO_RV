
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>

#include <stdarg.h>
#include "ipc.h"
#include "banking.h"
#include "var_lib.h"
#include "parent.h"
#include "pa2345.h"
#include "logwriter.h"

void PARENT_PROC_START(Proc *this){
    ugar(this);
    ugar(this);
	for(int i=1; i<=COUNTER_OF_PROCESSES; i++){
            waitpid(proc_pidts[i], NULL, 0);
	}
}

void ugar(Proc *this){
    for (size_t i=1; i<= COUNTER_OF_PROCESSES - 1; i++){
        Message msg;
        if (i!= this->this_id)
        {
            receive(&me,i,&msg);
            if (this->lamp_time < msg.s_header.s_local_time) this->lamp_time=msg.s_header.s_local_time;
            this->lamp_time++;
        }
    }
}

