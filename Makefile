build : build/hello_world.elf  build/CMakeFiles/Makefile2

clean:
	rm -rf build/

flash : build
	@# --force fails to reset the device to bootsel mode on the first try
	@picotool info --force > /dev/null ; sleep 0.5

	@picotool load build/hello_world.uf2

.PHONY: build/compile_commands.json
build/compile_commands.json : build/CMakeFiles/Makefile2
	cd build && compiledb make -j -n -k

build/CMakeFiles/Makefile2 : CMakeLists.txt
	mkdir -p build
	cd build && cmake .. -DPICO_BOARD=pico_w && cmake .. -DPICO_BOARD=pico_w 

build/hello_world.elf: main.cpp build/CMakeFiles/Makefile2
	cd build && make -j `nproc`