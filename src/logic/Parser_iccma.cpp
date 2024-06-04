#include "../../include/logic/Parser_iccma.h"

using namespace std;

void ParserICCMA::parse_af(AF &framework, string file)
{
	//long mem_base = get_mem_usage();																											//DEBUG
	//float start_time = omp_get_wtime();																										//DEBUG
	ifstream input;
	input.open(file);

	if (!input.good()) {
		cerr << "Cannot open input file\n";
		exit(1);
	}

	string line;
	int32_t n_args = 0;
	
	getline(input, line);
	std::istringstream iss(line);
	if (line[0] == 'p') {
		string p, af;
		iss >> p >> af >> n_args;
	}
	else {
		cerr << "File does not comply to ICCMA'23 format\n";
		exit(1);
	}

	framework.initialize(n_args);

	//printf("%d: ------- setup initialization framework --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());						//DEBUG
	//long mem_base = get_mem_usage();																											//DEBUG

	
	//printf("Start parsing\n");																												//DEBUG
	while (!input.eof()) {
		getline(input, line);
		if (line.length() == 0 || line[0] == '#') continue;
		std::istringstream iss(line);
		uint32_t attacker, victim;
		iss >> attacker >> victim;
		framework.add_attack(attacker, victim);
	}
	//printf("Finished parsing\n");																												//DEBUG

	input.close();
	//printf("%d: ------- finished reading file --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());								//DEBUG
	//printf("Memory space needed to process AF as input: %ld/%ld [kB]\n", get_mem_usage() - mem_base, get_mem_usage());						//DEBUG

	//float end_time = omp_get_wtime();																											//DEBUG
	//float duration = end_time - start_time;																									//DEBUG
	//printf("runtime parse_af [s]: %.2f s\n", duration);																						//DEBUG

	framework.finish_initilization();
}