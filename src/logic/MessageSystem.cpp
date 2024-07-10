#include "../../include/logic/MessageSystem.h"

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool read_message(int curr_pid, Observation &obsv)
{
    int shmFd = shm_open(SHARED_OBJ_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shmFd, sizeof(message));
    message *msg_ptr = (message *)mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    if (msg_ptr->pid == curr_pid)
    {
        //printf("Process %d: No new msg available.\n", curr_pid);                                                                      //DEBUG
        return false;
    }
    else
    {
        //printf("Process %d: Receive %d from PID %d.\n", curr_pid, msg_ptr->counter, msg_ptr->pid);                                     //DEBUG
        obsv.Executions_Code = msg_ptr->exec_code;
        obsv.Number_Args = msg_ptr->num_args;
        obsv.Number_Args_COI = msg_ptr->num_args_reduc_coi;
        obsv.Number_Args_COI_GR = msg_ptr->num_args_reduc_coi_gr;
        obsv.Number_Args_GR = msg_ptr->num_args_reduc_gr;
        obsv.Is_Solved = msg_ptr->is_solved;
        obsv.Level = msg_ptr->solve_lvl;
        obsv.Iterations = msg_ptr->solve_iterations;
        munmap(msg_ptr, sizeof(message));
    }

    close(shmFd);

    return true;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool write_message(int pid, Observation obsv)
{
    int shmFd = shm_open(SHARED_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shmFd, sizeof(message));
    message *msg_ptr = (message *)mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    //printf("Process %d: Write %d\n", pid, value);                                                                                     //DEBUG
    msg_ptr->pid = pid;
    msg_ptr->exec_code = obsv.Executions_Code;
    msg_ptr->num_args = obsv.Number_Args;
    msg_ptr->num_args_reduc_coi = obsv.Number_Args_COI;
    msg_ptr->num_args_reduc_coi_gr = obsv.Number_Args_COI_GR;
    msg_ptr->num_args_reduc_gr = obsv.Number_Args_GR;
    msg_ptr->is_solved = obsv.Is_Solved;
    msg_ptr->solve_lvl = obsv.Level;
    msg_ptr->solve_iterations = obsv.Iterations;

    munmap(msg_ptr, sizeof(message));

    // remember to close to not hit an error of
    // opening too many files
    close(shmFd);

    return true;
}