# APB Timer Sky130 Physical-Design Signoff

## Final run

`apb_timer_sky130_signoff_policy_only_20260730_021912`

## Timing constraints

- Signoff clock period: 10.0 ns
- Target frequency: 100 MHz
- PnR clock period: 9.8 ns
- Input delay: 2.0 ns
- Output delay: 2.0 ns
- `PREADY` is statically asserted and documented as a false-path endpoint.

## Final signoff

| Check | Result |
|---|---:|
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
| Standard-cell area | 5,526.55 µm² |
| Core utilization | 43.8978% |
| Standard-cell count | 710 |
| Routed wire length | 16,252 µm |
| Estimated total power | 0.723 mW |

## Qualification

`PREADY` is assigned constant logic high in the RTL for a zero-wait-state APB slave. The signoff audit therefore permits this documented constant endpoint while rejecting all unexpected unconstrained endpoints.
