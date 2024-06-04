#include "../../include/logic/AF.h"

using namespace std;

bool AF::add_attack(uint32_t attacker, uint32_t victim)
{
	if (attacker < 0 || victim < 0) return false;
	
	if (attacks.count(make_pair(attacker, victim))) {
		return false;
	}
	attacks.insert(make_pair(attacker, victim));
	return true;
}

void AF::initialize(uint32_t number_args) {
	num_args = number_args;
	attackers.resize(static_cast<uint64_t>(num_args) + 1);
	victims.resize(static_cast<uint64_t>(num_args) + 1);
	self_attack.resize(static_cast<uint64_t>(num_args) + 1);
	std::fill(self_attack.begin(), self_attack.end(), 0);
	distance_to_query.resize(static_cast<uint64_t>(num_args) + 1);
	std::fill(distance_to_query.begin(), distance_to_query.end(), 0);
}

void AF::finish_initilization()
{
	//float start_time = omp_get_wtime();																										//DEBUG	

	const uint64_t tmp_num_args = static_cast<uint64_t>(num_args) + 1;
	vector<vector<uint32_t>> attackers_vectors;
	attackers_vectors.resize(tmp_num_args);
	vector<vector<uint8_t>> attackers_bitsets;
	attackers_bitsets.resize(tmp_num_args);
	vector<vector<uint32_t>> victims_vectors;
	victims_vectors.resize(tmp_num_args);
	vector<vector<uint8_t>> victims_bitsets;
	victims_bitsets.resize(tmp_num_args);
	for (int i = 0; i < tmp_num_args; i++) {
		attackers_bitsets[i].resize(tmp_num_args);
		victims_bitsets[i].resize(tmp_num_args);
	}

	for (const pair<uint32_t, uint32_t> &attack : attacks) {
		int32_t source = attack.first;
		int32_t target = attack.second;
		attackers_vectors[target].push_back(source);
		attackers_bitsets[target][source] = true;
		victims_vectors[source].push_back(target);
		victims_bitsets[source][target] = true;

		if (source == target)
			self_attack[source] = true;
		if (attacks.count(make_pair(target, source))) {
			symmetric_attacks.insert(make_pair(source, target));
			symmetric_attacks.insert(make_pair(target, source));
		}
	}

	for (int i = 0; i < attackers.size(); i++) {
		attackers_vectors[i].shrink_to_fit();
		attackers[i] = VectorBitSet(attackers_vectors[i], attackers_bitsets[i]);
	}
	attackers_vectors.clear();
	attackers_bitsets.clear();

	for (int i = 0; i < victims.size(); i++) {
		victims_vectors[i].shrink_to_fit();
		victims[i] = VectorBitSet(victims_vectors[i], victims_bitsets[i]);
	}
	victims_vectors.clear();
	victims_bitsets.clear();

	self_attack.shrink_to_fit();

	//float end_time = omp_get_wtime();																											//DEBUG
	//float duration = end_time - start_time;																									//DEBUG
	//printf("runtime finish_initilization [s]: %.2f s\n", duration);																			//DEBUG
}

bool AF::exists_attack(uint32_t attacker, uint32_t victim) const {
	return attacks.count(make_pair(attacker, victim));
}