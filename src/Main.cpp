#include "../include/Main.h"

using namespace std;

void static print_usage()
{
	cout << "Usage: " << PROGAMNAME << " -d <directory> \n\n";
	cout << "  <directory>    container of  argumentation frameworks\n";
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
	cout << PROGAMNAME << " (version "<< VERSIONNUMBER <<")\n"
		<< "Lars Bengel, University of Hagen <lars.bengel@fernuni-hagen.de>\n" 
		<< "Julian Sander, University of Hagen <julian.sander@fernuni-hagen.de>\n";
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void static print_formats()
{
	cout << "AF: [i23]" << endl;
	cout << "Query: [af.arg]" << endl;
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

uint32_t static read_query(std::filesystem::__cxx11::directory_entry &file)
{
	filesystem::path query_file{ file.path() }, extension_query{ ".af.arg" };
	query_file.replace_extension(extension_query);
	ifstream input;
	input.open(query_file);

	if (!input.good()) {
		cerr << "Cannot open query file\n";
		exit(1);
	}

	string line, query;

	getline(input, line);
	std::istringstream iss(line);
	iss >> query;
	return std::stoi(query);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

int calculateSolution(uint32_t query, AF &framework, VectorBitSet &initial_reduct, const std::filesystem::path file, bool is_verbose)
{
	list<uint32_t> proof_extension;
	bool skept_accepted = false;
	int num_iterations = 0;
	int level_solution = Solver_DS_PR::solve(query, framework, initial_reduct, proof_extension, file, is_verbose, 
		LIMIT_CALCULATION_LEVEL, num_iterations, LIMIT_ITERATIONS);
	//free allocated memory
	proof_extension.clear();

	if (level_solution == 1 && num_iterations == 1) {

		if (is_verbose) {
			cout << file.filename() << "#### solved in one iteration" << endl;
		}
		return 7;
	}
	else if (level_solution == 1 && num_iterations > 1) {
		if (is_verbose) {
			cout << file.filename() << "#### solved without recursivity but not as first calculated set" << endl;
		}
		return 8;
	}
	else {
		return 5;
	}
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

int static start_pre_processor(uint32_t query, AF &framework, const std::filesystem::path file, VectorBitSet &out_final_reduct, int &num_args_coi, int &num_args_reduc_coi_gr, int &num_args_gr) {
	int exec_code;
	pre_proc_result result_preProcessor = PreProc_DS_PR::process(framework, query, out_final_reduct, file, true, exec_code,
		num_args_coi, num_args_reduc_coi_gr, num_args_gr);
	return exec_code;
}


/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

int handleFile(filesystem::directory_entry file, int &num_args, int &num_args_coi, int &num_args_reduc_coi_gr, int &num_args_gr) {

	string file_format = file.path().extension();

	if (file_format != ".i23") {
		//cerr << " Unsupported file format: " << file_format << endl;
		return -1;
	}
	else {
		cout << endl;
		cout << "Processing: " << file.path().filename() << endl;
	}

	AF framework;
	ParserICCMA::parse_af(framework, file.path());
	num_args = framework.num_args;
	uint32_t query = read_query(file);
	VectorBitSet initial_reduct_solver = VectorBitSet();
	int exec_code = start_pre_processor(query, framework, file.path(), initial_reduct_solver, num_args_coi, num_args_reduc_coi_gr, num_args_gr);
	if (exec_code == 5) {
		//instance was not solved during preprocessing
		exec_code = calculateSolution(query, framework, initial_reduct_solver, file.path(), true);
	}

	return exec_code;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void print_statistics() {
	cout << "[PreProcessor]Instances with query self-attacks: " << num_query_selfattack << "/" << num_files_processed << endl;
	cout << "[PreProcessor]Instances with unattacked queries: " << num_query_no_attacker << "/" << num_files_processed << endl;
	cout << "[PreProcessor]Instances where query was part of grounded extension: " << num_query_grounded_contained << "/" << num_files_processed << endl;
	cout << "[PreProcessor]Instances where query was rejected by grounded extension: " << num_query_grounded_rejected << "/" << num_files_processed << endl;
	cout << "[PreProcessor]Instances which were terminated: " << num_files_terminated_preprocessor << "/" << num_files_processed << endl;
	cout << "[PreProcessor]Instances not solved during preprocessing: " << num_not_solved_preprocessor << "/" << num_files_processed << endl;
	std::cout << std::setprecision(4);
	cout << "[PreProcessor]Average number of arguments reduced by cone of influence: " << num_args_coi_reducted_procent << "/100" << endl;
	cout << "[PreProcessor]Average number of arguments reduced by grounded extension after calculating cone of influence: " 
		<< num_args_coi_gr_reducted_procent << "/100" << endl;
	cout << "[PreProcessor]Average number of arguments reduced by grounded extension alone: " << num_args_gr_reducted_procent << "/100" << endl;
	std::cout << std::setprecision(2);
	cout << "[Solver]Instances solved with 1st calculated set: " << num_files_solved_fst_iteration << "/" << num_not_solved_preprocessor << endl;
	cout << "[Solver]Instances solved at 1st level: " << num_files_solved_fst_level << "/" << num_not_solved_preprocessor << endl;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void decode(int msg_code)
{
	if (msg_code > 0) {
		//file was processed
		MessageDecoder::decode_message(msg_code, num_query_selfattack, num_query_no_attacker,
			num_query_grounded_contained, num_query_grounded_rejected, num_files_terminated_preprocessor,
			num_not_solved_preprocessor, num_files_solved_fst_iteration, num_files_solved_fst_level);
		//count file since returned value was > 0
		num_files_processed++;
	}
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void static updateProcent(int &base, double &cur_val_procent, int new_base, int new_val_absolut) {
	double new_val_procent;
	if (new_base != 0) {
		new_val_procent = (new_val_absolut * 100.0) / new_base ;
	}
	else {
		new_val_procent = 0;
	}
	cur_val_procent = (new_val_procent * new_base) / (new_base + base)
		+ (cur_val_procent * base) / (new_base + base);
	base += new_base;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/


int main(int argc, char **argv)
{
	if (argc == 1) {
		print_version();
		return 0;
	}

	int option_index = 0;
	int opt = 0;
	string dir;

	while ((opt = getopt_long_only(argc, argv, "", longopts, &option_index)) != -1) {
		switch (opt) {
		case 0:
			break;
		case 'd':
			dir = optarg;
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

	if (dir.empty()) {
		cerr << argv[0] << ": Input directory must be specified via -d flag\n";
		return 1;
	}

	typedef vector<filesystem::directory_entry> vec; 
	vec v;                                

	copy(filesystem::directory_iterator(dir), filesystem::directory_iterator(), back_inserter(v));

	sort(v.begin(), v.end());				// sort, since directory iteration
											// is not ordered on some file systems

	//cout << "Process " << getpid() << ": Init the initial value." << endl;																	//DEBUT
	write_message(getpid(), 0, -1, -1, -1, -1);
	for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it)
	{
		pid_t pid_other = fork();
		//pid_t pid_other = getpid();																										//DEBUG
		pid_t pid_own = getpid();

		if (pid_other == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}else if (pid_other == 0) {
			//============== CHILD PROCESS ==============
			//cout << "Child: " << pid_own << endl;																							//DEBUG
			//cout << "   " << *it << '\n';																									//DEBUG
			int res_num_args = 0, res_num_args_coi = 0, res_num_args_coi_gr = 0, res_num_args_gr = 0;
			int res_exec_code = handleFile(*it, res_num_args, res_num_args_coi, res_num_args_coi_gr, res_num_args_gr);

			int exec_code, num_args, num_args_coi, num_args_coi_gr, num_args_gr;
			if (read_message(pid_own, exec_code, num_args, num_args_coi, num_args_coi_gr, num_args_gr) && exec_code == 0) {
				//if (read_message(0, exec_code, num_args, num_args_coi, num_args_coi_gr, num_args_gr) && exec_code == 0) {						//DEBUG
				write_message(pid_own, res_exec_code, res_num_args, res_num_args_coi, res_num_args_coi_gr, res_num_args_gr);
			}
			else if (read_message(pid_own, exec_code, num_args, num_args_coi, num_args_coi_gr, num_args_gr) && exec_code != 0) {
				cout << "Process " << pid_own << ": ERROR value was not reset." << endl;
			}

			//cout << "=========== End of process " << pid_own << endl;																		//DEBUG
			exit(EXIT_SUCCESS);
		}else {
			//============== PARENT PROCESS ==============
			//cout << "Parent: " << pid_own << endl;																							//DEBUT
			int status, exec_code, num_args, num_args_coi, num_args_coi_gr, num_args_gr;
			while (-1 == waitpid(pid_other, &status, 0));
			//cout << "waited until child process ended" << endl;
			if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
				cerr << "Process " << pid_other << " terminated" << endl;
				//count file for statistics
				decode(6);
			}else{
				if (read_message(pid_own, exec_code, num_args, num_args_coi, num_args_coi_gr, num_args_gr) && exec_code != 0) {
					//if (read_message(0, exec_code, num_args, num_args_coi, num_args_coi_gr, num_args_gr) && exec_code != 0)	{				//DEBUG
						//decode value received
					decode(exec_code);
					// update statistics
					if (num_args_coi > -1) {
						updateProcent(num_args_coi_base, num_args_coi_reducted_procent, num_args, num_args_coi);
					}
					if (num_args_gr > -1) {
						updateProcent(num_args_gr_base, num_args_gr_reducted_procent, num_args, num_args_gr);
					}
					if (num_args_coi_gr > -1) {
						updateProcent(num_args_coi_gr_base, num_args_coi_gr_reducted_procent, num_args - num_args_coi, num_args_coi_gr);
					}
					//reset value
					write_message(pid_own, 0, -1, -1, -1, -1);
				}
				else if (read_message(pid_own, exec_code, num_args, num_args_coi, num_args_coi_gr, num_args_gr) && exec_code == 0) {
					cout << "Process " << pid_own << ": ERROR value was not set." << endl;
				}
			}
		}
	}

	cout << endl;
	print_statistics();
	return 0;
}

