# Changelog

## v0.1.3 — Real regression evidence

- Recorded the real OSS CAD Suite verification results: 30/30 Verilator tests and 4,099/4,099 scoreboard checks passed.
- Recorded the Icarus smoke result: 32/32 checks passed.
- Recorded generic Yosys synthesis: 492 cells, 67 sequential cells, and zero structural problems.
- Added a focused GTKWave screenshot for the clear-versus-match corner case.
- Updated README and results documentation to distinguish measured RTL evidence from pending Sky130 physical-design results.

## v0.1.2 — Portable Verilator tracing

- Replaced the FST trace backend with VCD for the default Verilator build.
- Removed the external `lz4.h` build dependency exposed by OSS CAD Suite 2026-07-28.
- Updated the C++ simulation context, CLI help, scripts, README, and verification diagram.
- Retained Verilator assertions and coverage collection.

## v0.1.0 — Initial portfolio release

- APB4-compatible 32-bit programmable timer RTL
- Generated shared register definitions
- Self-checking Icarus smoke test
- C++17 object-oriented Verilator verification environment
- 29 directed tests and deterministic randomized regression
- Assertions, functional coverage, CI, Yosys, and OpenLane scaffolding
- Result collection that does not fabricate unrun metrics

## Validation update — 2026-07-29

- Removed reset and interrupt-sampling races from the Icarus smoke testbench.
- Added explicit Verilator coverage database generation and archival.
- Re-ran the complete C++ harness validation: 30/30 tests and 4,099/4,099 checks passed against an external behavioral stand-in.
- Documented that real Icarus, Verilator RTL, Yosys, and OpenLane execution remains pending because the packaging environment could not obtain the tool binaries.

## v0.1.1 — Real-tool regression compatibility fixes

- Made the Icarus smoke test sample `irq_o` on the falling edge after the expected match edge, avoiding an NBA-region scheduling race.
- Added an RTL `timescale` and removed unsupported `unique` simulation qualifiers to keep Icarus output clean.
- Added a targeted Verilator `SYNCASYNCNET` waiver for the intentional combination of asynchronous RTL reset and synchronously sampled SVA disable logic.
- Corrected Yosys invocation from Tcl mode (`-c`) to native Yosys-script mode (`-s`) and removed `yosys -import`.
- Added archival of the synthesized JSON netlist.
