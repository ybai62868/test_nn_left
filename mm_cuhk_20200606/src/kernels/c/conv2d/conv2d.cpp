
#include <hls_stream.h>
#include <ap_int.h>

#include "type_def.h"

static void load_ifmap(int i0, 
		dram_trans_t *ifmap,
		int ifmap_blk_offset,
		data_bitwidth ifmap_buf[BS_R * BS_I][R][I])
{
//#pragma HLS ARRAY_PARTITION variable=test_buf complete dim=2

	if (i0 < BN_I)
	{
	int num_packed_data_in_tile = (BS_R * R * BS_I * I) / NUM_PACKED_DATA;

//	for (int r1_r2 = 0; r1_r2 < (R * BS_R); r1_r2++)
//	for (int i1 = 0; i1 < BS_I; i1++)
	int r1 = 0;
	int r2 = 0;
	int i1 = 0;
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
				ifmap_buf[r1 * BS_I + i1][r2][i2] = temp((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
			}

			i1++;
			if (i1 == BS_I)
			{
				i1 = 0;
				r2++;
				if (r2 == R)
				{
					r2 = 0;
					r1++;
					if (r1 == BS_R)
					{
						r1 = 0;
					}
				}
			}
		}
	}
	}
}

static void load_wt(int i0,
		dram_trans_t *weight,
		int wt_blk_offset, 
		data_bitwidth wt_buf[BS_O * BS_I][O][I])
{
	if (i0 < BN_I)
	{
//	for (int o1 = 0; o1 < BS_O; o1++)
//	for (int o2 = 0; o2 < O; o1++)
//	for (int p = 0; p < cfg.k1; p++)
//	for (int q = 0; q < cfg.k2; q++)
//	for (int i1 = 0; i1 < BS_I; i1++)

	int num_packed_data_in_tile = (BS_O * O * BS_I * I) / NUM_PACKED_DATA;

	int o1 = 0;
	int o2 = 0;
	int i1 = 0;
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
				wt_buf[o1 * BS_I + i1][o2][i2] = temp((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH);
			}

			i1++;
			if (i1 == BS_I)
			{
				i1 = 0;
				o2++;
				if (o2 == O)
				{
					o2 = 0;
					o1++;
					if (o1 == BS_O)
					{
						o1 = 0;
					}
				}
			}
		}
	}

	}
}

static void conv2d_calc_tile(int i0,
		data_bitwidth ifmap_buf[BS_R * BS_I][R][I], 
		data_bitwidth wt_buf[BS_O * BS_I][O][I], 
		data_bitwidth ofmap_buf[BS_R * BS_O][R][O])
{
#pragma HLS inline off

	if (i0 > 0)
	{
	data_bitwidth ofmap_reg[R * O];
#pragma HLS ARRAY_PARTITION variable=ofmap_reg complete

	for (int i1 = 0; i1 < BS_I; i1++)
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
//#pragma HLS PIPELINE
//	for (int i1 = 0; i1 < BS_I; i1++)
	{
#pragma HLS PIPELINE
//#pragma HLS DEPENDENCE variable=ofmap_buf inter false

		for (int r2 = 0; r2 < R; r2++)
#pragma HLS UNROLL
		for (int o2 = 0; o2 < O; o2++)
		{
#pragma HLS UNROLL
			ofmap_reg[r2 * O + o2] = ((i0 == 1 && i1 == 0) ? 0 : ofmap_buf[r1 * BS_O + o1][r2][o2]);
		//	if (i0 == 1 && i1 == 0)
		//	{
		//		ofmap_buf[r1 * BS_O + o1][r2][o2] = 0;
		//	}
		}

		for (int r2 = 0; r2 < R; r2++)
		{
#pragma HLS UNROLL
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
			//		ofmap_buf[r1 * BS_O + o1][r2][o2] +=
			//			ifmap_buf[r1 * BS_I + i1][r2][i2] * wt_buf[o1 * BS_I + i1][o2][i2];
				//	ofmap_buf[r1 * BS_O + o1][r2][o2] +=
				//		1 * 1;
				//	ofmap_buf[r1 * BS_O + o1][r2][o2] = 10;
				}
				ofmap_reg[r2 * O + o2] += temp;
			//	ofmap_buf[r1 * BS_O + o1][r2][o2] += temp;
			}
		}

		for (int r2 = 0; r2 < R; r2++)
		{
#pragma HLS UNROLL
		for (int o2 = 0; o2 < O; o2++)
		{
#pragma HLS UNROLL
			ofmap_buf[r1 * BS_O + o1][r2][o2] = ofmap_reg[r2 * O + o2];
		//	ofmap_buf[r1 * BS_O + o1][r2][o2] = 10;
		}
		}
	}
	}
	}
}

