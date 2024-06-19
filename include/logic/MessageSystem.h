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
    int exec_code;
    int num_args;
    int num_args_reduc_coi;
    int num_args_reduc_coi_gr;
    int num_args_reduc_gr;
};

bool read_message(int curr_pid, int &exec_code, int &num_args, int &num_args_reduc_coi, int &num_args_reduc_coi_gr, int &num_args_reduc_gr);

bool write_message(int pid, int exec_code, int num_args, int num_args_reduc_coi, int num_args_reduc_coi_gr, int num_args_reduc_gr);


#endif