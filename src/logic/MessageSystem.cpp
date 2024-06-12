#include "../../include/logic/MessageSystem.h"

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool write_message(int pid, int value)
{
    int shmFd = shm_open(SHARED_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shmFd, sizeof(message));
    message *msg_ptr = (message *)mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    printf("Process %d: Write %d\n", pid, value);
    msg_ptr->pid = pid;
    msg_ptr->counter = value;

    munmap(msg_ptr, sizeof(message));

    // remember to close to not hit an error of
    // opening too many files
    close(shmFd);

    return true;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool read_message(int curr_pid, int &curr_value)
{
    int shmFd = shm_open(SHARED_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shmFd, sizeof(message));
    message *msg_ptr = (message *)mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    if (msg_ptr->pid == curr_pid)
    {
        printf("Process %d: No new msg available.\n", curr_pid);
        return false;
    }
    else
    {
        printf("Process %d: Receive %d from PID %d.\n", curr_pid, msg_ptr->counter, msg_ptr->pid);
        curr_value = msg_ptr->counter;
        munmap(msg_ptr, sizeof(message));
    }

    close(shmFd);

    return true;
}