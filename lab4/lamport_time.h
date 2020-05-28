#ifndef __LAMPORT_TIME_H__
#define __LAMPORT_TIME_H__

#include "banking.h"

/** Set the <time> value of current lamport time.
 */
void set_lamport_time(timestamp_t time);

/** Get the value of current lamport time.
 */
timestamp_t get_lamport_time(void);

/** Increase current lamport time.
 */
void increase_lamport_time(void);

#endif /* __LAMPORT_TIME_H__ */
