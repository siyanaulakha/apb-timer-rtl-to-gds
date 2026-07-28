# Real regression round 2 analysis

Date: 2026-07-29

## Passed gates

- Generated register files were current.
- Verilator lint passed.
- Icarus smoke regression passed all 32 checks.
- Yosys generic synthesis completed and `check` reported zero problems.
- Yosys emitted synthesized Verilog and JSON netlists.

## Remaining round-2 blocker

The C++ Verilator executable failed to compile only because the FST tracing backend included `lz4.h`, which was not exposed by the installed OSS CAD Suite environment. This is a packaging/dependency issue, not an RTL or test failure.

## v0.1.2 resolution

The default waveform backend is now VCD (`--trace` and `VerilatedVcdC`), which avoids the external LZ4 header while preserving optional waveform capture. Assertions and coverage remain enabled.

## Yosys generic synthesis snapshot

- 492 generic cells
- 67 flip-flop cells (34 + 32 enabled async-reset flops and 1 async-reset flop)
- 231 AND cells
- 107 OR cells
- 33 XOR cells
- 21 XNOR cells
- 29 NOT cells
- 4 MUX cells

These are generic technology-independent counts. They are not Sky130 mapped area, timing, or power results and must not be used as final ASIC metrics.
