
#ifndef __OPENCL_IF_H_
#define __OPENCL_IF_H_

#include "type_def.h"
#include "config.h"

#define NUM_QUEUES 1

// Check the status returned by the OpenCL API functions
#define CHECK(status) 								\
	if (status != CL_SUCCESS)						\
{									\
	fprintf(stderr, "error %d in line %d.\n", status, __LINE__);	\
	exit(1);							\
}									\

// Check the status returned by the OpenCL API functions, don't exit on error
#define CHECK_NO_EXIT(status) 								\
	if (status != CL_SUCCESS)						\
{									\
	fprintf(stderr, "error %d in line %d.\n", status, __LINE__);	\
}									\


void opencl_model(
       data_bitwidth *ifmap,
       data_bitwidth *weights,
       data_bitwidth *ofmap
	   );

void kernel_top(
		data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap
		) ;

#endif // __OPENCL_IF_H_
