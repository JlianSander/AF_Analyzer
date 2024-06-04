#include "../../include/logic/Solver_DS_PR.h"
using namespace std;

static list<uint32_t> ExtendExtension(list<uint32_t> &extension_build, list<uint32_t> &initial_set)
{
	list<uint32_t> tmpCopy_1, tmpCopy_2;
	std::copy(extension_build.begin(), extension_build.end(), std::back_inserter(tmpCopy_1));
	std::copy(initial_set.begin(), initial_set.end(), std::back_inserter(tmpCopy_2));
	tmpCopy_1.merge(tmpCopy_2);
	/*tmpCopy.insert(tmpCopy.end(), extension_build.begin(), extension_build.end());
	tmpCopy.insert(tmpCopy.end(), initial_set.begin(), initial_set.end());*/
	return tmpCopy_1;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void check_rejection_parallel_recursiv(uint32_t argument, AF &framework, VectorBitSet &activeArgs, bool *isRejected,
	list<uint32_t> &extension_build, list<uint32_t> &output_extension)	//, int *num_tasks, int *num_tasks_max
{
	int id = omp_get_thread_num();
	bool isRejected_tmp = false;

#pragma omp flush(isRejected)
#pragma omp atomic read
	isRejected_tmp = *isRejected;
	if (isRejected_tmp == true)
	{
		//printf("%d: preliminary terminated.\n", omp_get_thread_num());																		//DEBUG
		//printf("%d: finished task - argument %d ----------------------------------\n", omp_get_thread_num(), argument);						//DEBUG
		return;
	}

	//cout << id << ": starting task - argument " << argument << " ----------------------------------" << endl;									//DEBUG
	//cout << id << ": Extension so far: ";																										//DEBUG
	//Printer::print_list(extension_build);																										//DEBUG
	//cout << endl;																																//DEBUG
	//cout << id << ": current active arguments: ";																								//DEBUG
	//Printer::print_vector(activeArgs);																										//DEBUG
	//cout << endl;																																//DEBUG
	
	//long mem_base = get_mem_usage();																											//DEBUG
	VectorBitSet reduct = extension_build.empty() ? activeArgs.copy() : Reduct::get_reduct_set(activeArgs, framework, extension_build);
	//cout << id << ": reduct created: ";																										//DEBUG
	//Printer::print_set(reduct);																												//DEBUG
	//cout << endl;																																//DEBUG
	//cout << " memory_usage: " << get_mem_usage() - mem_base << "/" << get_mem_usage() << "[kB]" << endl;																					//DEBUG

	if (reduct._vector.size() < 2)
	{
		//there is only 1 active argument, this has to be the argument to check, if not then there should have been a rejection check earlier who did not work
		//printf("%d: only 1 argument left -> skip reduct\n", omp_get_thread_num());															//DEBUG

		return;
	}

	//flag used to signal that clauses need to be extended
	bool *isSolved = NULL;
	isSolved = (bool *)malloc(sizeof * isSolved);
	*isSolved = false;
	//flag used to signal that reduct has only empty set as admissible set
	bool *isFirstCalculation = NULL;
	isFirstCalculation = (bool *)malloc(sizeof * isFirstCalculation);
	*isFirstCalculation = true;
	
	uint64_t numVars = reduct._vector.size();

	//mem_base = get_mem_usage();																												//DEBUG
	SatSolver *solver = NULL;
	solver = new SatSolver_cadical(numVars);
	//printf("%d: ------- new SAT solver created --- memory usage: %ld/%ld\n", id, get_mem_usage() - mem_base, get_mem_usage());				//DEBUG
	
	//mem_base = get_mem_usage();																												//DEBUG
	//printf("%d Encode: \n", id);																												//DEBUG
	Encoding::add_clauses_nonempty_admissible_set(*solver, framework, reduct);
	//cout << id << ": SAT clauses added" << endl;																								//DEBUG
	//printf("%d: ------- SAT clauses added --- memory usage: %ld/%ld\n", id, get_mem_usage() - mem_base, get_mem_usage());						//DEBUG

#pragma omp flush(isRejected)
#pragma omp atomic read
	isRejected_tmp = *isRejected;
	if (isRejected_tmp == true)
	{
		free(isSolved);
		free(isFirstCalculation);
		delete solver;
		return;
	}

	bool has_Solution = true;

	//iterate through initial sets
	do {
		if (*isSolved)
		{
			//mem_base = get_mem_usage();																										//DEBUG
			Encoding::add_complement_clause(*solver, reduct);
			//printf("%d: added complement clause --- memory usage: %ld/%ld\n", id, get_mem_usage() - mem_base, get_mem_usage());				//DEBUG
		}

		*isSolved = true;

		//mem_base = get_mem_usage();																											//DEBUG
		has_Solution = (*solver).solve();
		//cout << id << ": SAT problem solved" << endl;																							//DEBUG
		//printf("%d: ------- SAT problem solved --- memory usage: %ld/%ld\n", id, get_mem_usage() - mem_base, get_mem_usage());				//DEBUG

		if (!has_Solution)
		{
			//no more initial sets to calculate after this one
			//printf("%d: no set \n", id);																										//DEBUG

			if (*isFirstCalculation)
			{
				//since this is the first calculation and no IS was found, this reduct has only the empty set as an admissible set, therefore the extension_build is complete
				// and since only extensions not containing the query argument proceed in the calculation, extension_build cannot contain the query argument, so that there exists
				// an extension not containing the query argument, so that the argument gets sceptical rejected
				//printf("%d: found only empty set in reduct\n", omp_get_thread_num());															//DEBUG
				
#pragma atomic write
				*isRejected = true;
#pragma omp flush(isRejected)		//maybe flush is not needed since isRejected point so a memory address, which content is changed

				//printf("%d: preliminary terminated.\n", omp_get_thread_num());																//DEBUG
				free(isSolved);
				free(isFirstCalculation);
				delete solver;
				//printf("%d: finished task - argument %d ----------------------------------\n", omp_get_thread_num(), argument);				//DEBUG
				return;
			}

			break;
		}

		//mem_base = get_mem_usage();																											//DEBUG
		list<uint32_t> initial_set = Decoding::get_set_from_solver(*solver, reduct);
		//printf("%d: ------- initial set allocated --- memory usage: %ld/%ld\n", id, get_mem_usage() - mem_base, get_mem_usage());				//DEBUG
		//cout << id << ": computed initial set: ";																								//DEBUG
		//Printer::print_list(initial_set);																										//DEBUG
		//cout << endl;																															//DEBUG
		
		if (initial_set.empty())
		{
			if (*isFirstCalculation)
			{
				//since this is the first calculation and only the empty set as IS was found, the extension_build is complete
				// and since only extensions not containing the query argument proceed in the calculation, extension_build cannot contain the query argument, so that there exists
				// an extension not containing the query argument, so that the argument gets sceptical rejected
				//printf("%d: found only empty set in reduct\n", omp_get_thread_num());															//DEBUG
				
#pragma atomic write
				*isRejected = true;
#pragma omp flush(isRejected)		//maybe flush is not needed since isRejected point so a memory address, which content is changed

				//printf("%d: preliminary terminated.\n", omp_get_thread_num());																//DEBUG
				free(isSolved);
				free(isFirstCalculation);
				delete solver;
				initial_set.clear();
				//printf("%d: finished task - argument %d ----------------------------------\n", omp_get_thread_num(), argument);				//DEBUG
				return;
			}
			else
			{
				printf("ERROR impossible that empty IS calculated and is not first calculation");
				exit(1);
			}
		}

		*isFirstCalculation = false;

		if (ScepticalCheck::check_rejection(argument, initial_set, framework))
		{
#pragma atomic write
			*isRejected = true;			
#pragma omp flush(isRejected)		//maybe flush is not needed since isRejected point so a memory address, which content is changed
			//cout << id << "initial set rejects " << argument << endl;																			//DEBUG
			//printf("%d: finished task - argument %d ----------------------------------\n", omp_get_thread_num(), argument);					//DEBUG

			//mem_base = get_mem_usage();																										//DEBUG
			list<uint32_t> new_extension_build = ExtendExtension(extension_build, initial_set);
			//cout << id << ": new extension created: ";																						//DEBUG
			//Printer::print_list(extension_build);																								//DEBUG
			//cout << " memory usage : " << get_mem_usage() - mem_base << "/" << get_mem_usage() << "[kB]" << endl;								//DEBUG	
			
#pragma atomic write
			output_extension = new_extension_build;

			free(isSolved);
			free(isFirstCalculation);
			delete solver;
			initial_set.clear();
			return;
		}
		else if (ScepticalCheck::check_terminate_extension_build(argument, initial_set))
		{
			//cout << id << ": path of initial set: ";																							//DEBUG
			//Printer::print_list(initial_set);																									//DEBUG
			//cout << " aborted" << endl;																										//DEBUG

			initial_set.clear();
			continue;
		}

		//mem_base = get_mem_usage();																											//DEBUG
		list<uint32_t> new_extension_build = ExtendExtension(extension_build, initial_set);
		//cout << id << ": new extension created: ";																							//DEBUG
		//Printer::print_list(extension_build);																									//DEBUG
		//cout << " memory usage : " << get_mem_usage() - mem_base << "/" << get_mem_usage() << "[kB]" << endl;									//DEBUG
		
		initial_set.clear();

		uint32_t prio = Prioritizer::calculate_priority(framework, new_extension_build);
		cout << "Priority calculated for:";																										//DEBUG
		Printer::print_list(new_extension_build);																								//DEBUG
		cout << " Calculated Priority: " << prio << endl;																						//DEBUG

//#pragma atomic write
//		*num_tasks = *num_tasks + 1;
//#pragma omp flush(num_tasks)
//
//		int tmp_num_tasks, tmp_num_tasks_max;
//#pragma omp atomic read
//			tmp_num_tasks = *num_tasks;
//#pragma omp flush(num_tasks_max)
//#pragma omp atomic read
//			tmp_num_tasks_max = *num_tasks_max;
//
//		if (tmp_num_tasks > tmp_num_tasks_max)
//		{
//#pragma atomic write
//			*num_tasks_max = tmp_num_tasks;
//
//			printf("%d: ------- New max Number of currently open tasks:  %d\n", id, tmp_num_tasks_max);
//		}
//
//		printf("%d: ------- Number of currently open tasks:  %d\n", id, tmp_num_tasks);

		//mem_base = get_mem_usage();																													//DEBUG
#pragma omp task \
	firstprivate(new_extension_build) \
	private(reduct, initial_set, solver) \
	shared(argument, framework, activeArgs, isRejected, output_extension) \
	priority(prio)
		{
			//printf("%d: ------- task started --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());									//DEBUG
			check_rejection_parallel_recursiv(argument, framework, activeArgs, isRejected, new_extension_build, output_extension); //, num_tasks, num_tasks_max
			new_extension_build.clear();
			int tmp_num_tasks = 0;																												
//#pragma atomic write
//			*num_tasks = *num_tasks - 1;
//#pragma omp flush(num_tasks)
//#pragma omp atomic read
//			tmp_num_tasks = *num_tasks;

			//printf("%d: ------- Number of currently open tasks:  %d\n", id, tmp_num_tasks);
		}
		//cout << id << ": task created" << " memory usage: " << get_mem_usage() - mem_base << "/" << get_mem_usage() << "[kB]" << endl;				//DEBUG
		
		new_extension_build.clear();

		//mem_base = get_mem_usage();																													//DEBUG
		reduct = extension_build.empty() ? activeArgs.copy() : Reduct::get_reduct_set(activeArgs, framework, extension_build);
		//cout << id << ": reduct created: ";																											//DEBUG
		//Printer::print_set(reduct);																												//DEBUG
		//cout << endl;																																//DEBUG
		//cout << " memory_usage: " << get_mem_usage() - mem_base << "/" << get_mem_usage() << "[kB]" << endl;										//DEBUG

#pragma omp flush(isRejected)
#pragma omp atomic read
		isRejected_tmp = *isRejected;
	} while (has_Solution && !isRejected_tmp);

	//if (has_Solution && isRejected_tmp)																											//DEBUG
	//{																																				//DEBUG
	//	printf("%d: preliminary terminated.\n", id);																								//DEBUG
	//}																																				//DEBUG

	free(isSolved);
	free(isFirstCalculation);
	delete solver;

	//printf("%d: finished task - argument %d ----------------------------------\n", omp_get_thread_num(), argument);								//DEBUG
	return;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static bool check_rejection_parallel(uint32_t argument, AF &framework, VectorBitSet &active_args, list<uint32_t> &proof_extension, uint8_t numCores)
{
	//float start_time = omp_get_wtime();																											//DEBUG
	//long mem_base = get_mem_usage();																												//DEBUG
	bool *isRejected = NULL;
	isRejected = (bool *)malloc(sizeof *isRejected);
	if (isRejected == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}

	//printf("%d: ------- isRejected allocated --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());									//DEBUG
	*isRejected = false;

	if (numCores > 0)
	{
		omp_set_num_threads(numCores);
	}

	/*int *num_tasks = NULL;
	num_tasks = (int *)malloc(sizeof * num_tasks);
	if (num_tasks == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}
	*num_tasks = 0;

	int *num_tasks_max = NULL;
	num_tasks_max = (int *)malloc(sizeof * num_tasks_max);
	if (num_tasks_max == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}
	*num_tasks_max = 0;*/

#pragma omp parallel shared(argument, framework, active_args, isRejected, proof_extension)   //, num_tasks, num_tasks_max
#pragma omp single
	{
		//printf("Number of threads: %d\n", omp_get_num_threads());																					//DEBUG

		//printf("%d: ------- started omp parallel -- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());									//DEBUG

		list<uint32_t> extension_build;

		check_rejection_parallel_recursiv(argument, framework, active_args, isRejected, extension_build, proof_extension);					// , num_tasks, num_tasks_max
	}
	
	bool result = *isRejected;
	free(isRejected);

	//printf("Finished program - voluntary context switches: %ld - involuntary context switches: %ld - memory usage: %ld/%ld [kB]\n",
	//	get_ctxt_switches_volun(), get_ctxt_switches_involun(), get_mem_usage() - mem_base, get_mem_usage());										//DEBUG
	//float end_time = omp_get_wtime();																												//DEBUG
	//float duration = end_time - start_time;																										//DEBUG
	//printf("runtime check_rejection_parallel [s]: %.2f s\n", duration);																			//DEBUG

	return result;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool Solver_DS_PR::solve(uint32_t argument, AF &framework, list<uint32_t> &proof_extension, uint8_t numCores) {
	
	VectorBitSet initial_reduct = VectorBitSet();

	pre_proc_result result_preProcessor = PreProc_DS_PR::process(framework, argument, initial_reduct);

	//cout << "Reduct after preprocessing ";																										//DEBUG
	//Printer::print_set(initial_reduct);																											//DEBUG
	//cout << endl;																																	//DEBUG

	switch (result_preProcessor){

		case accepted:
			return true;

		case rejected:
			return false;

		case unknown:
			return !check_rejection_parallel(argument, framework, initial_reduct, proof_extension, numCores);

		default:
			return unknown;
	}
}