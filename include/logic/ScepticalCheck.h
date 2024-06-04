#ifndef SCEPTICAL_PR_CHECK_H
#define SCEPTICAL_PR_CHECK_H

#include <vector>
#include <cstdint>
#include <list>

#include "AF.h"

extern "C" {
	#include "../util/MemoryWatchDog.h"
}

using namespace std;

	class ScepticalCheck {
	public:
		/// <summary>
		/// Checks if a query about the sceptical acceptance of a specified argument can be rejected using the specified initial set. This is the
		/// case if any argument of the initial set attacks the argument in question.
		/// </summary>
		/// <param name="argument">The argument, which could be sceptical accepted or not.</param>
		/// <param name="initial_set">Initial set of one reduct of the framework.</param>
		/// <param name="framework">The abstract argumentation framework, specifying the underlying attack relations between the arguments.</param>
		/// <returns>TRUE iff the sceptical acceptance is rejected by this initial set. FALSE otherwise.</returns>
		static bool check_rejection(uint32_t argument, list<uint32_t> &initial_set, AF &framework);

		/// <summary>
		/// Checks if the generation of an extension, using the specified initial set, can be terminated, since the initial set contains the argument in
		/// question, and can therefore never lead to an extension, with an argument attacking the specified argument, which would lead into rejecting the 
		/// sceptical acceptance.
		/// </summary>
		/// <param name="argument">The argument, which could be sceptical accepted or not.</param>
		/// <param name="initial_set">The initial set, which is used to generate an extension.</param>
		/// <returns>TRUE iff the generation of an extension can be stopped. FALSE otherwise.</returns>
		static bool check_terminate_extension_build(uint32_t argument, list<uint32_t> &initial_set);
};
#endif
