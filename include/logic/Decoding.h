#ifndef DECODINGS_SATSOLVER_H
#define DECODINGS_SATSOLVER_H

#include <cstdint>
#include <list>
#include <unordered_set>

#include "SatSolver.h"

#include "../util/VectorBitSet.h"

using namespace std;

class Decoding {
public:

	/// <summary>
	/// Translates the solution of a specified SAT-problem to a set of argument, in the context of a specified set of active arguments.
	/// </summary>
	/// <param name="problem">The SAT-Solver, which solution if to be tranlated.</param>
	/// <param name="activeArgs">The set of active arguments, in the current state of the framework.</param>
	/// <returns>Set of arguments, which solve the specified SAT-problem.</returns>
	static list<uint32_t> get_set_from_solver(SatSolver &solver, VectorBitSet &activeArgs);

};
#endif