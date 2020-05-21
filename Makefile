
SRC_DIR := $(shell pwd)

all:
	mkdir -p build && \
	pushd build && \
	cmake \
		-DCMAKE_BUILD_TYPE=Debug \
		-G'Unix Makefiles' \
		${SRC_DIR} && \
	cmake --build .
