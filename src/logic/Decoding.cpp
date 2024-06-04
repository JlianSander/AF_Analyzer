#include "../../include/logic/Decoding.h"

using namespace std;
using std::vector;

list<uint32_t> Decoding::get_set_from_solver(SatSolver &solver, VectorBitSet &activeArgs)
{
	list<uint32_t> output;

	for (int i = 0; i < activeArgs._vector.size(); i++) {
		if (solver.check_var_model(activeArgs._vector[i]))
		{
			output.push_back(activeArgs._vector[i]);
		}
	}

	return output;
}