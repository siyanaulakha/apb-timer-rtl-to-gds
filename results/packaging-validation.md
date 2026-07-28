# Validation Status

This repository has now been exercised with real open-source EDA tools on the user's Ubuntu workstation using OSS CAD Suite 2026-07-28.

## Real-tool gates passed in regression round 2

- Generated register files were current.
- Verilator lint completed successfully.
- Icarus Verilog smoke simulation passed all 32 checks and generated a VCD waveform.
- Yosys parsed, elaborated, optimized, technology-mapped, checked, and emitted Verilog/JSON netlists.
- Yosys `check` reported zero problems.
- Generic synthesis reported 492 cells and 813 wire bits.

## Verilator C++ regression status

The round-2 Verilator C++ build did not reach execution because the FST waveform backend attempted to include `lz4.h`, which was not available through the installed OSS CAD Suite compiler environment. No test or scoreboard failure occurred.

Version 0.1.2 replaces the default FST backend with portable VCD tracing (`--trace` plus `VerilatedVcdC`). A final real Verilator rerun is still required before claiming the 30-test RTL regression as complete.

## Earlier harness validation

Before real-tool execution, the C++ verification environment was also exercised against a temporary independent behavioral stand-in implementing the frozen timer semantics:

- 30/30 tests passed.
- 4,099 scoreboard checks passed.
- 500 deterministic randomized APB operations were included.

That stand-in run validates the harness structure but is not a substitute for the pending Verilator RTL run.

## Not yet claimed

- Completed Verilator RTL regression
- RTL/code coverage percentage
- Sky130 technology-mapped area
- Static timing closure
- OpenLane/OpenROAD completion
- DRC, LVS, antenna, or GDSII results
