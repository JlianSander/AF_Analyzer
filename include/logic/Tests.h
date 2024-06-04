#ifndef TESTS_H
#define TESTS_H

#include <wchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <omp.h>

//extern "C" {
//	#include "../include/util/MemoryWatchDog.h"
//}

#include "AF.h"
#include "Parser_iccma.h"
#include "Solver_DS_PR.h"

#include "Enums.h"

constexpr auto NUM_CORES_TESTS = 0;

using namespace std;

class TestCases {
public:

	/// <summary>
	/// Runs different test cases on the solver
	/// </summary>
	static void run_Tests();
};

#endif
