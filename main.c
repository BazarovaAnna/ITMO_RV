#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "io.h"
#include "ipc.h"
#include "log.h"


int main(int argc, char const *argv[]) {
    
	size_t NUMBER_OF_CHILDREN;

    if (argc == 3 && strcmp(argv[1], "-p") == 0) {
	NUMBER_OF_CHILDREN = strtol(argv[2], NULL, 10);//TODO change strtol?
	if (NUMBER_OF_CHILDREN >= MAX_COUNT_OF_PROCESSES) {	
        fprintf(stderr, "Error: you should input less than 10 children!\n");
        return 1;
    }
	else
	{
		fprintf(stderr, "Error: you should use key like a '-p NUMBER_OF_CHILDREN!'\n");
        	return 1;
	}


    NUMBER_OF_PROCESSES = NUMBER_OF_CHILDREN + 1;

    return 0;
}


