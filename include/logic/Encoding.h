#ifndef ENCODINGS_SATSOLVER_H
#define ENCODINGS_SATSOLVER_H

#include <cstdint>
#include <vector>
#include <unordered_set>

#include "AF.h"
#include "SatSolver.h"

#include "../util/VectorBitSet.h"
#include "../util/Printer.h"


using namespace std;
/*
Class to encode a problem to be solved with a cryptominisat-solver
*/
class Encoding {
public:
    /// <summary>
    /// Adds all clauses necessary to encode an initial set.
    /// </summary>
    /// <param name="solver">The SATSolver, to which the clauses will be added.</param>
    /// <param name="framework">The abstract argumentation framework, based upon which the attacks are analysed.</param>
    /// <param name="activeArgs">The set of active arguments, in the current state of the framework.</param>
    static void add_clauses_nonempty_admissible_set(SatSolver &solver, AF &framework, VectorBitSet &activeArgs);
    /// <summary>
    /// Adds a complement clause to a found solution to a specified SATSolver. This clause is necessary to provoke the solver to
    /// find another solution to the problem.
    /// </summary>
    /// <param name="solver">The SATSolver, to which a solution was found</param>
    /// <param name="activeArgs">The size of the set of active arguments, in the current state of the framework.</param>
    static void add_complement_clause(SatSolver &solver, VectorBitSet &activeArgs);
};

#endif