#include "banking.h"
#include "ipc.h"
#include "process.h"


typedef struct{
	BalanceHistory bal_hist;
	local_id this_id;
	AllHistory all_hist;
} Proc;
//added
Proc me;
pid_t proc_pidts[10];
//balance_t initial_balances[10];
int* BANK_ACCOUNTS;

//was
size_t reader_pipe[10][10];
size_t writer_pipe[10][10];
size_t COUNTER_OF_PROCESSES;
//local_id this_id
