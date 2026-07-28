#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
command -v verilator >/dev/null 2>&1 || {
  echo "ERROR: verilator is not installed. Run scripts/setup_tools.sh." >&2
  exit 127
}
mkdir -p build/lint results/verification
verilator --lint-only --sv --Wall --Wno-DECLFILENAME --Wno-UNUSEDPARAM \
  -Irtl \
  --top-module apb_timer \
  rtl/apb_timer.sv \
  2>&1 | tee build/lint/verilator-lint.log
cp build/lint/verilator-lint.log results/verification/verilator-lint.log
