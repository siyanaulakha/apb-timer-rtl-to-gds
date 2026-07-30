#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
for tool in iverilog vvp; do
  command -v "$tool" >/dev/null 2>&1 || {
    echo "ERROR: $tool is not installed. Run scripts/setup_tools.sh." >&2
    exit 127
  }
done
EVIDENCE_MODE="${EVIDENCE_MODE:-canonical}"
mkdir -p build results/verification
iverilog -g2012 -Wall -Irtl \
  -s apb_timer_smoke_tb \
  -o build/apb_timer_smoke.vvp \
  rtl/apb_timer.sv verification/rtl/apb_timer_smoke_tb.sv
vvp build/apb_timer_smoke.vvp | tee build/iverilog-smoke.log
if [[ "$EVIDENCE_MODE" == "canonical" ]]; then
  cp build/iverilog-smoke.log results/verification/iverilog-smoke.log
fi
