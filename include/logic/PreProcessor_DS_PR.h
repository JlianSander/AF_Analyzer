#ifndef PREPROC_DS_PR_H
#define PREPROC_DS_PR_H

#include <iostream>
#include <cstdint>
#include <filesystem>
#include <list>

#include "../Main.h"
#include "AF.h"
#include "Reduct.h"

#include "omp.h"

#include "../util/VectorBitSet.h"
#include "../logic/Enums.h"

extern "C" {
	#include "../util/MemoryWatchDog.h"
}

enum pre_proc_result { accepted, rejected, unknown };

class PreProc_DS_PR {
public:
	//returns 1 if argument was accepted, 2 if argument was  
	static pre_proc_result process(AF &framework, uint32_t argument, VectorBitSet &out_reduct, const std::filesystem::path file, bool is_verbose, int &code_msg);
};
#endif