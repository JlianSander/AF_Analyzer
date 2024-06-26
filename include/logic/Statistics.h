#ifndef STATISTICS_H
#define STATISTICS_H

using namespace std;

class Statistics {
public:
	int num_query_selfattack = 0;
	int num_query_no_attacker = 0;
	int num_query_grounded_contained = 0;
	int num_query_grounded_rejected = 0;
	int num_not_solved_preprocessor = 0;
	int num_files_terminated = 0;
	int num_files_timeout = 0;
	int num_files_processed = 0;
	int num_args_coi_base = 0;
	double num_args_coi_reducted_procent = 0;
	int num_args_gr_base = 0;
	double num_args_gr_reducted_procent = 0;
	int num_args_coi_gr_base = 0;
	double num_args_coi_gr_reducted_procent = 0;
	int num_files_solved_fst_iteration = 0;
	int num_files_solved_fst_level = 0;
};

#endif