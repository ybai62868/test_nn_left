#ifndef __TYPE_DEF_H_
#define __TYPE_DEF_H_

//#include <ap_int.h>
#include "config.h"

#define DATA_BITWIDTH 32
//#define DATA_BITWIDTH 8

#if (DATA_BITWIDTH == 32)
typedef int data_bitwidth;
#elif (DATA_BITWIDTH == 8)
typedef char data_bitwidth;
#else
// other bitwidths
//typedef half data_bitwidth;
#endif


//struct vec_input_t
//{
//	data_bitwidth data[IN_PACK_SIZE];
//};
//
//struct vec_output_t
//{
//	data_bitwidth data[OUT_PACK_SIZE];
//};


#endif
