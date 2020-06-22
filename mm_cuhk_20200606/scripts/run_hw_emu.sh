
source $SYSTOLIC_ROOT/scripts/setenv_sdaccel_2017_1.sh

KERNEL_LANG=$1

HOST_EXE_DIR=$SYSTOLIC_ROOT/src/host/bin
HOST_EXE=cnn.out
HOST_ARGS=cnn_hw_emu.xclbin

cp $HOST_EXE_DIR/$HOST_EXE $SYSTOLIC_ROOT/src/kernels/$KERNEL_LANG
cd $SYSTOLIC_ROOT/src/kernels/$KERNEL_LANG
make xconfig
#XCL_EMULATION_MODE=true valgrind --tool=memcheck --leak-check=full --track-origins=yes ./$HOST_EXE $HOST_ARGS
XCL_EMULATION_MODE=true ./$HOST_EXE $HOST_ARGS
