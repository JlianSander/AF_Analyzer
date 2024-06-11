/*!
 * Copyright (c) <2024> <Lars Bengel & Julian Sander, University of Hagen>
 *
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MAIN_H
#define MAIN_H

#include <cstdio>
#include <iostream> 
#include <unistd.h> 
#include <sys/wait.h>
#include <stdio.h>
#include <stdint.h>
#include <filesystem>
#include <omp.h>
#include <getopt.h>			// parsing commandline options

extern "C" {
	#include "../include/util/MemoryWatchDog.h"
}

#include "../include/logic/AF.h"
#include "../include/logic/Parser_iccma.h"
#include "../include/logic/Solver_DS_PR.h"

#include "../include/logic/Enums.h"

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

constexpr auto PROGAMNAME = "AF_Analyzer";
constexpr auto VERSIONNUMBER = "1.0";
constexpr auto NUM_CORES = 0;
constexpr auto LIMIT_MEMORY_KB = 30000000;

static int version_flag = 0;
static int usage_flag = 0;
static int formats_flag = 0;
static int problems_flag = 0;

static int num_query_selfattack = 0;
static int num_query_no_attacker = 0;
static int num_query_grounded_contained = 0;
static int num_query_grounded_rejected = 0;
static int num_files_processed = 0;

const struct option longopts[] =
{
	{"help", no_argument, &usage_flag, 1},
	{"version", no_argument, &version_flag, 1},
	{"formats", no_argument, &formats_flag, 1},
	{"problems", no_argument, &problems_flag, 1},
	{"d", required_argument, 0, 'd'},
	{0}
};

/// <summary>
/// This method is used to start the program.
/// </summary>
/// <param name="argc">Number of arguments with which the program got started.</param>
/// <param name="argv">Array of strings, containing the different starting arguments of this progam.</param>
/// <returns>0 iff the program exited without error. 1 otherwise.</returns>
int main(int argc, char **argv);

bool check_memory_limit_crossed();
#endif
