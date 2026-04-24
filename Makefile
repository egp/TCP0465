SHELL := /bin/bash

CXX ?= c++

BUILD_DIR := build
HOST_BUILD_DIR := $(BUILD_DIR)/host

HOST_CPPFLAGS := \
	-Isrc \
	-Itests/host/fakes


HOST_CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -pedantic

# ------------------------------------------------------------------
# Sources
# ------------------------------------------------------------------
HOST_TEST_FILES := $(wildcard tests/host/test_*.cpp)

HOST_TEST_SUPPORT := $(sort \
	src/TCP0465.cpp \
	src/TCP0465Core.cpp \
	src/TCP0465Protocol.cpp \
)

# ------------------------------------------------------------------
# Targets
# ------------------------------------------------------------------
.PHONY: clean host-test test

host-test:
	@set -euo pipefail; \
	mkdir -p "$(HOST_BUILD_DIR)"; \
	for test_src in $(HOST_TEST_FILES); do \
		test_name="$$(basename "$$test_src" .cpp)"; \
		test_bin="$(HOST_BUILD_DIR)/$$test_name"; \
		echo "Building $$test_name"; \
		$(CXX) $(HOST_CXXFLAGS) $(HOST_CPPFLAGS) \
			$$test_src \
			$(HOST_TEST_SUPPORT) \
			-o $$test_bin; \
		$$test_bin; \
	done

test: host-test
compile-all: host-test

clean:
	rm -rf "$(BUILD_DIR)"