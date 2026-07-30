#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
command -v verilator >/dev/null 2>&1 || {
  echo "ERROR: verilator is not installed. Run scripts/setup_tools.sh." >&2
  exit 127
}
EVIDENCE_MODE="${EVIDENCE_MODE:-canonical}"
mkdir -p build/lint results/verification
verilator --version
verilator --lint-only --sv --assert --Wall \
  --Wno-DECLFILENAME --Wno-UNUSEDPARAM --Wno-UNUSEDSIGNAL --Wno-SYNCASYNCNET \
  -DAPB_TIMER_ENABLE_SVA \
  -Irtl \
  --top-module apb_timer \
  rtl/apb_timer.sv assertions/apb_timer_sva.sv \
  2>&1 | tee build/lint/verilator-lint.log
if [[ "$EVIDENCE_MODE" == "canonical" ]]; then
  cp build/lint/verilator-lint.log results/verification/verilator-lint.log
fi
