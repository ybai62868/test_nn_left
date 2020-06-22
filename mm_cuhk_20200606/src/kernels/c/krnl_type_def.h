#ifndef __KRNL_TYPE_DEF_H_
#define __KRNL_TYPE_DEF_H_

#include <ap_int.h>
//#include "systolic_params.h"
#include "type_def.h"

// FIXME: NUM_PACKED_DATA must be divided by IN_PACK_SIZE
//#define BW_BITWIDTH 512 
#define BW_BITWIDTH (IN_PACK_SIZE * DATA_BITWIDTH) 
#define NUM_PACKED_DATA (BW_BITWIDTH / DATA_BITWIDTH)

typedef ap_int<BW_BITWIDTH> dram_trans_t;

//typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_in_t;
//
//typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_ifmap_t;
//typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_filter_t;
//typedef ap_int<DATA_BITWIDTH> ap_pe_out_t;
////typedef ap_int<DATA_BITWIDTH * OUT_PACK_SIZE> ap_out_t;
//typedef ap_int<DATA_BITWIDTH * IN_PACK_SIZE> ap_out_t;

#endif
