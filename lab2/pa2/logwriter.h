//
// Created by stepan on 03.05.20.
//

#include "ipc.h"

#ifndef LAB2_LOGWRITER_H
#define LAB2_LOGWRITER_H

void both_writer(const char *message, ...);
void both_writer_with_messages(Message *const message, const char *frmt, ...);
void log_open();
void log_close();
#endif //LAB2_LOGWRITER_H
