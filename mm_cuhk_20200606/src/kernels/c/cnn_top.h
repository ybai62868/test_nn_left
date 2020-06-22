
#ifndef __CNN_TOP_H_
#define __CNN_TOP_H_

#include "krnl_type_def.h"
#include "config.h"

extern "C" {

//	void cnn_top(dram_trans_t *ofmap);

	void cnn_top(dram_trans_t *ifmap,
			dram_trans_t *weights, 
			dram_trans_t *ofmap);
}

#endif
