
#include <ap_int.h>
#include <string.h>

#include "config.h"

#include "cnn_top.h"

#include "type_def.h"

#include "./conv2d/conv2d.cpp"

/*
void read_ifmap(data_bitwidth dst[][I], dram_trans_t *src, int size)
{
	dram_trans_t temp;

	for (int i = 0; i < (size / NUM_PACKED_DATA); i++)
	{
		temp = src[i];

		for (int j = 0; j < (NUM_PACKED_DATA / IN_PACK_SIZE); j++)
		{
#pragma HLS PIPELINE
			for (int i2 = 0; i2 < IN_PACK_SIZE; i2++)
			{
#pragma HLS UNROLL
				dst[i * (NUM_PACKED_DATA / IN_PACK_SIZE) + j][i2] = temp.range((j * IN_PACK_SIZE + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * IN_PACK_SIZE + i2) * DATA_BITWIDTH);
			//	dst[i * NUM_PACKED_DATA + j][i2] = temp((j * IN_PACK_SIZE + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * IN_PACK_SIZE + i2) * DATA_BITWIDTH);
			}
		}
	}

	for (int i2 = 0; i2 < IN_PACK_SIZE; i2++)
	{
		dst[0][i2] = src[0].range(DATA_BITWIDTH - 1, 0);
	}
}
*/

/*
void write_ofmap(dram_trans_t *dst, data_bitwidth src[][I], int size)
{
	dram_trans_t temp;

	for (int i = 0; i < (size / NUM_PACKED_DATA); i++)
	{
		for (int j = 0; j < (NUM_PACKED_DATA / IN_PACK_SIZE); j++)
		{
#pragma HLS PIPELINE
			for (int i2 = 0; i2 < IN_PACK_SIZE; i2++)
			{
#pragma HLS UNROLL
			//	temp((j * IN_PACK_SIZE + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * IN_PACK_SIZE + i2) * DATA_BITWIDTH) = src[i * (NUM_PACKED_DATA / IN_PACK_SIZE) + j][i2];
				temp.range((j * IN_PACK_SIZE + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * IN_PACK_SIZE + i2) * DATA_BITWIDTH) = src[i * (NUM_PACKED_DATA / IN_PACK_SIZE) + j][i2];
			}
		}

		dst[i] = temp;
	}

	for (int i2 = 0; i2 < IN_PACK_SIZE; i2++)
	{
		dst[0].range(DATA_BITWIDTH - 1, 0) = src[0][i2];
	}
}
*/

void cnn_top(dram_trans_t *ifmap,
		dram_trans_t *weights,
		dram_trans_t *ofmap)
{
#pragma HLS INTERFACE m_axi port=ifmap offset=slave bundle=ifmap
#pragma HLS INTERFACE m_axi port=weights offset=slave bundle=weights
#pragma HLS INTERFACE m_axi port=ofmap offset=slave bundle=ofmap
#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
#pragma HLS INTERFACE s_axilite port=weights bundle=control
#pragma HLS INTERFACE s_axilite port=ofmap bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

//#pragma HLS DATA_PACK variable=ifmap
//#pragma HLS DATA_PACK variable=weights
//#pragma HLS DATA_PACK variable=ofmap
//#pragma HLS DATA_PACK variable=cfg


	/*
	int	num_elem_input_fm;
//	int num_elem_weight;
	int	num_elem_output_fm;

	int wt_offset = 0;

	num_elem_input_fm = (cfg[0].bn_r * cfg[0].bn_c * cfg[0].bn_i) * (BS_R * R + cfg[0].k1 - 1) * (BS_C * C + cfg[0].k2 - 1) * BS_I * I;

	num_elem_output_fm = ((cfg[0].bn_r / cfg[0].pooling_stride) * (cfg[0].bn_c / cfg[0].pooling_stride) * cfg[0].bn_o) * (BS_R * R) * (BS_C * C) * BS_O * O;

//	num_elem_weight = cfg_BN_O[0] * cfg_BN_I[0] * O * cfg_BS_O[0] * cfg_k1[0] * cfg_k2[0] * cfg_BS_I[0] * I;
//	*/

	conv2d(ifmap, weights, ofmap);
}
