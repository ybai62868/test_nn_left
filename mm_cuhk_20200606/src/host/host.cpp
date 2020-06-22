#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef VIVADO_HLS
#include <CL/opencl.h>
#endif

#include <iostream>


#ifndef VIVADO_HLS
#include "opencl_if.h"
#else
#include "cnn_top.h"
#endif

#include "cnn.h"
#include "util.h"

#include "config.h"
#include "type_def.h"


#ifndef VIVADO_HLS
char *xclbin_filename;
#endif

////////////////////////////////////////////////////////////////////////////////

//data_bitwidth ifmap[BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I];
//data_bitwidth weight[BN_O * BN_I * O * BS_O * K1 * K2 * BS_I * I];
//data_bitwidth ofmap_opencl[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];
//
//data_bitwidth ofmap_sw[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];
//data_bitwidth ofmap_c[BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O];

data_bitwidth *ifmap;
data_bitwidth *weights;
data_bitwidth *ofmap_opencl;

data_bitwidth *ofmap_golden;
data_bitwidth *ofmap_c;

#ifdef VIVADO_HLS
//dram_trans_t *ifmap_hls;
//dram_trans_t *weights_hls;
//dram_trans_t *ofmap_hls;
data_bitwidth *ofmap_hls;
#endif


////////////////////////////////////////
// Data preparation for loader ifmap //
////////////////////////////////////////
void load_ifmap_prepare()
{
	int	num_elem_input_fm = BN_R * BN_I * R * BS_R * BS_I * I;

	if ((ifmap = (data_bitwidth *)malloc(num_elem_input_fm * sizeof(data_bitwidth))) == NULL) 
	{
		perror("Failed malloc of ifmap");
	}
//	one_array(ifmap, num_elem_input_fm);
//	idx_array(ifmap, num_elem_input_fm);
	randomize_array(ifmap, num_elem_input_fm);

#if 0
#ifdef VIVADO_HLS
	if ((ifmap_hls = (dram_trans_t *)malloc(num_elem_input_fm * sizeof(data_bitwidth))) == NULL) 
	{
		perror("Failed malloc of ifmap_hls");
	}

	// TODO: prepare data for ifmap_hls
#endif
#endif
}

