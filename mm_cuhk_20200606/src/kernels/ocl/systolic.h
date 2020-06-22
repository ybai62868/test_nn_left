
#ifndef __CNN_H_
#define __CNN_H_

#define BN_R 1
#define BN_O 1
#define BN_I 1

#define BS_R 2
#define BS_O 2
#define BS_I 4

#define R 1
#define O 2
//#define I 4


#define SYS_ARRAY_NUM_ROWS R
#define SYS_ARRAY_NUM_COLS O

#define ACCUM_SHIFT_REG_SIZE (BS_R * BS_O)
#define O_OUT_SHIFT_REG_SIZE (BS_R * BS_O)

#define NUM_BLOCKS (BN_R * BN_O * BN_I)
#define TOTAL_NUM_BLOCKS (BN_R * BN_O + 1) * BN_I

typedef float data_bitwidth;

#endif
