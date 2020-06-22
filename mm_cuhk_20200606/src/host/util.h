
#ifndef __UTIL_H_
#define __UTIL_H_

//#include "systolic_params.h"
#include "type_def.h"

#define DEBUG_MSG_LEN 2

void randomize_array(data_bitwidth *array, const int size);
void one_array(data_bitwidth *array, const int size);
void idx_array(data_bitwidth *array, const int size);
int get_num_conv();

#endif
