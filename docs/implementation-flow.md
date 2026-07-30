# Implementation Flow

## Canonical RTL verification

```bash
make generate
make check-generated
make lint
make smoke
make evidence
```

`make evidence` is fixed to all 30 tests, seed `107`, 500 randomized APB operations, and no tracing. Focused or reduced runs are written under `build/verilator/runs/` and cannot overwrite the canonical evidence.

Run one focused test without changing the committed evidence:

```bash
make verify TEST=T18_clear_match_collision_set_dominant TRACE=1
```

## Generic synthesis

```bash
make synth
```

The Yosys flow is technology-independent and does not consume `synthesis/constraints.sdc`; that file is an illustrative standalone-STA constraint set only.

## Sky130 physical design

Activate the tested OpenLane 2 Python environment, then run:

```bash
RUN_TAG="apb_timer_sky130_$(date +%Y%m%d_%H%M%S)" make openlane
```

The wrapper executes:

```text
python3 -m openlane --dockerized --flow Classic --run-tag <tag> openlane/config.json
```

It deliberately refuses to fall back to an unpinned `latest` container.

The frozen final run is documented under `results/final-openlane/`. Its exact configuration and both SDC files are committed separately from the active flow configuration.

## Result collection

```bash
make results
```

The collector reads only committed/tool-generated output, preferring `results/final-openlane/metrics.json`. Missing values remain `Not measured`.

## Release bundle

After a successful final flow has populated the ignored physical views:

```bash
./scripts/package_final_openlane.sh
```

Upload the generated archive and checksum from `dist/` to the matching GitHub Release.
