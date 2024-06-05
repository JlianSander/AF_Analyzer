#ifndef PARSER_ICCMA
#define PARSER_ICCMA

#include <cstdint>
#include <iostream>			
#include <fstream>	
#include <filesystem>
#include <algorithm>
#include <sstream>

#include <omp.h>

#include "AF.h"

extern "C" {
	#include "../util/MemoryWatchDog.h"
}

using namespace std;

class ParserICCMA {
public:

	/// <summary>
	/// This method parses an abstract argumentation framework from a file in ICCMA'23 format located 
	/// at the specified path.
	/// </summary>
	/// <param name="framework"> Object of the framework to create.</param>
	/// <param name="file">String that describes the location of the file to open.</param>
	/// <returns>The abstract argumentation framework, which is describes by the file.</returns>
	static void parse_af(AF &framework, const std::filesystem::path filename);
};

#endif