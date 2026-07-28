# Results Summary

> Values below are taken from real OSS CAD Suite runs performed on Ubuntu on 2026-07-29. Technology-independent synthesis counts must not be presented as Sky130 area, timing, or power results.

## Verification

- Generated-register consistency: **PASS**
- Verilator RTL lint: **PASS**
- Icarus smoke simulation: **PASS — 32/32 checks**
- Icarus waveform: **Generated (`build/apb_timer_smoke.vcd`)**
- Verilator C++ RTL regression: **PASS — 30/30 tests**
- Scoreboard: **PASS — 4,099 checks, 0 failures**
- Deterministic randomized sequence: **500 APB operations, seed 107**
- Assertions: **Enabled in the Verilator regression**
- Functional coverage collection: **PASS**
- Focused GTKWave evidence: [`gtkwave-t18-clear-match-collision.png`](verification/gtkwave-t18-clear-match-collision.png)

### Selected functional-coverage bins

| Bin | Hits |
|---|---:|
| APB reads | 271 |
| APB writes | 291 |
| Partial-strobe writes | 122 |
| Invalid accesses | 90 |
| Misaligned accesses | 181 |
| Compare matches | 15 |
| Counter rollovers | 2 |
| Random resets | 12 |
| Clear/match collision | 1 |

## Generic Yosys synthesis

- Yosys parse/elaboration: **PASS**
- Yosys structural check: **PASS — 0 problems**
- Generic cell count: **492**
- Wire count: **388**
- Wire bits: **813**
- Sequential cells: **67**
- Generic AND cells: **231**
- Generic OR cells: **107**
- Generic XOR cells: **33**
- Generic XNOR cells: **21**
- Generic NOT cells: **29**
- Generic MUX cells: **4**

These counts are technology-independent and must not be presented as Sky130 area, timing, or power results.

## Physical design

- OpenLane metrics file: **Not run**
- Implemented area: **Not measured**
- Setup WNS: **Not measured**
- Hold WNS: **Not measured**
- DRC error count: **Not measured**
- LVS error count: **Not measured**

## Resume gate

It is now defensible to claim APB4 RTL design, self-checking Icarus verification, reusable C++17/Verilator verification, an independent reference model, automated scoreboarding, directed and randomized testing, semantic functional coverage, assertions, and generic Yosys synthesis.

Do not yet claim Sky130 timing closure, physical area, DRC-clean layout, LVS-clean layout, or completed RTL-to-GDSII.
