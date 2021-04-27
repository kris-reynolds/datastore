
SRC_DIR := $(shell pwd)

BUILD_TYPE ?= Debug
BUILD_DIR := build/${BUILD_TYPE}

SETUP_FILE := ${BUILD_DIR}/InitialSetupComplete

.PHONY: setup-dev all-dev test clean shiny-clean

setup-dev:
	@mkdir -p ${BUILD_DIR}
	pushd ${BUILD_DIR} && \
	cmake \
		-DCMAKE_BUILD_TYPE=Debug \
		-DENABLE_EXAMPLES=ON \
		-DENABLE_TESTS=ON \
		-G'Unix Makefiles' \
		${SRC_DIR}
	@touch ${SETUP_FILE}

all-dev: ${SETUP_FILE}
	@pushd ${BUILD_DIR} && make

test: ${SETUP_FILE}
	@pushd ${BUILD_DIR} && ctest

clean:
	@pushd ${BUILD_DIR} && make clean

shiny-clean: clean
	@rm -rf ${BUILD_DIR}
