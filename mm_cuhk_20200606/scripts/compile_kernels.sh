
source $SYSTOLIC_ROOT/scripts/setenv_sdaccel_2017_1.sh

echo "=== Cleaning up ==="
make -C $SYSTOLIC_ROOT/src/kernels/c clean
echo "=== Compiling host program ==="
make -C $SYSTOLIC_ROOT/src/kernels/c SDA_FLOW=$1
