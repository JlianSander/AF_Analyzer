#include "../../include/logic/Solver_DS_PR.h"
using namespace std;

static list<uint32_t> ExtendExtension(list<uint32_t> &extension_build, list<uint32_t> &initial_set)
{
	list<uint32_t> tmpCopy_1, tmpCopy_2;
	std::copy(extension_build.begin(), extension_build.end(), std::back_inserter(tmpCopy_1));
	std::copy(initial_set.begin(), initial_set.end(), std::back_inserter(tmpCopy_2));
	tmpCopy_1.merge(tmpCopy_2);
	return tmpCopy_1;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void check_rejection_parallel_recursiv(uint32_t argument, AF &framework, VectorBitSet &activeArgs, bool *isRejected,
	list<uint32_t> &extension_build, list<uint32_t> &output_extension, int &num_iterations, int limit_iterations)	//, int *num_tasks, int *num_tasks_max
{
//#pragma atomic write
	num_iterations++;

	if (num_iterations > limit_iterations) {
		return;
	}
	
	int id = omp_get_thread_num();
	bool isRejected_tmp = false;
	VectorBitSet reduct = extension_build.empty() ? activeArgs.copy() : Reduct::get_reduct_set(activeArgs, framework, extension_build);																			//DEBUG

	if (reduct._vector.size() < 2)
	{
		//there is only 1 active argument, this has to be the argument to check, if not then there should have been a rejection check earlier who did not work
		return;
	}

	bool *isSolved = NULL;						//flag used to signal that clauses need to be extended
	isSolved = (bool *)malloc(sizeof * isSolved);
	*isSolved = false;
	bool *isFirstCalculation = NULL;			//flag used to signal that reduct has only empty set as admissible set
	isFirstCalculation = (bool *)malloc(sizeof * isFirstCalculation);
	*isFirstCalculation = true;
	uint64_t numVars = reduct._vector.size();
	SatSolver *solver = NULL;
	solver = new SatSolver_cadical(numVars);
	Encoding::add_clauses_nonempty_admissible_set(*solver, framework, reduct);
	bool has_Solution = true;

	//iterate through initial sets
	do {
		if (*isSolved)
		{
			Encoding::add_complement_clause(*solver, reduct);
		}

		*isSolved = true;
		has_Solution = (*solver).solve();
		if (!has_Solution)
		{
			//no more initial sets to calculate after this one

			if (*isFirstCalculation)
			{
				//since this is the first calculation and no IS was found, this reduct has only the empty set as an admissible set, therefore the extension_build is complete
				// and since only extensions not containing the query argument proceed in the calculation, extension_build cannot contain the query argument, so that there exists
				// an extension not containing the query argument, so that the argument gets sceptical rejected
				
				*isRejected = true;

				free(isSolved);
				free(isFirstCalculation);
				delete solver;
				return;
			}

			break;
		}

		list<uint32_t> initial_set = Decoding::get_set_from_solver(*solver, reduct);
		
		if (initial_set.empty())
		{
			if (*isFirstCalculation)
			{
				//since this is the first calculation and only the empty set as IS was found, the extension_build is complete
				// and since only extensions not containing the query argument proceed in the calculation, extension_build cannot contain the query argument, so that there exists
				// an extension not containing the query argument, so that the argument gets sceptical rejected
				
				*isRejected = true;

				free(isSolved);
				free(isFirstCalculation);
				delete solver;
				initial_set.clear();
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
			*isRejected = true;			
			list<uint32_t> new_extension_build = ExtendExtension(extension_build, initial_set);	
			output_extension = new_extension_build;

			free(isSolved);
			free(isFirstCalculation);
			delete solver;
			initial_set.clear();
			return;
		}
		else if (ScepticalCheck::check_terminate_extension_build(argument, initial_set))
		{
			initial_set.clear();
			continue;
		}

		list<uint32_t> new_extension_build = ExtendExtension(extension_build, initial_set);		
		initial_set.clear();
		check_rejection_parallel_recursiv(argument, framework, activeArgs, isRejected, new_extension_build,
			output_extension, num_iterations, limit_iterations); //, num_tasks, num_tasks_max
		new_extension_build.clear();
		reduct = extension_build.empty() ? activeArgs.copy() : Reduct::get_reduct_set(activeArgs, framework, extension_build);
		isRejected_tmp = *isRejected;
	} while (has_Solution && !isRejected_tmp);

	free(isSolved);
	free(isFirstCalculation);
	delete solver;

	return;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static bool check_rejection_parallel(uint32_t argument, AF &framework, VectorBitSet &active_args, 
	list<uint32_t> &proof_extension, uint8_t numCores, int &num_iterations, int limit_iterations)
{	
	bool *isRejected = NULL;
	isRejected = (bool *)malloc(sizeof *isRejected);
	if (isRejected == NULL) {
		printf("Memory allocation failed\n");
		exit(1);
	}

	*isRejected = false;
	list<uint32_t> extension_build;
	check_rejection_parallel_recursiv(argument, framework, active_args, isRejected, extension_build, proof_extension, num_iterations, limit_iterations);
	bool result = *isRejected;
	free(isRejected);
	return result;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

bool Solver_DS_PR::solve(uint32_t argument, AF &framework, VectorBitSet &activeArgs, list<uint32_t> &proof_extension, uint8_t numCores,
	const std::filesystem::path file, bool is_verbose, int &num_iterations, int limit_iterations) {
	
	num_iterations = 0;
	return !check_rejection_parallel(argument, framework, activeArgs, proof_extension, numCores, num_iterations, limit_iterations);
}