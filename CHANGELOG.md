# Changelog

## v1.0.1 — Green CI and frozen Sky130 evidence

- Pinned CI to Verilator 5.050 and disabled inaccessible container `ccache` state.
- Replaced unsupported concurrent SVA constructs with portable clocked assertions.
- Published the 100 MHz Sky130 signoff metrics and exact constraint policy.
- Added zero-violation timing, DRC, LVS, antenna, slew, and fanout evidence.
- Added quantified area, utilization, standard-cell, wire-length, and estimated-power results.

## v1.0.0 — Sky130 RTL-to-GDSII completion

- Completed the OpenLane 2 Classic flow for `sky130A` / `sky130_fd_sc_hd`.
- Closed multi-corner setup and hold timing at a 10.0 ns signoff period.
- Completed route, Magic, KLayout, LVS, and antenna checks.

## v0.1.3 — Real regression evidence

- Recorded 30/30 Verilator tests and 4,099/4,099 scoreboard checks.
- Recorded the Icarus smoke result: 32/32 checks passed.
- Recorded generic Yosys synthesis: 492 cells, 67 sequential cells, and zero structural problems.
- Added a focused GTKWave screenshot for the clear-versus-match corner case.

## v0.1.2 — Portable Verilator tracing

- Replaced the FST trace backend with VCD.
- Removed the external `lz4.h` build dependency.
- Retained Verilator assertions and coverage collection.

## v0.1.1 — Real-tool compatibility fixes

- Removed reset and interrupt-sampling races from the Icarus smoke test.
- Corrected the Yosys invocation and archived synthesized Verilog/JSON netlists.
- Added Verilator coverage archival and targeted warning handling.

## v0.1.0 — Initial portfolio release

- Added the APB4 timer RTL, generated register definitions, Icarus smoke test, C++17 Verilator environment, assertions, semantic coverage, generic Yosys synthesis, OpenLane scaffolding, documentation, and CI.
