#include "../../include/logic/Reduct.h"

VectorBitSet Reduct::get_reduct(VectorBitSet &activeArguments, AF &framework, uint32_t argument)
{
	vector<uint32_t> reduct_vector;
	vector<uint8_t> reduct_bitset;
	reduct_bitset.resize(static_cast<uint64_t>(framework.num_args) + 1);

	for (int i = 0; i < activeArguments._vector.size(); i++) {

		if (activeArguments._vector[i] == argument || framework.victims[argument]._bitset[activeArguments._vector[i]]) {
			continue;
		}

		reduct_vector.push_back(activeArguments._vector[i]);
		reduct_bitset[activeArguments._vector[i]] = true;
	}

	reduct_vector.shrink_to_fit();
	return VectorBitSet(reduct_vector, reduct_bitset);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

VectorBitSet Reduct::get_reduct_set(VectorBitSet &activeArguments, AF &framework, list<uint32_t> &set_arguments)
{
	if (set_arguments.empty()) {
		throw new exception;
	}

	//printf("%d: ------- before getReduct --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());												//DEBUG
	// do not free activeArguments, since its a given parameter and hence out of this method's responsibility
	VectorBitSet reduct = get_reduct(activeArguments, framework, *set_arguments.begin());
	//printf("%d: ------- after getReduct --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());													//DEBUG
	if (reduct._vector.empty()) {
		return reduct;
	}

	for (list<uint32_t>::iterator mIter = std::next(set_arguments.begin()); mIter != set_arguments.end(); ++mIter) {
		if (reduct._vector.empty())
		{
			break;
		}

		// free all temporary reducts
		//printf("%d: ------- before getReduct --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());											//DEBUG
		VectorBitSet reduct_tmp = get_reduct(reduct, framework, *mIter);
		reduct.clear();
		reduct = reduct_tmp;
		//printf("%d: ------- after getReduct + free tmp_Reduct --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());							//DEBUG
	}

	return reduct;
}