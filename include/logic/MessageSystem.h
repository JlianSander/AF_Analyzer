#ifndef MESSAGE_SYSTEM_H
#define MESSAGE_SYSTEM_H

//Source: https://biendltb.github.io/tech/inter-process-communication-ipc-in-cpp/

#include <stdio.h>
#include <unistd.h> // for fork()

#include <sys/mman.h> // for shared memory created
#include <sys/stat.h> // for mode constants
#include <fcntl.h> // for O_* constant

#define SHARED_OBJ_NAME "/somename"

// shared data struct
struct message
{
    int pid;
    int counter;
};

bool read_message(int curr_pid, int &curr_value);

bool write_message(int pid, int value);


#endif