#include "../include/Main.h"

using namespace std;

void static print_usage()
{
	cout << "Usage: " << PROGAMNAME << " -d <directory> -t <timeout> -i <limitIterations> -c <csv-file> \n\n";
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

int calculate_solution(uint32_t query, AF &framework, VectorBitSet &initial_reduct, const std::filesystem::path file, int &out_is_solved, int &out_level, 
	int &out_iterations, bool is_verbose)
{
	list<uint32_t> proof_extension;
	bool skept_accepted = false;
	Solver_DS_PR::solve(query, framework, initial_reduct, proof_extension, file, is_verbose, 
		LIMIT_CALCULATION_LEVEL, out_is_solved, out_level, out_iterations, limit_iterations);

	//free allocated memory
	proof_extension.clear();

	if (out_level == 1 && out_iterations == 1) {

		if (is_verbose) {
			cout << file.filename() << "#### solved in one iteration" << endl;
		}
		return 7;
	}
	else if (out_level == 1 && out_iterations > 1 && out_iterations < limit_iterations) {
		if (is_verbose) {
			cout << file.filename() << "#### solved on level 1 in iteration " << out_iterations << endl;
		}
		return 8;
	}
	else if (out_iterations >= limit_iterations) {
		if (is_verbose) {
			cout << file.filename() << "#### reached limit of iterations" << endl;
		}
		return 5;
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

void write_csv_line(filesystem::directory_entry file, string csv_file_path, Observation obsv, bool is_timeout, bool is_terminated) {

	std::ofstream csv_file;
	csv_file.open(csv_file_path, std::ios_base::app);
	csv_file << file.path().filename() << "," 
		<< to_string(obsv.Executions_Code) << ","
		<< to_string(obsv.Is_Solved_PreProc) << ","
		<< (obsv.Number_Args != -1 ? to_string(obsv.Number_Args) : "" ) << ","
		<< (obsv.Number_Args_COI != -1 ? to_string(obsv.Number_Args_COI) : "") << ","
		<< (obsv.Number_Args_GR != -1 ? to_string(obsv.Number_Args_GR) : "") << ","
		<< (obsv.Number_Args_COI_GR != -1 ? to_string(obsv.Number_Args_COI_GR) : "") << ","
		<< to_string(obsv.Is_Solved) << ","
		<< (obsv.Iterations != -1 ? to_string(obsv.Iterations) : "") << ","
		<< (obsv.Level != -1 ? to_string(obsv.Level) : "") << ","
		<< to_string(is_timeout) << "," 
		<< to_string(is_terminated) <<"\n";
	csv_file.close();
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

Observation handle_file(filesystem::directory_entry file, string csv_file_path) {

	string file_format = file.path().extension();
	bool is_solved_preproc = false;

	if (file_format != ".i23") {
		//cerr << " Unsupported file format: " << file_format << endl;
		return Observation();
	}
	else {
		cout << endl;
		cout << "Processing: " << file.path().filename() << endl;
	}

	AF framework;
	ParserICCMA::parse_af(framework, file.path());
	uint32_t query = read_query(file);
	VectorBitSet initial_reduct_solver = VectorBitSet();
	Observation obsv = Observation();
	obsv.Number_Args = framework.num_args;
	obsv.Executions_Code = start_pre_processor(query, framework, file.path(), initial_reduct_solver, obsv.Number_Args_COI, obsv.Number_Args_COI_GR, obsv.Number_Args_GR);
	obsv.Is_Solved_PreProc = obsv.Executions_Code != 5;
	if (obsv.Executions_Code == 5) {
		//instance was not solved during preprocessing
		obsv.Executions_Code = calculate_solution(query, framework, initial_reduct_solver, file.path(), obsv.Is_Solved, obsv.Level, obsv.Iterations, true);
	}
	

	write_csv_line(file, csv_file_path, obsv, false, false);

	return obsv;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void child_handler(int sig)
{
	is_child_done = 1;
}

void alarm_handler(int sig)
{
	is_time_over = 1;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void print_statistics(Statistics &stats) {
	cout << "[PreProcessor]Instances with query self-attacks: " << stats.num_query_selfattack << "/" << stats.num_files_processed << endl;
	cout << "[PreProcessor]Instances with unattacked queries: " << stats.num_query_no_attacker << "/" << stats.num_files_processed << endl;
	cout << "[PreProcessor]Instances where query was part of grounded extension: " << stats.num_query_grounded_contained << "/" << stats.num_files_processed << endl;
	cout << "[PreProcessor]Instances where query was rejected by grounded extension: " << stats.num_query_grounded_rejected << "/" << stats.num_files_processed << endl;
	cout << "[PreProcessor]Instances not solved during preprocessing: " << stats.num_not_solved_preprocessor << "/" << stats.num_files_processed << endl;
	std::cout << std::setprecision(4);
	cout << "[PreProcessor]Average number of arguments reduced by cone of influence: " << stats.num_args_coi_reducted_procent << "/100" << endl;
	cout << "[PreProcessor]Average number of arguments reduced by grounded extension after calculating cone of influence: " 
		<< stats.num_args_coi_gr_reducted_procent << "/100" << endl;
	cout << "[PreProcessor]Average number of arguments reduced by grounded extension alone: " << stats.num_args_gr_reducted_procent << "/100" << endl;
	std::cout << std::setprecision(2);
	cout << "[Solver]Instances solved with 1st calculated set: " << stats.num_files_solved_fst_iteration << "/" << stats.num_not_solved_preprocessor << endl;
	cout << "[Solver]Instances solved at 1st level: " << stats.num_files_solved_fst_level << "/" << stats.num_not_solved_preprocessor << endl;
	std::cout << std::setprecision(4);
	cout << "[Solver]Average level of the calculated solution: " << stats.solve_lvl_avg << endl;
	cout << "[Solver]Average numbers of iterations per solution: " << stats.solve_iterations_avg << endl;
	std::cout << std::setprecision(2);
	cout << endl;
	cout << "Instances which were terminated: " << stats.num_files_terminated << "/" << stats.num_files_processed << endl;
	cout << "Instances which were stopped due to time out: " << stats.num_files_timeout << "/" << stats.num_files_processed << endl;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void decode(Statistics &stats, int msg_code)
{
	if (msg_code > 0) {
		//file was processed
		MessageDecoder::decode_message(stats, msg_code);
		//count file since returned value was > 0
		stats.num_files_processed++;
	}
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void static updateAverage(int &base, double &current_val, int new_base, double new_val, bool is_update_base) {
	current_val = (new_val * new_base) / (new_base + base)
		+ (current_val * base) / (new_base + base);
	if (is_update_base) {
		base += new_base;
	}
}

void static updateAverageProcent(int &base, double &cur_val_procent, int new_base, int new_val_absolut, bool is_update_base) {
	double new_val_procent;
	if (new_base != 0) {
		new_val_procent = (new_val_absolut * 100.0) / new_base ;
	}
	else {
		new_val_procent = 0;
	}
	
	updateAverage(base, cur_val_procent, new_base, new_val_procent, is_update_base);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void readResultFromChild(filesystem::directory_entry file, string csv_file_path, Statistics &stats, pid_t pid_own, pid_t pid_other)
{
	Observation obsv = Observation();

	if (read_message(pid_own, obsv)) {
		if (obsv.Executions_Code > 0) {
			//decode value received
			decode(stats, obsv.Executions_Code);
			// update statistics
			if (obsv.Number_Args_COI > -1) {
				updateAverageProcent(stats.num_args_coi_base, stats.num_args_coi_reducted_procent, obsv.Number_Args, obsv.Number_Args_COI, true);
			}
			if (obsv.Number_Args_GR > -1) {
				updateAverageProcent(stats.num_args_gr_base, stats.num_args_gr_reducted_procent, obsv.Number_Args, obsv.Number_Args_GR, true);
			}
			if (obsv.Number_Args_COI_GR > -1) {
				updateAverageProcent(
					stats.num_args_coi_gr_base, 
					stats.num_args_coi_gr_reducted_procent, 
					obsv.Number_Args - obsv.Number_Args_COI, 
					obsv.Number_Args_COI_GR, true);
			}
			if (obsv.Is_Solved == 1) {
				updateAverage(stats.num_files_solved, stats.solve_iterations_avg, 1, obsv.Iterations, false);
				updateAverage(stats.num_files_solved, stats.solve_lvl_avg, 1, obsv.Level, true);
			}
		}
	}else {
		//cout << "Process " << pid_own << ": ERROR value was not set." << endl;
		cerr << "Process " << pid_other << " terminated" << endl;
		//count file for statistics
		decode(stats, 6);
		write_csv_line(file, csv_file_path, obsv, false, true);
	}
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

void writeResultToParent(pid_t pid_own, Observation toWrite)
{
	Observation readObsv = Observation();
	if (read_message(pid_own, readObsv) && readObsv.Executions_Code == 0) {
		write_message(pid_own, toWrite);
	}
	else if (read_message(pid_own, readObsv) && readObsv.Executions_Code != 0) {
		cout << "Process " << pid_own << ": ERROR value was not reset." << endl;
	}
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
	string dir, csv_file_path;

	while ((opt = getopt_long_only(argc, argv, "", longopts, &option_index)) != -1) {
		switch (opt) {
		case 0:
			break;
		case 'd':
			dir = optarg;
			break;
		case 'c':
			csv_file_path = optarg;
			break;
		case 't':
			limit_timeout = std::stoi(optarg);
			break;
		case 'i':
			limit_iterations = std::stoi(optarg);
			break;
		default:
			return 1;
		}
	}

	if (csv_file_path.empty()) {
		csv_file_path = std::string(".") + std::filesystem::path::preferred_separator + std::string("Data_Tmp.csv");
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

	cout << "Process files in " << dir << endl;
	cout << "timeout: " << limit_timeout << endl;
	cout << "max. iterations: " << limit_iterations << endl;
	cout << "Save results in " << csv_file_path << endl;

	if (dir.empty()) {
		cerr << argv[0] << ": Input directory must be specified via -d flag\n";
		return 1;
	}
	//prepare csv-file
	std::ofstream csv_file;
	csv_file.open(csv_file_path);
	csv_file << "File,Execution_Code,Solved_Preprocessor,Number_Args,Args_reducted_COI, Args_reducted_GR,Args_reducted_COI_GR,Is_Solved,Num_Iterations,Level_Solution,Is_TimeOut,Is_Terminated,\n";
	csv_file.close();

	//prepare iterating through directory
	typedef vector<filesystem::directory_entry> vec; 
	vec v;                                

	copy(filesystem::directory_iterator(dir), filesystem::directory_iterator(), back_inserter(v));

	sort(v.begin(), v.end());				// sort, since directory iteration
											// is not ordered on some file systems

	//cout << "Process " << getpid() << ": Init the initial value." << endl;																	//DEBUT
	Observation initObsv = Observation();
	initObsv.Executions_Code = 0;
	write_message(getpid(), initObsv);
	Statistics stats;
	for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it)
	{
		pid_t pid_other = fork();
		pid_t pid_own = getpid();

		if (pid_other == -1) {
			perror("fork failed");
			exit(EXIT_FAILURE);
		}else if (pid_other == 0) {
			//============== CHILD PROCESS ==============
			//cout << "Child: " << pid_own << endl;			
			Observation obsv = handle_file(*it, csv_file_path);
			writeResultToParent(pid_own, obsv);

			//cout << "=========== End of process " << pid_own << endl;																			//DEBUG
			exit(EXIT_SUCCESS);
		}else {
			//============== PARENT PROCESS ==============
			//cout << "Parent: " << pid_own << endl;																							//DEBUT
			int status;

			signal(SIGALRM, alarm_handler);
			signal(SIGCHLD, child_handler);

			alarm(limit_timeout);  // install an alarm to be fired after limit_timeout
			pause();
			alarm(0);

			if (is_time_over) {
				cout << "TIME OUT" << endl;
				int result = waitpid(pid_other, NULL, WNOHANG);
				if (result == 0) {
					// child still running, so kill it
					kill(pid_other, 9);
					wait(NULL);
					//count file for statistics
					decode(stats, 9);
					Observation obsv = Observation();
					obsv.Executions_Code = 9;
					obsv.Is_Solved = false;
					obsv.Is_Solved_PreProc = false;
					write_csv_line(*it, csv_file_path, obsv, true, false);
				}
				else {
					printf("alarm triggered, but child finished normally\n");
					readResultFromChild(*it, csv_file_path, stats, pid_own, pid_other);
				}
			}else if(is_child_done) {
				wait(NULL);
				//cout << "waited until child process ended" << endl;
				readResultFromChild(*it, csv_file_path, stats, pid_own, pid_other);
			}

			is_time_over = 0;
			is_child_done = 0;
		}

		
		/*int res_num_args = -1, res_num_args_coi = -1, res_num_args_coi_gr = -1, res_num_args_gr = -1,
			res_is_solved = -1, res_solve_lvl = -1, res_solve_iterations = -1;

		int res_exec_code = handleFile(*it, res_num_args, res_num_args_coi, res_num_args_coi_gr, res_num_args_gr,
			res_is_solved, res_solve_lvl, res_solve_iterations);
		writeResultToParent(1, res_exec_code, res_num_args, res_num_args_coi, res_num_args_coi_gr, res_num_args_gr,
			res_is_solved, res_solve_lvl, res_solve_iterations);
		readResultFromChild(stats, 0, 1);*/

		//reset value
		write_message(pid_own, initObsv);
	}

	cout << endl;
	print_statistics(stats);
	return 0;
}

