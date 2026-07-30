#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

RUN_TAG="${RUN_TAG:-apb_timer_sky130_$(date +%Y%m%d_%H%M%S)}"
LOG_DIR="$ROOT/results/local-run"
LOG_FILE="$LOG_DIR/${RUN_TAG}.log"
mkdir -p "$LOG_DIR"

if ! python3 -c 'import openlane' >/dev/null 2>&1; then
  cat >&2 <<'MSG'
ERROR: the OpenLane 2 Python environment is not active.
Activate the tested OpenLane 2 environment, then rerun `make openlane`.
The wrapper intentionally does not fall back to an unpinned `latest` container.
MSG
  exit 127
fi

set -o pipefail
python3 -m openlane \
  --dockerized \
  --flow Classic \
  --run-tag "$RUN_TAG" \
  openlane/config.json \
  2>&1 | tee "$LOG_FILE"

printf 'run_tag=%s\n' "$RUN_TAG"
printf 'run_directory=%s\n' "$ROOT/openlane/runs/$RUN_TAG"
printf 'log_file=%s\n' "$LOG_FILE"

python3 scripts/collect_results.py
