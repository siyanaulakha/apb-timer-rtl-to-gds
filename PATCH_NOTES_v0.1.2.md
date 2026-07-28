# v0.1.2 Patch Notes

Regression round 2 passed register generation, Verilator lint, the 32-check Icarus smoke test, and generic Yosys synthesis. The Verilator C++ executable was blocked before execution because the FST trace backend required `lz4.h`, which was not visible in the installed OSS CAD Suite environment.

Version 0.1.2 changes the default Verilator waveform backend from FST to VCD:

- Verilator option: `--trace` instead of `--trace-fst`
- C++ trace type: `VerilatedVcdC` instead of `VerilatedFstC`
- Default trace file: `build/verilator/apb_timer.vcd`

Assertions, code coverage, functional coverage, deterministic seeds, and the complete test suite remain enabled.

## Apply and rerun

```bash
source "$HOME/tools/eda/oss-cad-suite/environment"
cd "$HOME/research/projects/apb-timer-rtl-to-gds"
make clean
make verify RANDOM_OPERATIONS=500 SEED=107
```

After the Verilator regression passes, run:

```bash
make coverage RANDOM_OPERATIONS=500 SEED=107
make results
```
