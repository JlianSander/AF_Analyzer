#ifndef ENUMS_H
#define ENUMS_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <iostream>


enum task { DC, DS, SE, EE, CE, UNKNOWN_TASK };
enum semantics { IT, UC, PR, GR, UNKNOWN_SEM };
enum SOLVERS { CMS, CADICAL };

using namespace std;

class Enums {
public:

	/// <summary>
	/// This method converts the specified string into a task.
	/// </summary>
	/// <param name="task_abbreviation">String containing the abbreviation of the task as used in the enum.</param>
	/// <returns>Task corresponding to the specified string.</returns>
	static task string_to_task(string task_abbreviation);

	/// <summary>
	/// This method converts the specified string into a semantics.
	/// </summary>
	/// <param name="semantics_abbreviation">String containing the abbreviation of the semantics as used in the enum.</param>
	/// <returns>Semantics corresponding to the specified string.</returns>
	static semantics string_to_sem(string semantics_abbreviation);
};

#endif