////////////////////////////////////////
// Data preparation for loader weights //
////////////////////////////////////////
void load_wt_prepare()
{
	int num_elem_weights = 0;

	int num_conv = get_num_conv();

	for (int i = 0; i < num_conv; i++) 
	{
		int num_elem_weight = BN_O * BN_I * O * BS_O * BS_I * I;

		num_elem_weights += num_elem_weight;
	}

	if ((weights = (data_bitwidth *)malloc(num_elem_weights * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of weights");
	}

#if 0
#ifdef VIVADO_HLS
	if ((weights_hls = (dram_trans_t *)malloc(num_elem_weights * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of weights_hls");
	}
#endif
#endif

	data_bitwidth *weight = weights;

	for (int i = 0; i < num_conv; i++)
	{
		int num_elem_weight = BN_O * BN_I * O * BS_O * BS_I * I;

	//	one_array(weight, num_elem_weight);
	//	idx_array(weight, num_elem_weight);
		randomize_array(weight, num_elem_weight);

		weight += num_elem_weight;
	}
}

////////////////////////////////////////
// Data preparation for drain ofmap //
////////////////////////////////////////
void drain_ofmap_prepare()
{
	int	num_elem_output_fm = BN_R * BN_O * R * BS_R * O * BS_O;

	if ((ofmap_golden = (data_bitwidth *)malloc(num_elem_output_fm * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of ofmap_golden");
	}
//	memset(output_fm_block_wise, 0, num_elem_output_fm*sizeof(float));

	if ((ofmap_c = (data_bitwidth *)malloc(num_elem_output_fm * sizeof(data_bitwidth))) == NULL) 
	{
		perror("Failed malloc of ofmap_c");
	}
//	memset(output_fm, 0, num_elem_output_fm*sizeof(float));

//	if((ofmap_opencl = (data_bitwidth *)malloc(num_elem_output_fm * sizeof(data_bitwidth))) == NULL)
//	{
//		perror("Failed malloc of ofmap_opencl");
//	}
	ofmap_opencl = (data_bitwidth *)calloc(num_elem_output_fm, sizeof(data_bitwidth));
	if (ofmap_opencl == NULL)
	{
		perror("Failed malloc of ofmap_opencl");
	}

#ifdef VIVADO_HLS
//	if((ofmap_hls = (dram_trans_t *)malloc(num_elem_output_fm * sizeof(data_bitwidth))) == NULL)
	if((ofmap_hls = (data_bitwidth *)malloc(num_elem_output_fm * sizeof(data_bitwidth))) == NULL)
	{
		perror("Failed malloc of ofmap_hls");
	}
#endif
//	memset(output_fm_opencl, 0, num_elem_output_fm*sizeof(float));

}

void run_model(void (*cnn)(data_bitwidth *, data_bitwidth *, data_bitwidth *),
		data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap)
{
//	int	num_elem_output_fm = cfg_BN_R[NUM_CONV - 1] * cfg_BN_O[NUM_CONV - 1] * R * cfg_BS_R[NUM_CONV - 1] * C * cfg_BS_C[NUM_CONV - 1] * O * cfg_BS_O[NUM_CONV - 1];

	cnn(ifmap, weights, ofmap);
}

#ifdef VIVADO_HLS
void run_hls_model(
	//	dram_trans_t *ifmap,
	//	dram_trans_t *weights,
	//	dram_trans_t *ofmap, 
		data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap) 
{

	cnn_top((dram_trans_t *)ifmap,
			(dram_trans_t *)weights,
			(dram_trans_t *)ofmap);
//	cnn_top(ifmap, weights, ofmap);
}
#endif

int model_verify(data_bitwidth *ofmap_golden, data_bitwidth *ofmap_test)
{
	int match = 1;

	int	num_elem_output_fm = BN_R * BN_O * R * BS_R * O * BS_O;

	for (int i = 0; i < num_elem_output_fm; i++)
	{
#if (DATA_BITWIDTH == 32)
		if (ofmap_test[i] != ofmap_golden[i])
#else
			if (ofmap_test[i] != ofmap_golden[i])
#endif
			{
				//	std::cout << "Error: Result mismatch" << std::endl;
#if (DATA_BITWIDTH == 32)
				printf("ofmap_golden[%d]=%d. ofmap_test[%d]=%d\n", i, ofmap_golden[i], i, ofmap_test[i]);
#else
				printf("ofmap_golden[%d]=%d. ofmap_test[%d]=%d\n", i, ofmap_golden[i], i, ofmap_test[i]);
#endif
				match = 0;
				break;
			}
	}

	return match;
}

void cleanup()
{
	free(ifmap);
	free(weights);
	free(ofmap_golden);
	free(ofmap_c);
	free(ofmap_opencl);

#ifdef VIVADO_HLS
	free(ofmap_hls);
#endif
}

int main(int argc, char **argv)
{
//	int ifmap_length = BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I;
//	int wt_length = BN_O * BN_I * O * BS_O * K1 * K2 * BS_I * I;
//	int ofmap_length = BN_R * BN_O * R * BS_R * C * BS_C * O * BS_O;

#ifndef VIVADO_HLS
	if (argc != 2)
	{
		printf("%s <inputfile>\n", argv[0]);
		return EXIT_FAILURE;
	}

	xclbin_filename = argv[1];
#endif

	int match;

	// Fill our data sets with pattern
	//
	load_ifmap_prepare();
	load_wt_prepare();
	drain_ofmap_prepare();

	run_model(cnn_golden, ifmap, weights, ofmap_golden);

//	run_model(c_model, ifmap, weights, ofmap_c, cfg);
//	match = model_verify(ofmap_golden, ofmap_c);
//	std::cout << "C Model TEST: " << (match ? "PASSED" : "FAILED") << std::endl; 

#ifdef VIVADO_HLS
	run_hls_model(ifmap, weights, ofmap_hls);
	match = model_verify(ofmap_golden, ofmap_hls);
	std::cout << "HLS Model TEST: " << (match ? "PASSED" : "FAILED") << std::endl; 
#else
	run_model(opencl_model, ifmap, weights, ofmap_opencl);
	match = model_verify(ofmap_golden, ofmap_opencl);
	std::cout << "OpenCL Model TEST: " << (match ? "PASSED" : "FAILED") << std::endl; 
#endif

	cleanup();

	return 0;
}
