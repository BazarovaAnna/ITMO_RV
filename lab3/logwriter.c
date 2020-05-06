#include <stdarg.h>
#include <stdio.h>
#include "logwriter.h"
#include "common.h"
#include "ipc.h"

static FILE *log;
void both_writer(const char *message, ...){
    va_list list;


    //write to event log file
    va_start(list,message);
    vfprintf(log, message, list);
    va_end(list);

    //write to console
    va_start (list, message);
    vprintf(message, list);
    va_end(list);

}
void both_writer_with_messages(Message *const message, const char *frmt, ...){
    va_list list;

    //write to event log file
    va_start(list,frmt);
    vfprintf(log, frmt, list);
    va_end(list);

    //write to console
    va_start (list, frmt);
    vprintf(frmt, list);
    va_end(list);

    va_start(list, frmt);
    size_t payload_lenght = sprintf(message -> s_payload, frmt, list);
    message->s_header.s_payload_len = payload_lenght;
    va_end(list);

}
void log_open(){
    log = fopen(events_log, "a");
}
void log_close(){
    fclose(log);
}