void init_ofmap(data_bitwidth ofmap_buf[BS_R * BS_O][R][O])
{
	for (int r1 = 0; r1 < BS_R; r1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	{
#pragma HLS PIPELINE
		for (int r2 = 0; r2 < R; r2++)
		{
#pragma HLS UNROLL
		for (int o2 = 0; o2 < O; o2++)
		{
#pragma HLS UNROLL
		//	ofmap_reg[r2 * O + o2] = ((i0 == 1 && i1 == 0) ? 0 : ofmap_buf[r1 * BS_O + o1][r2][o2]);
			ofmap_buf[r1 * BS_O + o1][r2][o2] = 0;
		}
		}
	}
}

static void store_ofmap(
		dram_trans_t *ofmap, 
		int ofmap_blk_offset,
		data_bitwidth ofmap_buf[BS_R * BS_O][R][O]) 
{
//	if (i0 == (BN_I - 1))
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
			//	temp((j * I + i2) * DATA_BITWIDTH + DATA_BITWIDTH - 1, (j * I + i2) * DATA_BITWIDTH) = 8;
			}


			o2_i2++;
			if (o2_i2 == (O / I))
			{
				o2_i2 = 0;
				o1++;
				if (o1 == BS_O)
				{
					o1 = 0;
					r2++;
					if (r2 == R)
					{
						r2 = 0;
						r1++;
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

void conv2d(dram_trans_t *ifmap,
		dram_trans_t *weights,
		dram_trans_t *ofmap)
{

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
//#pragma HLS ARRAY_PARTITION variable=ofmap_buf1 complete  dim=2
//#pragma HLS ARRAY_PARTITION variable=ofmap_buf1 complete  dim=3

//#pragma HLS RESOURCE variable=wt_buf0 core=RAM_2P_BRAM


	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		bool ping_pong_flag = 1;

		int ofmap_blk_offset = (r0 * BN_O + o0) * (BS_R * R * BS_O * O);

	//	init_ofmap(ofmap_buf0);

		for (int i0 = 0; i0 < (BN_I + 1); i0++)
		{
			int ifmap_blk_offset = (r0 * BN_I + i0) * BS_R * R * BS_I * I;
			int wt_blk_offset = (o0 * BN_I + i0) * BS_O * O * BS_I * I;

			if (ping_pong_flag == 1)
			{
				load_ifmap(i0, ifmap, ifmap_blk_offset, ifmap_buf0);

				load_wt(i0, weights, wt_blk_offset, wt_buf0);

				conv2d_calc_tile(i0, ifmap_buf1, wt_buf1, ofmap_buf0);
			}
			else
			{
				load_ifmap(i0, ifmap, ifmap_blk_offset, ifmap_buf1);

				load_wt(i0, weights, wt_blk_offset, wt_buf1);

				conv2d_calc_tile(i0, ifmap_buf0, wt_buf0, ofmap_buf0);
			}

			ping_pong_flag = 1 - ping_pong_flag;
		}

		store_ofmap(ofmap, ofmap_blk_offset, ofmap_buf0);
	}
}
