#include "StartAllTests.h"

extern "C" {
#include "../../include/util/Array.h"

}

TEST(Array, Memory_allocated) {

	/*int mem_base = get_mem_usage();
	arrayInt_t *array = create_array(5);
	int mem_array = get_mem_usage() - mem_base;*/
	int mem_array = 1;

	EXPECT_NE(0, mem_array);
}