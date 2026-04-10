# Makefile v7
SHELL := /bin/bash

ARDUINO_CLI ?= arduino-cli

AVR_CORE ?= arduino:avr
RENESAS_CORE ?= arduino:renesas_uno

AVR_FQBN ?= arduino:avr:uno
RENESAS_FQBN ?= arduino:renesas_uno:unor4wifi
CI_FQBNS ?= $(AVR_FQBN) $(RENESAS_FQBN)

CXX ?= c++
HOST_CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -pedantic -Isrc

HOST_TEST_FILES := $(wildcard tests/host/*.cpp)
HOST_TEST_SUPPORT := src/TCP0465Protocol.cpp src/TCP0465Core.cpp

EXAMPLE_DIRS := $(sort $(dir $(wildcard examples/*/*.ino)))

.PHONY: help setup host-test test compile compile-all clean

help:
	@echo "Targets:"
	@echo "  make setup        - install/update Arduino CLI indexes and required cores"
	@echo "  make host-test    - build and run host-side tests"
	@echo "  make test         - alias for host-test"
	@echo "  make compile      - compile all examples for both supported boards"
	@echo "  make compile-all  - alias for compile"
	@echo "  make clean        - remove local build output"

setup:
	@set -euo pipefail; \
	$(ARDUINO_CLI) config init --overwrite >/dev/null 2>&1 || true; \
	$(ARDUINO_CLI) core update-index >/dev/null; \
	$(ARDUINO_CLI) core install $(AVR_CORE) >/dev/null; \
	$(ARDUINO_CLI) core install $(RENESAS_CORE) >/dev/null

host-test:
	@set -euo pipefail; \
	mkdir -p build/host; \
	for test_src in $(HOST_TEST_FILES); do \
	  test_name="$$(basename "$$test_src" .cpp)"; \
	  test_bin="build/host/$$test_name"; \
	  $(CXX) $(HOST_CXXFLAGS) "$$test_src" $(HOST_TEST_SUPPORT) -o "$$test_bin"; \
	  "$$test_bin"; \
	done

test: host-test

compile:
	@set -euo pipefail; \
	for fqbn in $(CI_FQBNS); do \
	  for sketch_dir in $(EXAMPLE_DIRS); do \
	    $(ARDUINO_CLI) compile \
	      --fqbn "$$fqbn" \
	      --library . \
	      "$$sketch_dir" >/dev/null; \
	  done; \
	done

compile-all: compile

clean:
	@rm -rf build
# Makefile v7