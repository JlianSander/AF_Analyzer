#include "../../include/logic/Tests.h"
#include <assert.h>
#include <vector>

#include <limits.h>

using std::vector;

using namespace std;

void static runParallel(int numArgsStart, int numArgsEnd, AF &framework)
{
	double start, end;
	start = omp_get_wtime();
	for (uint32_t i = numArgsStart; i < numArgsEnd + 1; i++)
	{
		uint32_t argument = i;
		list<uint32_t> proof_extension;
		//printf("%d: ------- extension allocated --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());									//DEBUG

		printf("////////////////////////// ARGUMENT %d //////////////////////////////\n", i);
		bool isScepticAccepted = Solver_DS_PR::solve(argument, framework, proof_extension, NUM_CORES_TESTS);
		
		std::cout << std::boolalpha << (isScepticAccepted ? "YES" : "NO") << std::endl;
		if (!isScepticAccepted)
		{
			cout << "w ";
			if (proof_extension.empty())
			{
				if (framework.victims[argument]._bitset[argument])
				{
					cout <<"Argument attacks itself." << endl;
				}
				else
				{
					cout << "No Extension found" << endl;
				}
			}
			else
			{
				/*EXTENSIONSOLVER_CMS::BuildExtension(framework, actives, proof_extension);
				printf("Extension that proves rejection: ");
				print_list_uint32((*proof_extension));
				printf("\n");*/
				Printer::print_list(proof_extension);
				cout << endl;
			}
		}
	}
	end = omp_get_wtime();
	double run_time_para_rec = end - start;
	printf("Compute Time: %f seconds\n", run_time_para_rec);
}



/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void testSelfAttack()
{
	AF framework = AF(3);
	framework.add_attack(1, 1);
	framework.add_attack(2, 1);
	framework.add_attack(3, 2);
	framework.finish_initilization();

	runParallel(1, 1, framework);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void test4Args()
{
	AF framework = AF(4);
	framework.add_attack(1, 2);
	framework.add_attack(4, 1);
	framework.finish_initilization();

	runParallel(1, 1, framework);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void test6Args() 
{
	AF framework = AF(6);
	framework.add_attack(1, 2);
	framework.add_attack(1, 6);
	framework.add_attack(2, 4);
	framework.add_attack(2, 5);
	framework.add_attack(3, 1);
	framework.add_attack(3, 2);
	framework.add_attack(4, 1);
	framework.add_attack(5, 4);
	framework.finish_initilization();

	runParallel(1, 6, framework);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void test6ArgsFile()
{
	AF framework;
	ParserICCMA::parse_af(framework, "./../../../rsc/examples/example2.i23");
	runParallel(1, 6, framework);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void testArgsALot()
{
	int numArgs = 50;
	AF framework = AF(numArgs);
	for (int i = 1; i < numArgs + 1; i++)
	{
		if (i % 3)
		{
			framework.add_attack(i, ( i * numArgs / 2) %numArgs);
			framework.add_attack(i, (i * numArgs / 3) % numArgs);
		}
		else
		{
			framework.add_attack(i, (i + numArgs / 2) % numArgs);
			framework.add_attack(i, (i + numArgs / 3) % numArgs);
		}
	}
	framework.finish_initilization();

	runParallel(1, 1, framework);
}

/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/


/*===========================================================================================================================================================*/
/*===========================================================================================================================================================*/

static void testFile()
{
	//printf("%d: ------- program started --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());											//DEBUG

	double start, end, parse_af_time;
	start = omp_get_wtime();

	AF framework; 
	ParserICCMA::parse_af(framework, "/home/jsander/solvers/ICCMA23/benchmarks/main/g_1477_0.03_8.i23");

	end = omp_get_wtime();
	parse_af_time = end - start;
	printf("Time to parse + initialize AF: %f seconds\n", parse_af_time);

	//printf("%d: ------- framework initialized --- memory usage: %ld\n", omp_get_thread_num(), get_mem_usage());									//DEBUG														//DEBUG

	runParallel(671, 671, framework);
}

void TestCases::run_Tests()
{
	//testSelfAttack();
	//test4Args();
	//test6Args();
	//test6ArgsFile();
	//testArgsALot();
	testFile();
}
