# Makefile v5
SHELL := /bin/bash

ARDUINO_CLI ?= arduino-cli
ARDUINO_CORE ?= arduino:avr
DEFAULT_FQBN ?= arduino:avr:uno
CI_FQBNS ?= arduino:avr:uno arduino:avr:mega

CXX ?= c++
HOST_CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -pedantic -Isrc

EXAMPLES := $(wildcard examples/*/*.ino)
HOST_TEST_FILES := $(wildcard tests/host/*.cpp)
HOST_TEST_SUPPORT := src/TCP0465Protocol.cpp src/TCP0465Core.cpp
LIBRARY_ROOT := $(CURDIR)
CLI_SKETCH_ROOT := build/cli-sketches

.PHONY: help setup host-test test compile compile-all ci clean

help:
	@echo "Targets:"
	@echo "  make setup        - install/update Arduino CLI index and AVR core"
	@echo "  make host-test    - build and run host-side tests"
	@echo "  make test         - alias for host-test"
	@echo "  make compile      - compile all examples for DEFAULT_FQBN ($(DEFAULT_FQBN))"
	@echo "  make compile-all  - compile all examples for all CI_FQBNS"
	@echo "  make ci           - host-test + setup + compile-all"
	@echo "  make clean        - remove local build output"

setup:
	@set -euo pipefail; \
	$(ARDUINO_CLI) config init --overwrite >/dev/null 2>&1 || true; \
	$(ARDUINO_CLI) core update-index >/dev/null; \
	$(ARDUINO_CLI) core install $(ARDUINO_CORE) >/dev/null

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
	mkdir -p "$(CLI_SKETCH_ROOT)"; \
	for sketch in $(EXAMPLES); do \
	  sketch_name="$$(basename "$$sketch" .ino)"; \
	  temp_sketch_dir="$(CLI_SKETCH_ROOT)/$$sketch_name"; \
	  board_tag="$$(echo "$(DEFAULT_FQBN)" | tr ':/' '__')"; \
	  build_dir="build/$$sketch_name-$$board_tag"; \
	  rm -rf "$$temp_sketch_dir"; \
	  mkdir -p "$$temp_sketch_dir"; \
	  cp "$$sketch" "$$temp_sketch_dir/$$sketch_name.ino"; \
	  $(ARDUINO_CLI) compile \
	    --fqbn "$(DEFAULT_FQBN)" \
	    --library "$(LIBRARY_ROOT)" \
	    --warnings all \
	    --build-path "$$build_dir" \
	    "$$temp_sketch_dir" >/dev/null; \
	done

compile-all:
	@set -euo pipefail; \
	mkdir -p "$(CLI_SKETCH_ROOT)"; \
	for fqbn in $(CI_FQBNS); do \
	  for sketch in $(EXAMPLES); do \
	    sketch_name="$$(basename "$$sketch" .ino)"; \
	    temp_sketch_dir="$(CLI_SKETCH_ROOT)/$$sketch_name"; \
	    board_tag="$$(echo "$$fqbn" | tr ':/' '__')"; \
	    build_dir="build/$$sketch_name-$$board_tag"; \
	    rm -rf "$$temp_sketch_dir"; \
	    mkdir -p "$$temp_sketch_dir"; \
	    cp "$$sketch" "$$temp_sketch_dir/$$sketch_name.ino"; \
	    $(ARDUINO_CLI) compile \
	      --fqbn "$$fqbn" \
	      --library "$(LIBRARY_ROOT)" \
	      --warnings all \
	      --build-path "$$build_dir" \
	      "$$temp_sketch_dir" >/dev/null; \
	  done; \
	done

ci: host-test setup compile-all

clean:
	@rm -rf build
# Makefile v5