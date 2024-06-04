#include "../../include/logic/PreProcessor_DS_PR.h"

static VectorBitSet calculate_cone_influence(AF &framework, uint32_t query) {
	//long mem_base = get_mem_usage();																												//DEBUG
	vector<uint32_t> active_args_vector;
	vector<uint8_t> active_args_bitset(framework.num_args + 1, 0);

	list<uint32_t> ls_args_unprocessed;
	ls_args_unprocessed.push_back(query);
	framework.distance_to_query[query] = 0;
	active_args_vector.push_back(query);
	active_args_bitset[query] = true;

	for (list<uint32_t>::iterator mIter = ls_args_unprocessed.begin(); mIter != ls_args_unprocessed.end(); ++mIter) {
		const auto &argument = *mIter;
		uint32_t distance = framework.distance_to_query[argument];
		for (int i = 0; i < framework.attackers[argument]._vector.size(); i++) {
			uint32_t attacker = framework.attackers[argument]._vector[i];
			if (framework.distance_to_query[attacker] > 0 || attacker == query) {
				//attacker was already visited
				continue;
			}

			//increase distance to query for attacker
			framework.distance_to_query[attacker] = distance + 1;

			active_args_vector.push_back(attacker);
			active_args_bitset[attacker] = true;

			ls_args_unprocessed.push_back(attacker);
		}
	}

	VectorBitSet active_args = VectorBitSet(active_args_vector, active_args_bitset);
	//printf("Memory space of initialized active arguments: %ld/%ld [kB]\n", get_mem_usage() - mem_base, get_mem_usage());							//DEBUG
	return active_args;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/


static pre_proc_result reduce_by_grounded(AF &framework, VectorBitSet &active_args, uint32_t query, VectorBitSet &out_reduct)
{
	// fill list with unattacked arguments
	list<uint32_t> ls_unattacked_unprocessed;
	vector<uint32_t> num_attacker;
	num_attacker.resize(framework.num_args + 1);
	//iterate through active arguments
	for (int i = 0; i < active_args._vector.size(); i++) {
		//check if argument is unattacked
		if (framework.attackers[active_args._vector[i]]._vector.empty()) {
			ls_unattacked_unprocessed.push_back(active_args._vector[i]);
		}

		num_attacker[active_args._vector[i]] = framework.attackers[active_args._vector[i]]._vector.size();
	}

	// init variable of current reduct
	out_reduct = active_args;

	//process list of unattacked arguments
	for (list<uint32_t>::iterator mIter = ls_unattacked_unprocessed.begin(); mIter != ls_unattacked_unprocessed.end(); ++mIter) {
		const auto &ua = *mIter;

		//reject query if it gets attacked by argument of grounded extension
		if( ua == query) {
			return pre_proc_result::accepted;
		}


		//reject query if it gets attacked by argument of grounded extension
		if (framework.victims[ua]._bitset[query]) {
			return pre_proc_result::rejected;
		}

		//iterate through victims of the victims of ua
		for (int i = 0; i < framework.victims[ua]._vector.size(); i++) {
			uint32_t vua = framework.victims[ua]._vector[i];

			if (!out_reduct._bitset[vua]) {
				//only account victims that are still active
				continue;
			}

			for (int j = 0; j < framework.victims[vua]._vector.size(); j++) {
				uint32_t vvua = framework.victims[vua]._vector[j];

				if (!out_reduct._bitset[vvua]) {
					//only account victims of victims that are still active
					continue;
				}

				//update number of attackers
				num_attacker[vvua]--;

				//check if victim of victim is unattacked
				if (num_attacker[vvua] == 0) {
					ls_unattacked_unprocessed.push_back(vvua);
				}
			}
		}

		//reduce active argument by unattacked argument + update current reduct
		out_reduct = Reduct::get_reduct(out_reduct, framework, ua);
	}

	return pre_proc_result::unknown;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

pre_proc_result PreProc_DS_PR::process(AF &framework, uint32_t query, VectorBitSet &out_reduct) {
	//long mem_base = get_mem_usage();																													//DEBUG

	if (framework.victims[query]._bitset[query])
	{
		return pre_proc_result::rejected;
	}

	if (framework.attackers[query]._vector.empty())
	{
		return pre_proc_result::accepted;
	}

	VectorBitSet active_args = calculate_cone_influence(framework, query);
	
	pre_proc_result result =  reduce_by_grounded(framework, active_args, query, out_reduct);

	//printf("PreProc_DS_PR::process() finished - voluntary context switches: %ld - involuntary context switches: %ld - memory usage: %ld/%ld [kB]\n",
	//	get_ctxt_switches_volun(), get_ctxt_switches_involun(), get_mem_usage() - mem_base, get_mem_usage());											//DEBUG

	return result;
}
