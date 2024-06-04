#include "../../include/logic/ScepticalCheck.h"

bool ScepticalCheck::check_rejection(uint32_t argument, list<uint32_t> &initial_set, AF &framework)
{
	//printf("%d: ------- check rejection --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());												//DEBUG

	//iterate through arguments in the initial set
	for (list<uint32_t>::iterator mIter = initial_set.begin(); mIter != initial_set.end(); ++mIter) {
		//check if argument if victim of initial set
		if (framework.victims[*mIter]._bitset[argument]) {
			return true;
		}
	}

	return false;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool ScepticalCheck::check_terminate_extension_build(uint32_t argument, list<uint32_t> &initial_set)
{
	//printf("%d: ------- check termination of extension --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());								//DEBUG
	//iterate through arguments in the initial set

	for (list<uint32_t>::iterator mIter = initial_set.begin(); mIter != initial_set.end(); ++mIter) {
		//check if argument if victim of initial set
		if (*mIter == argument) {
			return true;
		}
	}

	return false;
}