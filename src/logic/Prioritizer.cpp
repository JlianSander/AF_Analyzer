#include "../../include/logic/Prioritizer.h"


uint32_t Prioritizer::calculate_priority(AF &framework, list<uint32_t> &set_arguments) {

	if (set_arguments.empty()) {
		throw new exception;
	}
	uint32_t num_args = framework.num_args;
	uint32_t prio = num_args - framework.distance_to_query[*set_arguments.begin()];

	for (list<uint32_t>::iterator mIter = std::next(set_arguments.begin()); mIter != set_arguments.end(); ++mIter) {
		uint32_t newPrio = num_args - framework.distance_to_query[*mIter];

		if (newPrio > prio) {
			prio = newPrio;
		}
	}

	return prio;
}