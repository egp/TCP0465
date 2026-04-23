# Makefile v8

SHELL := /bin/bash

ARDUINO_CLI ?= arduino-cli

AVR_CORE ?= arduino:avr
RENESAS_CORE ?= arduino:renesas_uno

AVR_UNO_FQBN ?= arduino:avr:uno
AVR_MEGA_FQBN ?= arduino:avr:mega
RENESAS_UNOR4WIFI_FQBN ?= arduino:renesas_uno:unor4wifi

# Keep defaults aligned with the current CI workflow.
CI_FQBNS ?= $(AVR_UNO_FQBN) $(AVR_MEGA_FQBN)

# Path to your cloned TCP1819 library repo.
TCP1819_DIR ?= ../TCP1819

CXX ?= c++
HOST_CPPFLAGS ?= -Isrc -Itests/host/fakes
HOST_CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -pedantic

BUILD_DIR := build
HOST_BUILD_DIR := $(BUILD_DIR)/host

HOST_TEST_FILES := $(wildcard tests/host/*.cpp)
HOST_TEST_SUPPORT := $(sort \
	src/TCP0465.cpp \
	src/TCP0465Core.cpp \
	src/TCP0465Protocol.cpp \
	$(wildcard tests/host/fakes/*.cpp) \
)

HOST_WRAPPER_FAKE := tests/host/fakes/TCP1819.h

EXAMPLE_DIRS := $(sort $(dir $(wildcard examples/*/*.ino)))

.PHONY: \
	help \
	setup \
	check-tcp1819-dir \
	host-test \
	host-wrapper-compile \
	test \
	compile \
	compile-all \
	ci \
	clean \
	print-config

help:
	@echo "Targets:"
	@echo "  make setup                 - install/update Arduino CLI indexes and required cores"
	@echo "  make host-test             - build and run current host-side tests"
	@echo "  make host-wrapper-compile  - compile-check src/TCP0465.cpp if tests/host/fakes/TCP1819.h exists"
	@echo "  make test                  - alias for host-test"
	@echo "  make compile               - compile all examples using this repo plus TCP1819_DIR"
	@echo "  make compile-all           - alias for compile"
	@echo "  make ci                    - run host-test and compile"
	@echo "  make clean                 - remove local build output"
	@echo "  make print-config          - print resolved configuration"
	@echo
	@echo "Variables:"
	@echo "  TCP1819_DIR=$(TCP1819_DIR)"
	@echo "  CI_FQBNS=$(CI_FQBNS)"

setup:
	@set -euo pipefail; \
	$(ARDUINO_CLI) config init --overwrite >/dev/null 2>&1 || true; \
	$(ARDUINO_CLI) core update-index >/dev/null; \
	$(ARDUINO_CLI) core install $(AVR_CORE) >/dev/null; \
	$(ARDUINO_CLI) core install $(RENESAS_CORE) >/dev/null

check-tcp1819-dir:
	@set -euo pipefail; \
	test -d "$(TCP1819_DIR)" || { \
		echo "ERROR: TCP1819_DIR does not exist: $(TCP1819_DIR)"; \
		echo "Set it explicitly, e.g.:"; \
		echo "  make compile TCP1819_DIR=../TCP1819"; \
		exit 1; \
	}; \
	test -f "$(TCP1819_DIR)/library.properties" || { \
		echo "ERROR: $(TCP1819_DIR) does not look like an Arduino library repo"; \
		echo "Missing: $(TCP1819_DIR)/library.properties"; \
		exit 1; \
	}

host-test:
	@set -euo pipefail; \
	mkdir -p "$(HOST_BUILD_DIR)"; \
	for test_src in $(HOST_TEST_FILES); do \
		test_name="$$(basename "$$test_src" .cpp)"; \
		test_bin="$(HOST_BUILD_DIR)/$$test_name"; \
		$(CXX) $(HOST_CXXFLAGS) $(HOST_CPPFLAGS) "$$test_src" $(HOST_TEST_SUPPORT) -o "$$test_bin"; \
		"$$test_bin"; \
	done
	@$(MAKE) --no-print-directory host-wrapper-compile

host-wrapper-compile:
	@set -euo pipefail; \
	mkdir -p "$(HOST_BUILD_DIR)"; \
	if [[ -f "$(HOST_WRAPPER_FAKE)" ]]; then \
		echo "Compiling src/TCP0465.cpp with host fakes"; \
		$(CXX) $(HOST_CXXFLAGS) $(HOST_CPPFLAGS) -c src/TCP0465.cpp -o "$(HOST_BUILD_DIR)/TCP0465.o"; \
	else \
		echo "Skipping wrapper compile-check; missing $(HOST_WRAPPER_FAKE)"; \
	fi

test: host-test

compile: check-tcp1819-dir
	@set -euo pipefail; \
	for fqbn in $(CI_FQBNS); do \
		for sketch_dir in $(EXAMPLE_DIRS); do \
			echo "Compiling $$sketch_dir for $$fqbn"; \
			$(ARDUINO_CLI) compile \
				--fqbn "$$fqbn" \
				--library . \
				--library "$(abspath $(TCP1819_DIR))" \
				"$$sketch_dir" >/dev/null; \
		done; \
	done

compile-all: compile

ci: host-test compile

clean:
	@rm -rf "$(BUILD_DIR)"

print-config:
	@echo "ARDUINO_CLI=$(ARDUINO_CLI)"
	@echo "TCP1819_DIR=$(TCP1819_DIR)"
	@echo "CI_FQBNS=$(CI_FQBNS)"
	@echo "HOST_TEST_FILES=$(HOST_TEST_FILES)"
	@echo "HOST_TEST_SUPPORT=$(HOST_TEST_SUPPORT)"
	@echo "EXAMPLE_DIRS=$(EXAMPLE_DIRS)"

# Makefile v8