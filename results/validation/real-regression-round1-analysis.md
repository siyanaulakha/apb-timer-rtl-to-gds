# Real Regression Round 1 Analysis

Date: 2026-07-29

The first run on Ubuntu with OSS CAD Suite established that:

- Register generation passed.
- Standalone Verilator RTL lint passed.
- Icarus compiled and executed the RTL/testbench, but the smoke test sampled `irq_o` in a simulator-sensitive scheduling window.
- Verilator elaborated the RTL and assertions, then stopped because `SYNCASYNCNET` was treated as fatal. The warning is intentional: RTL reset is asynchronously asserted while assertions sample reset at the clock edge.
- Yosys parsed and elaborated `apb_timer.sv`, then failed because a native `.ys` command file was invoked as Tcl with `yosys -c`.

Corrections in v0.1.1:

1. Falling-edge IRQ sampling in the smoke test.
2. Targeted `--Wno-SYNCASYNCNET` for the Verilator regression only.
3. Native Yosys script execution with `yosys -s`.
4. RTL timescale and portable plain `case` statements.

A second real-tool regression is required before claiming RTL simulation or synthesis completion.
