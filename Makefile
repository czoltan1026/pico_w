SHELL := /bin/bash -o pipefail

build : build/hello_world.elf  build/CMakeFiles/Makefile2 server

clean:
	rm -rf build/

flash : build
	@# --force fails to reset the device to bootsel mode on the first try
	@picotool info --force > /dev/null ; sleep 1

	@picotool load --force build/hello_world.uf2 && picotool reboot --force


log : 
	(cat /dev/ttyACM0 | grep -a --line-buffered . | tee log.txt) || (sleep 0.5 ; $(MAKE) log)

flash_and_log :
	$(MAKE) flash && $(MAKE) log

	

.PHONY: build/compile_commands.json
build/compile_commands.json : build/CMakeFiles/Makefile2 server
	cd build && compiledb -f make  -n -k
	compiledb -o $@ make server -n -k

build/CMakeFiles/Makefile2 : CMakeLists.txt
	mkdir -p build
	cd build && cmake .. -DPICO_BOARD=pico_w && cmake .. -DPICO_BOARD=pico_w

build/hello_world.elf: main.cpp build/CMakeFiles/Makefile2 $(shell find src/)
	cd build && make -j `nproc`

server : server.cpp
	g++ -o $@ $<