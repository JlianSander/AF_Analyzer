#include "../../include/logic/MessageSystem.h"

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool read_message(int curr_pid, int &exec_code, int &num_args, int &num_args_reduc_coi, int &num_args_reduc_coi_gr, int &num_args_reduc_gr, int &is_solved,
    int &solve_lvl, int &solve_iterations)
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
        exec_code = msg_ptr->exec_code;
        num_args = msg_ptr->num_args;
        num_args_reduc_coi = msg_ptr->num_args_reduc_coi;
        num_args_reduc_coi_gr = msg_ptr->num_args_reduc_coi_gr;
        num_args_reduc_gr = msg_ptr->num_args_reduc_gr;
        is_solved = msg_ptr->is_solved;
        solve_lvl = msg_ptr->solve_lvl;
        solve_iterations = msg_ptr->solve_iterations;
        munmap(msg_ptr, sizeof(message));
    }

    close(shmFd);

    return true;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool write_message(int pid, int exec_code, int num_args, int num_args_reduc_coi, int num_args_reduc_coi_gr, int num_args_reduc_gr, int is_solved,
    int solve_lvl, int solve_iterations)
{
    int shmFd = shm_open(SHARED_OBJ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(shmFd, sizeof(message));
    message *msg_ptr = (message *)mmap(NULL, sizeof(message), PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);

    //printf("Process %d: Write %d\n", pid, value);                                                                                     //DEBUG
    msg_ptr->pid = pid;
    msg_ptr->exec_code = exec_code;
    msg_ptr->num_args = num_args;
    msg_ptr->num_args_reduc_coi = num_args_reduc_coi;
    msg_ptr->num_args_reduc_coi_gr = num_args_reduc_coi_gr;
    msg_ptr->num_args_reduc_gr = num_args_reduc_gr;
    msg_ptr->is_solved = is_solved;
    msg_ptr->solve_lvl = solve_lvl;
    msg_ptr->solve_iterations = solve_iterations;

    munmap(msg_ptr, sizeof(message));

    // remember to close to not hit an error of
    // opening too many files
    close(shmFd);

    return true;
}