#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
command -v verilator >/dev/null 2>&1 || {
  echo "ERROR: verilator is not installed. Run scripts/setup_tools.sh." >&2
  exit 127
}

SEED="${SEED:-107}"
TEST="${TEST:-}"
RANDOM_OPERATIONS="${RANDOM_OPERATIONS:-500}"
TRACE="${TRACE:-0}"
OBJ_DIR="build/verilator/obj_dir"
mkdir -p "$OBJ_DIR" results/verification
rm -f coverage.dat

CPP_SOURCES=(
  verification/cpp/src/simulation_context.cpp
  verification/cpp/src/apb_transaction.cpp
  verification/cpp/src/timer_reference_model.cpp
  verification/cpp/src/scoreboard.cpp
  verification/cpp/src/functional_coverage.cpp
  verification/cpp/src/apb_master.cpp
  verification/cpp/src/test_runner.cpp
  verification/cpp/src/directed_tests.cpp
  verification/cpp/src/random_tests.cpp
  verification/cpp/src/main.cpp
)

verilator --cc --exe --build --sv \
  --top-module apb_timer \
  --Mdir "$OBJ_DIR" \
  -o apb_timer_verification \
  -Irtl \
  -Iverification/cpp/include \
  --trace --assert --coverage \
  -DAPB_TIMER_ENABLE_SVA \
  -Wall --Wno-DECLFILENAME --Wno-UNUSEDPARAM --Wno-UNUSEDSIGNAL --Wno-SYNCASYNCNET \
  -CFLAGS "-std=c++17 -O2 -Wall -Wextra -Wpedantic -I$ROOT/verification/cpp/include" \
  rtl/apb_timer.sv assertions/apb_timer_sva.sv \
  "${CPP_SOURCES[@]}"

ARGS=(--seed "$SEED" --random-operations "$RANDOM_OPERATIONS")
if [[ -n "$TEST" ]]; then
  ARGS+=(--test "$TEST")
fi
if [[ "$TRACE" == "1" ]]; then
  ARGS+=(--trace)
fi

"$OBJ_DIR/apb_timer_verification" "${ARGS[@]}" \
  2>&1 | tee results/verification/verilator-regression.log

if [[ -f coverage.dat ]]; then
  cp coverage.dat results/verification/verilator-coverage.dat
  if command -v verilator_coverage >/dev/null 2>&1; then
    verilator_coverage --write-info results/verification/verilator-coverage.info coverage.dat
  fi
fi
