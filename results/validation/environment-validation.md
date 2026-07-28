# Environment Validation Report

Date: 2026-07-29 (IST)

## Requested validation

Install Icarus Verilog, Verilator, and Yosys in the packaging environment and run the repository's real RTL simulation, C++/Verilator regression, and synthesis flow.

## Installation result

The packaging environment does not currently permit acquisition of the required EDA binaries:

- The configured Debian repositories cannot be reached because external DNS and outbound package access are blocked.
- The environment's internal package gateway did not provide the requested Verilator or YoWASP Yosys packages during this run.
- Direct connections to public package mirrors are blocked.
- No preinstalled Icarus Verilog, Verilator, or Yosys executables were found.

Therefore, this report does **not** claim a real Icarus, Verilator RTL, Yosys, OpenROAD, or OpenLane run.

## Validation completed locally

| Check | Result |
|---|---|
| Register generator execution | PASS |
| Generated-file freshness | PASS |
| Shell-script syntax (`bash -n`) | PASS |
| Python compilation (`py_compile`) | PASS |
| JSON parsing for register map and OpenLane configuration | PASS |
| C++17 strict compilation (`-Wall -Wextra -Wpedantic -Werror`) | PASS |
| Verification harness run against an external behavioral DUT stand-in | PASS |
| Directed tests | 29/29 PASS |
| Randomized test | PASS, 500 operations, seed 107 |
| Total tests | 30/30 PASS |
| Scoreboard checks | 4,099 PASS, 0 failures |

The behavioral stand-in was created outside the repository solely to validate the C++ transaction driver, reference model, scoreboard, coverage collector, test runner, and test sequencing. It is not a substitute for compiling and simulating `rtl/apb_timer.sv` with Verilator.

## Corrections made during this validation

### 1. Smoke-test scheduling race

`verification/rtl/apb_timer_smoke_tb.sv` previously deasserted reset and sampled the compare interrupt on clock edges without moving beyond the active/nonblocking-assignment scheduling regions. The testbench now:

- Deasserts `PRESETn` on the falling edge of `PCLK`.
- Adds a small post-edge sampling delay before checking the compare interrupt.

This removes simulator-dependent reset and IRQ sampling races.

### 2. Verilator coverage persistence

The project enabled Verilator coverage at build time but did not explicitly write the coverage database. The C++ runner now calls `VerilatedCov::write("coverage.dat")` when coverage is compiled in, and `scripts/run_verilator.sh` archives the resulting database under `results/verification/`.

## Real-tool gates still required

The following commands must pass in an environment with the tools installed before the associated claims are used on a résumé:

```bash
make lint
make smoke
make verify RANDOM_OPERATIONS=500 SEED=107
make synth
```

The OpenLane/OpenROAD physical-design flow remains a separate gate:

```bash
make openlane
make results
```

Until those commands execute successfully, synthesis area, timing slack, DRC, LVS, and GDSII completion remain unmeasured.
