# APB Timer Sky130 Physical-Design Signoff

## Final run

`apb_timer_sky130_signoff_policy_only_20260730_021912`

## Constraint policy

- Signoff clock period: **10.0 ns (100 MHz)**
- PnR clock period: **9.8 ns**
- Input delay: **2.0 ns**
- Output delay: **2.0 ns**
- `PREADY`: statically asserted and documented as a constant false-path endpoint

## Timing and physical verification

| Check | Result |
|---|---:|
| Worst setup slack | +0.174 ns |
| Worst hold slack | +0.113 ns |
| Setup violations | 0 |
| Hold violations | 0 |
| Maximum-slew violations | 0 |
| Maximum-fanout violations | 0 |
| OpenROAD routing DRC errors | 0 |
| Magic DRC errors | 0 |
| KLayout DRC errors | 0 |
| LVS errors | 0 |
| Antenna violations | 0 |

## Physical metrics

| Metric | Value |
|---|---:|
| Die area | 16,900 µm² (`130 × 130 µm`) |
| Core area | 12,589.6 µm² |
| Standard-cell area | 5,526.55 µm² |
| Core utilization | 43.8978% |
| Standard-cell count | 710 |
| Routed wire length | 16.252 mm |
| Estimated total power | 0.723 mW |

## Qualifications

- The power value is an OpenLane estimate under the flow's activity assumptions, not workload-characterized silicon power.
- Package-aware IR-drop accuracy is not claimed because `VSRC_LOC_FILES` was not supplied.
- OpenLane recorded 453 frontend lint warnings and zero lint errors. The compact evidence does not contain a line-level waiver for every warning; see [`WARNING_AUDIT.md`](WARNING_AUDIT.md).
- This is an open-source physical-design result, not commercial signoff or a tapeout claim.
