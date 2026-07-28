#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
command -v yosys >/dev/null 2>&1 || {
  echo "ERROR: yosys is not installed. Run scripts/setup_tools.sh." >&2
  exit 127
}
mkdir -p build/yosys results/synthesis
yosys -s synthesis/synth.ys 2>&1 | tee build/yosys/yosys.log
cp build/yosys/yosys.log results/synthesis/yosys.log
cp build/yosys/apb_timer_synth.v results/synthesis/apb_timer_synth.v
cp build/yosys/apb_timer_synth.json results/synthesis/apb_timer_synth.json
