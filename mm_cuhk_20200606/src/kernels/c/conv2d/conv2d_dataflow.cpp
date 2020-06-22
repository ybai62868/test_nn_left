
#include <hls_stream.h>
#include <ap_int.h>

#include "type_def.h"

static void load_ifmap(
		dram_trans_t *ifmap,
		int ifmap_blk_offset,
		data_bitwidth ifmap_buf[BS_R * BS_I][R][I])
{
//#pragma HLS ARRAY_PARTITION variable=test_buf complete dim=2

//	if (it < (BN_R * BN_O * BN_I))
	{
	int num_packed_data_in_tile = (BS_R * R * BS_I * I) / NUM_PACKED_DATA;

//	for (int r1_r2 = 0; r1_r2 < (R * BS_R); r1_r2++)
//	for (int i1 = 0; i1 < BS_I; i1++)
	int r1_o1 = 0;
	int r2 = 0;
	for (int i = 0; i < num_packed_data_in_tile; i++)
	{
#pragma HLS PIPELINE
		dram_trans_t temp = ifmap[(ifmap_blk_offset / I) + i];
		for (int j = 0; j < (NUM_PACKED_DATA / I); j++)
		{
#pragma HLS PIPELINE
			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
				ifmap_buf[r1_o1][r2][i2] = temp((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
			}

			r2++;
			if (r2 == R)
			{
				r2 = 0;
				r1_o1++;
				if (r1_o1 == (BS_R * BS_I))
				{
					r1_o1 = 0;
				}
			}
		}
	}
	}
}

static void load_wt(
		dram_trans_t *weight,
		int wt_blk_offset, 
		data_bitwidth wt_buf[BS_O * BS_I][O][I])
{
//	if (it < (BN_R * BN_O * BN_I))
	{
//	for (int o1 = 0; o1 < BS_O; o1++)
//	for (int o2 = 0; o2 < O; o1++)
//	for (int p = 0; p < cfg.k1; p++)
//	for (int q = 0; q < cfg.k2; q++)
//	for (int i1 = 0; i1 < BS_I; i1++)

	int num_packed_data_in_tile = (BS_O * O * BS_I * I) / NUM_PACKED_DATA;

	int o1_i1 = 0;
	int o2 = 0;
	for (int i = 0; i < num_packed_data_in_tile; i++)
	{
#pragma HLS PIPELINE
		dram_trans_t temp = weight[(wt_blk_offset / I) + i];

		for (int j = 0; j < (NUM_PACKED_DATA / I); j++)
		{
#pragma HLS PIPELINE
//#pragma HLS UNROLL
			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
			//	wt_buf[it][o2][i2] = temp.range((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
				wt_buf[o1_i1][o2][i2] = temp((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
			}

			o2++;
			if (o2 == O)
			{
				o2 = 0;
				o1_i1++;
			//	if (it == (BS_O * cfg.k1 * cfg.k2 * BS_I))
				if (o1_i1 == (BS_O * BS_I))
				{
					o1_i1 = 0;
				}
			}
		}
	}

	}
}

static void conv2d_calc_tile(
		data_bitwidth ifmap_buf[BS_R * BS_I][R][I], 
		data_bitwidth wt_buf[BS_O * BS_I][O][I], 
		data_bitwidth ofmap_buf[BS_R * BS_O][R][O], 
		int i0)
{
#pragma HLS inline off

//	if (it > 0 && it < (BN_R * BN_O * BN_I + 1))
	{
	data_bitwidth ofmap_reg[R * O];
#pragma HLS ARRAY_PARTITION variable=ofmap_reg complete

	for (int r1 = 0; r1 < BS_R; r1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int i1 = 0; i1 < BS_I; i1++)
	{
#pragma HLS DEPENDENCE variable=ofmap_buf inter false
#pragma HLS PIPELINE

		int i = i0 * BS_I + i1;

		for (int r2 = 0; r2 < R; r2++)
#pragma HLS UNROLL
		for (int o2 = 0; o2 < O; o2++)
		{
#pragma HLS UNROLL
		//	ofmap_reg[(r2 * C + c2) * o2] = ofmap_buf[(r1 * BS_C + c1) * BS_O + o1][r2][c2][o2];
			ofmap_reg[r2 * O + o2] = ((i == 0) ? 0 : ofmap_buf[r1 * BS_O + o1][r2][o2]);
		}

		for (int r2 = 0; r2 < R; r2++)
#pragma HLS UNROLL
		{
			for (int o2 = 0; o2 < O; o2++)
			{
#pragma HLS UNROLL
				data_bitwidth temp = 0;
				for (int i2 = 0; i2 < I; i2++)
				{
#pragma HLS UNROLL

				//	ofmap_buf[(r1 * BS_C + c1) * BS_O + o1][r2][c2][o2] +=
				//		ifmap_buf[i1][r1 * R + r2 + p][c1 * C + c2 + q][i2] * wt_buf[(r1 * BS_C + c1) * BS_O + o1][o2][i2];
					temp +=
						ifmap_buf[r1 * BS_I + i1][r2][i2] * wt_buf[o1 * BS_I + i1][o2][i2];
				}
				ofmap_reg[r2 * O + o2] += temp;
			}
		}

		for (int r2 = 0; r2 < R; r2++)
#pragma HLS UNROLL
		for (int o2 = 0; o2 < O; o2++)
		{
#pragma HLS UNROLL
			ofmap_buf[r1 * BS_O + o1][r2][o2] = ofmap_reg[r2 * O + o2];
		}
	}
	}
}

static void store_ofmap(
		dram_trans_t *ofmap, 
		int ofmap_blk_offset,
		data_bitwidth ofmap_buf[BS_R * BS_O][R][O], 
		int r0, 
		int i0)
{
//	if (it > 1)
	{
//	if (i0 == (cfg.BN_I - 1))
	if (i0 == (BN_I - 1))
	{
	/*
	 * FIXME: currently O must be divided by I
	 * */
	int num_packed_data_in_tile = (BS_R * BS_O * R * O) / NUM_PACKED_DATA;

//	for (int r1 = 0; r1 < BS_R; r1++)
//	for (int o1 = 0; o1 < BS_O; o1++)
//	for (int r2 = 0; r2 < R; r2++)
	int r1 = 0;
	int o1 = 0;
	int r2 = 0;
	int o2_i2 = 0;
	for (int i = 0; i < num_packed_data_in_tile; i++)
	{
#pragma HLS PIPELINE
		dram_trans_t temp;
		for (int j = 0; j < (NUM_PACKED_DATA / I); j++)
		{
#pragma HLS PIPELINE
			for (int i2 = 0; i2 < I; i2++)
			{
#pragma HLS UNROLL
				temp((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH) = ofmap_buf[r1 * BS_O + o1][r2][o2_i2 * I + i2];
			}


			o2_i2++;
			if (o2_i2 == (O / I))
			{
				o2_i2 = 0;
				r2++;
				if (r2 == R)
				{
					r2 = 0;
					o1++;
					if (o1 == BS_O)
					{
						o1 = 0;
						o1++;
						if (r1 == BS_R)
						{
							r1 = 0;
						}
					}
				}
			}
		}

		ofmap[ofmap_blk_offset / I + i] = temp;
	}
	}
	}
}

void conv2d(dram_trans_t *ifmap,
		dram_trans_t *weights,
		dram_trans_t *ofmap)
{
#pragma HLS INTERFACE m_axi port=ifmap offset=slave bundle=ifmap
#pragma HLS INTERFACE m_axi port=weights offset=slave bundle=weights
#pragma HLS INTERFACE m_axi port=ofmap offset=slave bundle=ofmap
//#pragma HLS INTERFACE s_axilite port=ifmap bundle=control
//#pragma HLS INTERFACE s_axilite port=weights bundle=control
//#pragma HLS INTERFACE s_axilite port=ofmap bundle=control

	data_bitwidth ifmap_buf0[BS_R * BS_I][R][I];
	data_bitwidth ifmap_buf1[BS_R * BS_I][R][I];

	data_bitwidth wt_buf0[BS_O * BS_I][O][I];
	data_bitwidth wt_buf1[BS_O * BS_I][O][I];

	data_bitwidth ofmap_buf0[BS_R * BS_O][R][O];
	data_bitwidth ofmap_buf1[BS_R * BS_O][R][O];

#pragma HLS ARRAY_PARTITION variable=ifmap_buf0 complete dim=2
#pragma HLS ARRAY_PARTITION variable=ifmap_buf0 complete dim=3
#pragma HLS ARRAY_PARTITION variable=ifmap_buf1 complete dim=2
#pragma HLS ARRAY_PARTITION variable=ifmap_buf1 complete dim=3

#pragma HLS ARRAY_PARTITION variable=wt_buf0 complete  dim=2
#pragma HLS ARRAY_PARTITION variable=wt_buf0 complete  dim=3
#pragma HLS ARRAY_PARTITION variable=wt_buf1 complete  dim=2
#pragma HLS ARRAY_PARTITION variable=wt_buf1 complete  dim=3

#pragma HLS ARRAY_PARTITION variable=ofmap_buf0 complete  dim=2
#pragma HLS ARRAY_PARTITION variable=ofmap_buf0 complete  dim=3
#pragma HLS ARRAY_PARTITION variable=ofmap_buf1 complete  dim=2
#pragma HLS ARRAY_PARTITION variable=ofmap_buf1 complete  dim=3

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		for (int i0 = 0; i0 < BN_I; i0++)
		{
			int ifmap_blk_offset = (r0 * BN_I + i0) * BS_R * R * BS_I * I;
			int wt_blk_offset = (o0 * BN_I + i0) * BS_O * O * BS_I * I;
			int ofmap_blk_offset = (r0 * BN_O + o0) * (BS_R * R * BS_O * O);

#pragma HLS dataflow

			load_ifmap(ifmap, ifmap_blk_offset, ifmap_buf0);

			load_wt(weights, wt_blk_offset, wt_buf0);

			conv2d_calc_tile(ifmap_buf0, wt_buf0, ofmap_buf0, i0);

			store_ofmap(ofmap, ofmap_blk_offset, ofmap_buf0, r0, i0);
		}
	}
}
