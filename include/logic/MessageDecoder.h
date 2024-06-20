#ifndef MESSAGE_DECODER_H
#define MESSAGE_DECODER_H

#include <stdint.h>
#include <iostream> 

using namespace std;

class MessageDecoder {
public:
	static void decode_message(int msg_value, int &num_query_selfattack, int &num_query_no_attacker,
		int &num_query_grounded_contained, int &num_query_grounded_rejected, int &num_files_terminated_preprocessor, 
		int &num_not_solved_preprocessor, int &num_files_solved_fst_iteration, int &num_files_solved_fst_level);
};
#endif