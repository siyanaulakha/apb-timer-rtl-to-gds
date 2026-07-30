# Results and Evidence Policy

The repository keeps compact, inspectable evidence in Git and keeps large generated run directories out of version control.

## Canonical evidence

- `verification/`: canonical 30-test Verilator regression, semantic coverage, Icarus smoke result, lint log, and focused waveform image
- `synthesis/`: technology-independent Yosys log and synthesized netlists
- `final-openlane/`: frozen Sky130 metrics, exact constraints/configuration, signoff summary, warning audit, and local-view checksums
- `results-summary.md`: generated cross-flow summary

Focused tests, reduced CI runs, temporary flow logs, and intermediate OpenLane directories belong under `build/`, `results/local-run/`, or `openlane/runs/` and are ignored.

Regenerate the canonical evidence with:

```bash
make all
```

Package the ignored final physical views for a GitHub Release with:

```bash
./scripts/package_final_openlane.sh
```
