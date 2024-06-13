#include "../../include/logic/MessageDecoder.h"

void MessageDecoder::decode_message(int msg_value, int &num_query_selfattack, int &num_query_no_attacker,
	int &num_query_grounded_contained, int &num_query_grounded_rejected, int &num_files_terminated_preprocessor, int &num_not_solved_preprocessor) {

	switch (msg_value) {
	case 1:
		num_query_selfattack++;
		break;
	case 2:
		num_query_no_attacker++;
		break;
	case 3:
		num_query_grounded_contained++;
		break;
	case 4:
		num_query_grounded_rejected++;
		break;
	case 5:
		num_not_solved_preprocessor++;
		break;
	case 6:
		num_files_terminated_preprocessor++;
		break;
	default:
		cout << "ERROR value not known to decoder." << endl;
	}
}