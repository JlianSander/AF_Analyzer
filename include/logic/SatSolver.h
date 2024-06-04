#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <iostream>
#include <cstdint>
#include <vector>

class SatSolver {
public:

	/// <summary>
	/// Adds a new clause to the SAT-problem of the solver.
	/// </summary>
	/// <param name="clause">The SAT-clause to add.</param>
	virtual void add_clause(std::vector<int64_t> clause) = 0;

	/// <summary>
	/// Adds a new clause, containing up to 2 literals, to the SAT-problem of the solver.
	/// </summary>
	/// <param name="lit1">First literal of the clause to add.</param>
	/// <param name="lit2">Second literal of the clause to add. In case clause has only 1 literal, this parameter is NULL</param>
	virtual void add_clause_short(int64_t lit1, int64_t lit2) = 0;

	/// <summary>
	/// Returns the solution for the SAT-problem of the solver. Rejecting variable are returned after all accepting variables are returned.
	/// </summary>
	/// <returns>Array of type bool, indicating for each variable if it's set or not</returns>
	virtual std::vector<bool> get_model() = 0;

	/// <summary>
	/// Method to check if single variable is part of the model.
	/// </summary>
	/// <param name="variable"></param>
	/// <returns>TRUE if the variable is true in the model.</returns>
	virtual std::uint8_t check_var_model(int64_t variable) = 0;

	/// <summary>
	/// This method calculates a solution for the SAT-Problem of the solver.
	/// </summary>
	/// <returns> <c>TRUE</c> iff a solution was found. Otherwise <c>FALSE</c></returns>
	virtual bool solve() = 0;
};


#endif