
KERNEL_LANG=c

run_hw: build_host build_hw
	source ./setenv_systolic.sh ; $(SYSTOLIC_ROOT)/scripts/run_hw.sh $(KERNEL_LANG) |& tee run_hw.log

build_hw:
	source ./setenv_systolic.sh ; $(SYSTOLIC_ROOT)/scripts/compile_kernels.sh hw |& tee build_hw.log;

run_cpu_emu: build_host build_cpu_emu
	source ./setenv_systolic.sh ; $(SYSTOLIC_ROOT)/scripts/run_cpu_emu.sh $(KERNEL_LANG) |& tee run_cpu_emu.log
	
build_cpu_emu:
	source ./setenv_systolic.sh; $(SYSTOLIC_ROOT)/scripts/compile_kernels.sh cpu_emu |& tee build_cpu_emu.log;

run_hw_emu: build_host build_hw_emu
	source ./setenv_systolic.sh ; $(SYSTOLIC_ROOT)/scripts/run_hw_emu.sh $(KERNEL_LANG) |& tee run_hw_emu.log
	
build_hw_emu:
	source ./setenv_systolic.sh; $(SYSTOLIC_ROOT)/scripts/compile_kernels.sh hw_emu |& tee build_hw_emu.log; 

build_host:
	source ./setenv_systolic.sh ; $(SYSTOLIC_ROOT)/scripts/compile_host.sh |& tee build_host.log ;

clean:
	rm -rf log
	make -C ./src/host clean
	make -C ./src/kernels/$(KERNEL_LANG) clean
