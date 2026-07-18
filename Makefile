.PHONY: all config build clean run

all: build

config:
	cmake -B build -DCMAKE_BUILD_TYPE=Debug

build:
	@if [ ! -d "build" ]; then cmake -B build -DCMAKE_BUILD_TYPE=Debug; fi
	cmake --build build -j$(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)

clean:
	rm -rf build

run: build
	./build/gargantuan
