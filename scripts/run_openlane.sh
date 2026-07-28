#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
mkdir -p build/openlane results/physical-design

if command -v openlane >/dev/null 2>&1; then
  openlane --run-tag apb_timer_100mhz --output-dir "$ROOT/build/openlane" "$ROOT/openlane/config.json"
elif command -v docker >/dev/null 2>&1; then
  IMAGE="${OPENLANE_IMAGE:-ghcr.io/efabless/openlane2:latest}"
  echo "Using OpenLane container: $IMAGE"
  echo "For reproducible final results, replace :latest with a tested release digest."
  docker run --rm \
    -v "$ROOT:/work" \
    -w /work \
    "$IMAGE" \
    openlane --run-tag apb_timer_100mhz --output-dir /work/build/openlane /work/openlane/config.json
else
  echo "ERROR: neither OpenLane nor Docker is available." >&2
  exit 127
fi

python3 scripts/collect_results.py
