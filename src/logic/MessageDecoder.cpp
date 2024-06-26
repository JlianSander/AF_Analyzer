#include "../../include/logic/MessageDecoder.h"

void MessageDecoder::decode_message(Statistics &stats, int msg_value) {

	switch (msg_value) {
	case 1:
		stats.num_query_selfattack++;
		break;
	case 2:
		stats.num_query_no_attacker++;
		break;
	case 3:
		stats.num_query_grounded_contained++;
		break;
	case 4:
		stats.num_query_grounded_rejected++;
		break;
	case 5:
		stats.num_not_solved_preprocessor++;
		break;
	case 6:
		stats.num_files_terminated++;
		break;
	case 7:
		stats.num_files_solved_fst_iteration++;
		stats.num_not_solved_preprocessor++;
		break;
	case 8:
		stats.num_files_solved_fst_level++;
		stats.num_not_solved_preprocessor++;
		break;
	case 9:
		stats.num_files_timeout++;
		break;
	default:
		cout << "ERROR value not known to decoder." << endl;
	}
}