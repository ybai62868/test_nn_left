
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "type_def.h"

//#include "systolic_params.h"
#include "config.h"

static void conv2d_calc_tile(data_bitwidth *ifmap_buf,
		data_bitwidth *wt_buf,
		data_bitwidth *ofmap_buf) 
{
//	memset(ofmap_buf, 0, BS_R * R * BS_O * O * sizeof(data_bitwidth));

	for (int r1 = 0; r1 < BS_R; r1++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int i1 = 0; i1 < BS_I; i1++)
	{
		for (int r2 = 0; r2 < R; r2++)
		for (int o2 = 0; o2 < O; o2++)
		for (int i2 = 0; i2 < I; i2++)
		{
			// (r0,c0,i0,r1*R+r2+p,c1*C+c2+q,i1,i2)
			int ifmap_idx = (r1 * R + r2) * BS_I * I + i1 * I + i2;
			// (o0,i0,o2,o1,p,q,i1,i2)
		//	int wt_idx = (o0 * cfg.BN_I + i0) * O * BS_O * cfg.k1 * cfg.k2 * BS_I * I + (((o1 * O + o2) * cfg.k1 + p) * cfg.k2 + q) * BS_I * I + i1 * I + i2;
			int wt_idx = (o1 * O + o2) * BS_I * I + i1 * I + i2;

			// (r0,c0,o0,r1,r2,c1,c2,o1,o2)
		//	int ofmap_idx = ((r0 * cfg.BN_C + c0) * cfg.BN_O + o0) * BS_R * R * BS_C * C * BS_O * O + ((r1 * R + r2) * BS_C * C + c1 * C + c2) * BS_O * O + o1 * O + o2;
			int ofmap_idx = (r1 * R + r2) * BS_O * O + o1 * O + o2;

	//		if (ifmap_idx > num_elem_input_fm || wt_idx > num_elem_weight || ofmap_idx > num_elem_output_fm)
	//		{
	//			printf("ifmap_idx=%d, wt_idx=%d, ofmap_idx=%d\n", ifmap_idx, wt_idx, ofmap_idx);
	//		}
		
		//	ofmap[ofmap_idx] += ifmap[ifmap_idx] * weight[wt_idx];
			ofmap_buf[ofmap_idx] += ifmap_buf[ifmap_idx] * wt_buf[wt_idx];

		//	printf("ofmap_buf[%d]+=ifmap_buf[%d]*wt_buf[%d], %d, %d, %d\n", ofmap_idx, ifmap_idx, wt_idx, ofmap_buf[ofmap_idx], ifmap_buf[ifmap_idx], wt_buf[wt_idx]);
		}
	}
}

static void store_ofmap(data_bitwidth *ofmap_buf,
		data_bitwidth *ofmap, 
		int r0, 
		int o0)
{
	for (int r21 = 0; r21 < (BS_R * R); r21++)
	for (int o21 = 0; o21 < (BS_O * O); o21++)
	{
		int ofmap_buf_idx = r21 * BS_O * O + o21;
		int ofmap_idx = (r0 * BN_O + o0) * (BS_R * R * BS_O * O) + ofmap_buf_idx;

		ofmap[ofmap_idx] = ofmap_buf[ofmap_buf_idx];
	}
}

static void init_ofmap_buf(data_bitwidth *ofmap_buf) 
{
	memset(ofmap_buf, 0, BS_R * R * BS_O * O * sizeof(data_bitwidth));
}

static void conv2d(data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap)
{


	data_bitwidth ifmap_buf[BS_R * R * BS_I * I];
	data_bitwidth wt_buf[BS_O * O * BS_I * I];
	data_bitwidth ofmap_buf[BS_R * R * BS_O * O];

//	int	num_elem_input_fm = cfg.BN_R * cfg.BN_C * cfg.BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I;
//	int num_elem_weight = cfg.BN_O * cfg.BN_I * O * BS_O * K1 * K2 * BS_I * I;
//	int	num_elem_output_fm = cfg.BN_R * cfg.BN_C * cfg.BN_O * R * BS_R * C * BS_C * O * BS_O;
//
//	printf("num_elem_input_fm=%d, num_elem_weight=%d, num_elem_output_fm=%d\n", num_elem_input_fm, num_elem_weight, num_elem_output_fm);
//	fflush(stdout);

	for (int r0 = 0; r0 < BN_R; r0++)
	for (int o0 = 0; o0 < BN_O; o0++)
	{
		init_ofmap_buf(ofmap_buf);

		for (int i0 = 0; i0 < BN_I; i0++)
		{
			memcpy(ifmap_buf, ifmap + (r0 * BN_I + i0) * (BS_R * R * BS_I * I), BS_R * R  * BS_I * I * sizeof(data_bitwidth));
			memcpy(wt_buf, weights + (o0 * BN_I + i0) * BS_O * O *  BS_I * I, BS_O * O * BS_I * I * sizeof(data_bitwidth));
			conv2d_calc_tile(ifmap_buf, wt_buf, ofmap_buf);
		}

		store_ofmap(ofmap_buf, ofmap, r0, o0);

	//	memcpy(ofmap + ((r0 * cfg.BN_C + c0) * cfg.BN_O + o0) * (BS_R * R * BS_C * C * BS_O * O / (cfg.pooling_stride * cfg.pooling_stride)), ofmap_buf, (BS_R * R * BS_C * C * BS_O * O / cfg.pooling_stride) * sizeof(data_bitwidth));
	//	memcpy(ofmap + (((r0 / cfg.pooling_stride) * (cfg.BN_C / cfg.pooling_stride) + c0) * cfg.BN_O + o0) * (BS_R * R * BS_C * C * BS_O * O), ofmap_buf, (BS_R * R * BS_C * C * BS_O * O) * sizeof(data_bitwidth));
	}
}

void cnn_golden(data_bitwidth *ifmap,
		data_bitwidth *weights,
		data_bitwidth *ofmap) 
{
	// read the feature map for the first layer
//	int	num_elem_input_fm = BN_R * BN_I * (R * BS_R + K1 - 1) * (C * BS_C + K2 - 1) * BS_I * I;

	conv2d(ifmap, weights, ofmap);

}
