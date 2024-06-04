#ifndef PRIORITIZER_H
#define PRIORITIZER_H

#include <iostream>
#include <cstdint>
#include <list>

#include "AF.h"

class Prioritizer {
public:
	//calculates the priority for the specified set of arguments 
	static uint32_t calculate_priority(AF &framework, list<uint32_t> &set_args);
};
#endif