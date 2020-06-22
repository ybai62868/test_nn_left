
#include <stdlib.h>

#include "type_def.h"
//#include "systolic_params.h"
#include "config.h"

void randomize_array(data_bitwidth *array, const int size)
{
	for (int i = 0; i < size; ++i) 
	{
	//	array[i] = (data_bitwidth)rand() / (data_bitwidth)RAND_MAX;
		array[i] = (data_bitwidth)(rand() % 10);
	}
}

void one_array(data_bitwidth *array, const int size)
{
	for (int i = 0; i < size; ++i) 
	{
	//	array[i] = (data_bitwidth)rand() / (data_bitwidth)RAND_MAX;
		array[i] = 1;
	}
}

void idx_array(data_bitwidth *array, const int size)
{
	//	printf("%d %s\n", __LINE__, __func__);
	for (int i = 0; i < size; ++i) 
	{
		//      array[i] = 0;
		array[i] = 1 * i;
	}
}

int get_num_conv()
{
	int num_conv = 0;

//	for (int i = 0; i < BUILDING_BLOCK_NUM; i++)
//	{
//		num_conv += building_block_content[i];
//	}
//
	num_conv = NUM_CONV;

	return num_conv;
}
