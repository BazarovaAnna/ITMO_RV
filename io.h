#include "ipc.h"

enum {
    MAX_COUNT_OF_PROCESSES = 10,
};

localid selfid;
size_t count_of_processes;
size_t custom_reader[MAX_COUNT_OF_PROCESSES][MAX_COUNT_OF_PROCESSES];
size_t custom_writer[MAX_COUNT_OF_PROCESSES][MAX_COUNT_OF_PROCESSES];

