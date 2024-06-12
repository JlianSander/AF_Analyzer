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

int calculateSolution(uint32_t query, AF &framework, const std::filesystem::path file, bool is_verbose)
{
	list<uint32_t> proof_extension;
	bool skept_accepted = false;
	int code_msg;
	skept_accepted = Solver_DS_PR::solve(query, framework, proof_extension, NUM_CORES, file, is_verbose, code_msg);
	cout << (skept_accepted ? "YES" : "NO") << endl;
	if (!skept_accepted)
	{
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
	return code_msg;
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

int static start_pre_processor(uint32_t query, AF &framework, const std::filesystem::path file) {
	VectorBitSet initial_reduct = VectorBitSet();
	int code_msg;
	pre_proc_result result_preProcessor = PreProc_DS_PR::process(framework, query, initial_reduct, file, true, code_msg);
	return code_msg;
}


/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

int handleFile(filesystem::directory_entry file) {

	//string file_path = file.path().stem().string();
	//string file_format = file_path.substr(file_path.find_last_of(".") + 1, file_path.length() - file_path.find_last_of(".") - 1);
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
	uint32_t query = read_query(file);
	return start_pre_processor(query, framework, file.path());
	//calculateSolution(query, framework);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void print_statistics() {
	cout << "Instances with query self-attacks: " << num_query_selfattack << "/" << num_files_processed << endl;
	cout << "Instances with unattacked queries: " << num_query_no_attacker << "/" << num_files_processed << endl;
	cout << "Instances which were part of grounded extension: " << num_query_grounded_contained << "/" << num_files_processed << endl;
	cout << "Instances which were rejected by grounded extension: " << num_query_grounded_rejected << "/" << num_files_processed << endl;
	cout << "Instances not solved during preprocessing: " << num_not_solved_preprocessor << "/" << num_files_processed << endl;
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

	cout << "Process " << getpid() << ": Init the initial value." << endl;
	write_message(getpid(), 0);

	for (vec::const_iterator it(v.begin()), it_end(v.end()); it != it_end; ++it)
	{
		pid_t pid_other = fork();
		pid_t pid_own = getpid();

		if (pid_other == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else if (pid_other != 0) {
			//============== PARENT PROCESS ==============
			cout << "Parent: " << pid_own << endl;
			int status;
			while (-1 == waitpid(pid_other, &status, 0));
			//cout << "waited until child process ended" << endl;
			if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
				cerr << "Child process (pid " << pid_other << ") failed" << endl;
			}

			int value;
			if (read_message(pid_own, value) && value != 0) {
				if (value > 0) {
					//file was processed
					//decode value received
					MessageDecoder::decode_message(value, num_query_selfattack, num_query_no_attacker,
						num_query_grounded_contained, num_query_grounded_rejected, num_not_solved_preprocessor);
					//count file since returned value was not 0
					num_files_processed++;
				}
				
				//reset value
				write_message(pid_own, 0);
			}
			else if (read_message(pid_own, value) && value == 0) {
				cout << "Process " << pid_own << ": ERROR value was not set." << endl;
			}
		}
		else {
			//============== CHILD PROCESS ==============
			cout << "Child: " << pid_own << endl;
			//cout << "   " << *it << '\n';
			int result = handleFile(*it);

			int value;
			if (read_message(pid_own, value) && value == 0) {
				write_message(pid_own, result);
			}
			else if (read_message(pid_own, value) && value != 0) {
				cout << "Process " << pid_own << ": ERROR value was not reset." << endl;
			}

			cout << "=========== End of process "<< pid_own << endl;
			exit(EXIT_SUCCESS);
		}
	}

	cout << endl;
	print_statistics();
	return 0;
}

