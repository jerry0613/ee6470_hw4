all : main.cpp
	riscv32-unknown-elf-g++ -std=c++14 testbench.cpp -o testbench -march=rv32ima -mabi=ilp32
	
sim: all
	$(EE6470)/riscv-vp/vp/build/bin/riscv-vp-acc --intercept-syscalls testbench
	
clean:
	rm -f main
