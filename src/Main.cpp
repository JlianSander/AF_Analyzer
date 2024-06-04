#include "../include/Main.h"

using namespace std;

void static print_usage()
{
	cout << "Usage: " << SOLVERNAME << " -p <task> -f <file> -fo <format> [-a <query>]\n\n";
	cout << "  <task>      computational problem; for a list of available problems use option --problems\n";
	cout << "  <file>      input argumentation framework\n";
	cout << "  <format>    file format for input AF; for a list of available formats use option --formats\n";
	cout << "  <query>     query argument\n";
	cout << "Options:\n";
	cout << "  --help      Displays this help message.\n";
	cout << "  --version   Prints version and author information.\n";
	cout << "  --formats   Prints available file formats.\n";
	cout << "  --problems  Prints available computational tasks.\n";
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void static print_version()
{
	cout << SOLVERNAME << " (version "<< VERSIONNUMBER <<")\n"
		<< "Lars Bengel, University of Hagen <lars.bengel@fernuni-hagen.de>\n" 
		<< "Julian Sander, University of Hagen <julian.sander@fernuni-hagen.de>\n";
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void static print_formats()
{
	cout << "[i23]\n";
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void static print_problems()
{
	/*vector<string> tasks = { "DC", "DS", "SE", "EE", "CE" };
	vector<string> sems = { "IT", "PR", "UC" };*/
	vector<string> tasks = { "DS"};
	vector<string> sems = { "PR"};
	cout << "[";
	for (uint32_t i = 0; i < tasks.size(); i++) {
		for (uint32_t j = 0; j < sems.size(); j++) {
			string problem = tasks[i] + "-" + sems[j];
			if (j != sems.size() - 1)
			{
				cout << problem << ",";
			}
			else
			{
				cout << problem;
			}
		}
	}
	cout << "]\n";
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

int execute(int argc, char **argv)
{
	if (argc == 1) {
		print_version();
		return 0;
	}

	int option_index = 0;
	int opt = 0;
	string problem, file, fileformat, query;

	while ((opt = getopt_long_only(argc, argv, "", longopts, &option_index)) != -1) {
		switch (opt) {
		case 0:
			break;
		case 'p':
			problem = optarg;
			break;
		case 'f':
			file = optarg;
			break;
		case 'o':
			fileformat = optarg;
			break;
		case 'a':
			query = optarg;
			break;
		default:
			return 1;
		}
	}

	if (version_flag) {
		print_version();
		return 0;
	}

	if (usage_flag) {
		print_usage();
		return 0;
	}

	if (formats_flag) {
		print_formats();
		return 0;
	}

	if (problems_flag) {
		print_problems();
		return 0;
	}

	if (problem.empty()) {
		cerr << argv[0] << ": Problem must be specified via -p flag\n";
		return 1;
	}

	if (file.empty()) {
		cerr << argv[0] << ": Input file must be specified via -f flag\n";
		return 1;
	}

	//printf("Set file format\n");																										//DEBUG

	if (fileformat.empty()) {
		fileformat = file.substr(file.find_last_of(".") + 1, file.length() - file.find_last_of(".") - 1);
		/*cerr << argv[0] << ": File format must be specified via -fo flag\n";
		return 1;*/
	}

	AF framework;
	if (fileformat == "i23") {
		ParserICCMA::parse_af(framework, file);
	}
	else {
		cerr << argv[0] << ": Unsupported file format\n";
		return 1;
	}

	string task = problem.substr(0, problem.find("-"));
	problem.erase(0, problem.find("-") + 1);
	string sem = problem.substr(0, problem.find("-"));
	switch (Enums::string_to_task(task)) {
		case DS:
		{
			if (query.empty()) {
				cerr << argv[0] << ": Query argument must be specified via -a flag\n";
				return 1;
			}

			uint32_t argument = std::stoi(query);
			list<uint32_t> proof_extension;
			bool skept_accepted = false;

			switch (Enums::string_to_sem(sem)) {
				case PR:
					skept_accepted = Solver_DS_PR::solve(argument, framework, proof_extension, NUM_CORES);
					break;
				default:
					cerr << argv[0] << ": Unsupported semantics\n";
					return 1;
			}

			cout << (skept_accepted ? "YES" : "NO") << endl;
			if (!skept_accepted)
			{
				/*if (*proof_extension != NULL)
				{
					EXTENSIONSOLVER::BuildExtension(framework, actives, proof_extension);
				}*/
				cout << "w " << endl;

				if (!proof_extension.empty()) {
					for (list<uint32_t>::iterator mIter = proof_extension.begin(); mIter != proof_extension.end(); ++mIter) {
						cout << *mIter << " ";
					}
					proof_extension;
					cout << endl;
				}
			}

			//free allocated memory
			proof_extension.clear();
			break;
		}
		default:
			cerr << argv[0] << ": Problem not supported!\n";
			return 1;
	}

	return 0;
}

int test(int argc, char **argv)
{

	TestCases::run_Tests();

	return 0;
}

int main(int argc, char **argv)
{
	execute(argc, argv);
	//test(argc, argv)
}