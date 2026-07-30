# Final-Run Warning Audit

This document separates final signoff results from non-fatal flow warnings observed in `apb_timer_sky130_signoff_policy_only_20260730_021912`.

| Warning category | Assessment |
|---|---|
| OpenLane frontend lint warnings: 453 | **Open qualification.** The metrics record zero lint errors and zero inferred latches, but the compact frozen evidence does not preserve a line-level classification of all 453 warnings. They are not represented as individually waived. |
| `GRT-0097` during mid-PnR | **Intermediate-flow warning.** The final routed design completed with zero route DRC errors. |
| `DRT-0349` LEF58 enclosure/CUTCLASS limitation | **Tool/PDK compatibility warning.** Final OpenROAD, Magic, and KLayout DRC counts are all zero. |
| Wire-length threshold not configured | **Checker skipped.** Actual routed wire length is still recorded as 16.252 mm; no threshold-based long-wire claim is made. |
| `VSRC_LOC_FILES` not provided | **Known limitation.** Package-aware IR-drop accuracy is not claimed. |
| Antenna gate-information metadata warning | **Metadata warning.** Final antenna violation count is zero. |
| Constant `PREADY` endpoint | **Intentional constraint exception.** `PREADY` is tied high by RTL and documented as a false-path endpoint. |

No final setup, hold, slew, fanout, capacitance, DRC, LVS, or antenna violation remains in the frozen metrics.
