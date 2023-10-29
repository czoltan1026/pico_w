all : build/hello_world.elf build/compile_commands.json build/CMakeFiles/Makefile2


build/compile_commands.json : build/CMakeFiles/Makefile2
	cd build && compiledb make -j -n -k

build/CMakeFiles/Makefile2 : CMakeLists.txt
	mkdir -p build
	cd build && cmake .. -DPICO_BOARD=pico_w && cmake .. -DPICO_BOARD=pico_w 

build/hello_world.elf: main.cpp build/CMakeFiles/Makefile2
	cd build && make -j `nproc`