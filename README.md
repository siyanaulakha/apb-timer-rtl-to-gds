# APB4 Programmable Timer: C++ Verification to GDSII

A compact SoC-quality timer peripheral designed in synthesizable SystemVerilog, verified with a reusable C++17/Verilator environment, synthesized with Yosys, and prepared for an OpenLane/OpenROAD RTL-to-GDSII flow.

The project is intentionally small enough to audit in an interview while still exercising real hardware-engineering work: protocol timing, MMIO register design, partial writes, event ordering, sticky interrupts, reference modelling, scoreboarding, assertions, randomized verification, synthesis constraints, and physical-design report collection.

> **Status:** RTL verification and generic synthesis are complete on OSS CAD Suite: Verilator lint passed, the Icarus smoke test passed 32/32 checks, the C++/Verilator regression passed 30/30 tests with 4,099 scoreboard checks and zero failures, and Yosys reported zero structural problems. Sky130 physical implementation, timing, area, DRC, and LVS remain pending.

## Architecture

![Timer architecture](docs/diagrams/architecture.svg)

The peripheral implements:

- APB4-compatible slave interface with `PSTRB`, `PPROT`, `PREADY`, and `PSLVERR`
- 32-bit writable free-running counter
- Programmable compare register
- Sticky compare-match status
- Level-sensitive, maskable interrupt
- Write-one-to-clear status
- Natural 32-bit rollover
- Invalid and misaligned address errors
- Constant one-cycle access with no inserted wait states

## Register map

| Offset | Register | Access | Purpose |
|---:|---|---|---|
| `0x000` | `CONTROL` | R/W | Timer enable and interrupt enable |
| `0x004` | `COUNTER` | R/W | Current counter value |
| `0x008` | `COMPARE` | R/W | Compare threshold |
| `0x00C` | `STATUS` | R/W1C + R/O | Match pending and live IRQ state |

The machine-readable source of truth is [`spec/register_map.json`](spec/register_map.json). It generates matching SystemVerilog, C++, and Markdown definitions.

## Verification environment

![Verification environment](docs/diagrams/verification-environment.svg)

The primary environment uses meaningful C++ classes rather than a procedural testbench wrapped in classes:

- `SimulationContext` owns the Verilated DUT, clock, reset, time, and portable VCD trace.
- `ApbTransaction` records stimulus and observed transfer results.
- `ApbMaster` performs legal setup/access phases and supports back-to-back transfers.
- `TimerReferenceModel` predicts cycle-accurate timer state independently from the RTL.
- `Scoreboard` compares bus responses and IRQ behaviour and returns non-zero on failure.
- `FunctionalCoverage` records semantic events and register/strobe categories.
- `TestRunner` runs isolated named tests with deterministic seeds.

The suite contains **29 directed tests plus one deterministic randomized test**. See [`docs/verification-plan.md`](docs/verification-plan.md).

### Measured verification evidence

- Icarus smoke test: **32/32 checks passed**
- Verilator regression: **30/30 tests passed**
- Scoreboard: **4,099 checks, zero failures**
- Randomized traffic: **500 deterministic APB operations, seed 107**
- Generic Yosys synthesis: **492 cells, 67 sequential cells, zero structural problems**

The focused GTKWave capture below shows the APB transaction activity and internal timer state used to inspect the set-dominant clear-versus-match corner case.

![GTKWave APB timer verification capture](results/verification/gtkwave-t18-clear-match-collision.png)

## Quick start on Ubuntu 24.04

```bash
./scripts/setup_tools.sh
make tool-versions
make all
```

Run one test:

```bash
make verify TEST=T18_clear_match_collision_set_dominant TRACE=1
```

Replay a random regression:

```bash
make verify SEED=107 RANDOM_OPERATIONS=1000
```

Open the Icarus waveform:

```bash
gtkwave build/apb_timer_smoke.vcd
```

## Common targets

```text
make generate          regenerate shared register definitions
make check-generated   verify generated files are not stale
make lint              Verilator RTL lint
make smoke             Icarus self-checking smoke test and VCD
make verify            complete C++/Verilator regression
make coverage          regression with VCD and coverage output
make synth             generic Yosys synthesis
make openlane          OpenLane 2 physical implementation
make results           collect only metrics that actually exist
```

## Frozen behavioural rules

- `PREADY` is always high; the slave inserts no wait states.
- A transfer completes during `PSEL && PENABLE`.
- Invalid or misaligned accesses assert `PSLVERR`, return zero on reads, and do not modify registers.
- Effective CONTROL and COUNTER writes suppress automatic increment on their completion edge.
- COMPARE and STATUS writes do not suppress counting.
- A compare event occurs when automatic increment enters the programmed compare value.
- A COMPARE write takes effect after its completion edge; that edge compares against the old value.
- Match status is sticky and write-one-to-clear.
- A simultaneous hardware match and software clear is **set-dominant**.
- `irq_o = MATCH_PENDING && IRQ_ENABLE`.

Full details are in [`docs/specification.md`](docs/specification.md).

## ASIC flow

The default target is:

- PDK: `sky130A`
- Library: `sky130_fd_sc_hd`
- Clock: 100 MHz (`10 ns`)
- Flow: Yosys → OpenROAD/OpenLane → Magic/KLayout checks → GDSII

OpenLane is kept separate from `make all` because it is heavier and must be run in a pinned environment. Replace the default container tag with a tested release digest before publishing final results.

```bash
OPENLANE_IMAGE=ghcr.io/efabless/openlane2:<pinned-release-or-digest> make openlane
make results
```

## Results and résumé evidence

The repository starts with **no fabricated PPA or signoff values**. [`scripts/collect_results.py`](scripts/collect_results.py) writes `Not measured` until genuine reports exist.

Only use a résumé claim after its evidence is present:

- **APB4/MMIO:** RTL, bus tests, and assertions pass.
- **C++ OOP verification:** reusable classes and automated regression pass.
- **Logic synthesis:** Yosys report is committed.
- **100 MHz timing closure:** setup and hold slack are non-negative at 10 ns.
- **RTL-to-GDSII:** final GDS exists and the flow log is preserved.
- **DRC/LVS clean:** actual zero-error reports exist.

Do not describe this project as UVM, commercial signoff, or tapeout-ready.

## Repository layout

```text
rtl/                 synthesizable timer
verification/rtl/    Icarus smoke test
verification/cpp/    object-oriented Verilator environment
assertions/          APB and interface assertions
spec/                register-map source of truth
synthesis/           Yosys and SDC inputs
openlane/            physical-design configuration
scripts/             reproducible flow and report collection
docs/                specification, plans, diagrams, interview notes
results/              only preserved, tool-generated evidence
```

## Qualcomm-focused interview summary

> I designed a 32-bit APB4 timer peripheral with partial register writes, sticky compare status, a maskable level interrupt, deterministic error handling, and explicitly defined simultaneous-event priority. I verified it using a C++17 Verilator environment with an APB bus-functional model, independent cycle-accurate reference model, scoreboard, directed corner cases, deterministic random tests, assertions, and semantic coverage. I then synthesized it with Yosys and prepared a reproducible OpenLane/OpenROAD flow, reporting only measured timing, area, DRC, and LVS results.

## License

Apache-2.0.
