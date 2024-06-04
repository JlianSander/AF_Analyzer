#include "../../include/logic/Enums.h"


task Enums::string_to_task(string task)
{
	if (task == "DC") return DC;
	if (task == "DS") return DS;
	if (task == "SE") return SE;
	if (task == "EE") return EE;
	if (task == "CE") return CE;
	return UNKNOWN_TASK;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

semantics Enums::string_to_sem(string semantics)
{
	if (semantics == "IT") return IT;
	if (semantics == "UC") return UC;
	if (semantics == "PR") return PR;
	if (semantics == "GR") return GR;
	return UNKNOWN_SEM;
}