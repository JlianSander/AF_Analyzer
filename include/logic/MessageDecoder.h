#ifndef MESSAGE_DECODER_H
#define MESSAGE_DECODER_H

#include <stdint.h>
#include <iostream>

#include "Statistics.h"

using namespace std;

class MessageDecoder {
public:
	static void decode_message(Statistics &stats, int msg_value);
};
#endif