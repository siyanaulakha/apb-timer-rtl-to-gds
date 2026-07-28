# Implementation Flow

## RTL verification

```bash
make generate
make lint
make smoke
make verify SEED=107 RANDOM_OPERATIONS=500
```

## Generic synthesis

```bash
make synth
```

Inspect `results/synthesis/yosys.log` for latches, unresolved modules, warnings, and generic cell statistics.

## Physical design

Pin a tested OpenLane 2 image or local installation before publishing results:

```bash
OPENLANE_IMAGE=ghcr.io/efabless/openlane2:<release-or-digest> make openlane
```

The initial target is 100 MHz with a small absolute floorplan. Tiny blocks sometimes require floorplan adjustment; any change should be recorded with the associated run tag.

## Result integrity

Run:

```bash
make results
```

The collector reports `Not measured` rather than guessing. Preserve the full flow log, metrics JSON, timing reports, DRC/LVS reports, final GDS, and tool-version record.
