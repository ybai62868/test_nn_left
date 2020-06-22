#ifndef __CONFIG_H_
#define __CONFIG_H_

/************configuration for testing************/
#define BN_R 4
#define BN_O 4
#define BN_I 16

#define BS_R 32
#define BS_O 8
#define BS_I 4

#define R 4
#define O 16
#define I 8

#define IN_PACK_SIZE I
#define OUT_PACK_SIZE O
/*************************************************/

#define NUM_CONV 1

//const unsigned int cfg_img_row[] =	{256,	256};
//const unsigned int cfg_in_num[] =	{256,	256};
//const unsigned int cfg_out_num[] =	{256,	256};

//typedef struct 
//{
//	int bn_r;
//	int bn_o;
//	int bn_i;
//	int padding; // padding for axi master bitwidth (must be power of 2)
//} OpCFG;

#endif
