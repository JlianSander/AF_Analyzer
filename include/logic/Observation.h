#ifndef MESSAGE_H
#define MESSAGE_H

using namespace std;

class Observation {
public:
	int Executions_Code;
	int Number_Args;
	int Number_Args_COI;
	int Number_Args_COI_GR;
	int Number_Args_GR;
	int Is_Solved;
	int Level; 
	int Iterations;
	int Is_Solved_PreProc;

	Observation() {
		Executions_Code = -1;
		Number_Args = -1;
		Number_Args_COI = -1;
		Number_Args_COI_GR = -1;
		Number_Args_GR = -1;
		Is_Solved = -1;
		Level = -1;
		Iterations = -1;
		Is_Solved_PreProc = -1;
	}
};
#endif