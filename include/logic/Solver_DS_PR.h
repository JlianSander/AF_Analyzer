#ifndef SOLVER_DS_PR_H
#define SOLVER_DS_PR_H

#include <iostream>
#include <cstdint>
#include <vector>

extern "C" {
	#include "../util/MemoryWatchDog.h"
}

#include "AF.h"
#include "Encoding.h"
#include "Enums.h"
#include "Decoding.h"
#include "PreProcessor_DS_PR.h"
#include "Prioritizer.h"
#include "Reduct.h"
#include "SatSolver.h"
#include "SatSolver_cadical.h"
#include "ScepticalCheck.h"
#include "omp.h"

#include "../util/Printer.h"

using namespace std;

class Solver_DS_PR {
public:

	/// <summary>
	/// Checks if a specified argument is sceptically accepted.
	/// </summary>
	/// <param name="argument">The argument, which could be sceptical accepted or not.</param>
	/// <param name="framework">The abstract argumentation framework, specifying the underlying attack relations between the arguments.</param>
	/// <param name="proof_extension"> Extension proving, that the argument cannot be sceptically accepted.</param>
	/// <param name="numCores"> Number of cores requested to be used to solve the problem. Actual number can be lower depending on the OS scheduler.</param>
	/// <returns>TRUE iff the argument is sceptical accepted. FALSE otherwise.</returns>
	static bool solve(uint32_t argument, AF &framework, list<uint32_t> &proof_extension, uint8_t numCores);
};

#endif


