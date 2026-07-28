SHELL := /usr/bin/env bash
.DEFAULT_GOAL := help

ROOT := $(CURDIR)
BUILD := $(ROOT)/build
SEED ?= 107
TEST ?=
RANDOM_OPERATIONS ?= 500
TRACE ?= 0

.PHONY: help generate check-generated lint smoke verify coverage synth openlane results all clean tool-versions

help:
	@printf '%s\n' \
	  'APB Timer RTL-to-GDSII project' \
	  '' \
	  'Targets:' \
	  '  make generate          Regenerate SV/C++/Markdown register definitions' \
	  '  make check-generated   Fail when generated files are stale' \
	  '  make lint              Run Verilator lint' \
	  '  make smoke             Run Icarus self-checking smoke test' \
	  '  make verify            Run C++/Verilator regression' \
	  '  make verify TEST=NAME  Run one named C++ test' \
	  '  make coverage          Run regression with coverage and waveform' \
	  '  make synth             Run Yosys synthesis' \
	  '  make openlane          Run OpenLane 2 flow' \
	  '  make results           Collect available metrics without inventing values' \
	  '  make all               Generate, lint, smoke, verify, synth' \
	  '  make clean             Remove generated build artifacts'

generate:
	@python3 scripts/generate_registers.py

check-generated:
	@bash scripts/check_generated_files.sh

lint: generate
	@bash scripts/run_lint.sh

smoke: generate
	@bash scripts/run_iverilog.sh

verify: generate
	@SEED='$(SEED)' TEST='$(TEST)' RANDOM_OPERATIONS='$(RANDOM_OPERATIONS)' TRACE='$(TRACE)' bash scripts/run_verilator.sh

coverage: generate
	@SEED='$(SEED)' RANDOM_OPERATIONS='$(RANDOM_OPERATIONS)' TRACE=1 bash scripts/run_verilator.sh

synth: generate
	@bash scripts/run_yosys.sh

openlane: generate
	@bash scripts/run_openlane.sh

results:
	@python3 scripts/collect_results.py

tool-versions:
	@bash scripts/report_tool_versions.sh

all: generate check-generated lint smoke verify synth results

clean:
	@rm -rf $(BUILD)
	@rm -f results/verification/functional-coverage.md
