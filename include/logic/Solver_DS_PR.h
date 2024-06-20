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

#include "../util/Printer.h"

using namespace std;

class Solver_DS_PR {
public:

	/// <summary>
	/// Checks if a specified argument is sceptically accepted.
	/// </summary>
	/// <param name="argument"> The argument, which could be sceptical accepted or not.</param>
	/// <param name="framework"> The abstract argumentation framework, specifying the underlying attack relations between the arguments.</param>
	/// <param name="activeArgs"> The active arguments at the beginning of the process.</param>
	/// <param name="proof_extension"> Extension proving, that the argument cannot be sceptically accepted.</param>
	/// <param name="numCores"> Number of cores requested to be used to solve the problem. Actual number can be lower depending on the OS scheduler.</param>
	/// <param name="file"> File of the instance that is being processed.</param>
	/// <param name="is_verbose"> If TRUE, then comments are added to the console about the execution.</param>
	/// <param name="limit_level"> Recursive depth that can be used to solved the instance at maximum.</param>
	/// <param name="out_num_iterations">Number of recursive calls until the solution was calculated.</param>
	/// <param name="limit_iterations">Number of recursive calls that can be used to solve the instance at maximum.</param>
	/// <returns>Depth of the level of the solution. -1 means no solution could be calculated within the specified limit.</returns>
	static int solve(uint32_t argument, AF &framework, VectorBitSet &activeArgs, list<uint32_t> &proof_extension, uint8_t numCores,
		const std::filesystem::path file, bool is_verbose, int limit_level, int &out_num_iterations, int limit_iterations);
};

#endif


