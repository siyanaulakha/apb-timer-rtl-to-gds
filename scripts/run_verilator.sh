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
EVIDENCE_MODE="${EVIDENCE_MODE:-auto}"
OBJ_DIR="build/verilator/obj_dir"

slugify() {
  printf '%s' "$1" |
    tr '[:upper:]' '[:lower:]' |
    sed -E 's/[^a-z0-9._-]+/-/g; s/^-+//; s/-+$//'
}

if [[ "$EVIDENCE_MODE" == "auto" ]]; then
  if [[ -z "$TEST" && "$SEED" == "107" && "$RANDOM_OPERATIONS" == "500" && "$TRACE" == "0" ]]; then
    EVIDENCE_MODE="canonical"
  elif [[ -n "$TEST" ]]; then
    EVIDENCE_MODE="focused-$(slugify "$TEST")"
  else
    EVIDENCE_MODE="adhoc-seed-${SEED}-ops-${RANDOM_OPERATIONS}-trace-${TRACE}"
  fi
fi

if [[ "$EVIDENCE_MODE" == "canonical" ]]; then
  RESULT_DIR="results/verification"
  RUN_SCOPE="full-regression"
else
  RUN_SLUG="$(slugify "$EVIDENCE_MODE")"
  [[ -n "$RUN_SLUG" ]] || RUN_SLUG="adhoc"
  RESULT_DIR="build/verilator/runs/$RUN_SLUG"
  RUN_SCOPE="$RUN_SLUG"
fi

REGRESSION_LOG="$RESULT_DIR/verilator-regression.log"
COVERAGE_REPORT="$RESULT_DIR/functional-coverage.md"
COVERAGE_DAT="$RESULT_DIR/verilator-coverage.dat"
COVERAGE_INFO="$RESULT_DIR/verilator-coverage.info"
METADATA="$RESULT_DIR/verilator-run-metadata.json"
TRACE_PATH="$RESULT_DIR/apb_timer.vcd"

mkdir -p "$RESULT_DIR"
rm -rf "$OBJ_DIR"
mkdir -p "$OBJ_DIR"
rm -f coverage.dat

VERILATOR_VERSION="$(verilator --version)"
printf '%s\n' "$VERILATOR_VERSION"
printf 'evidence_mode=%s\n' "$EVIDENCE_MODE"
printf 'result_dir=%s\n' "$RESULT_DIR"

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

ARGS=(
  --seed "$SEED"
  --random-operations "$RANDOM_OPERATIONS"
  --coverage-path "$COVERAGE_REPORT"
)

if [[ -n "$TEST" ]]; then
  ARGS+=(--test "$TEST")
fi
if [[ "$TRACE" == "1" ]]; then
  ARGS+=(--trace --trace-path "$TRACE_PATH")
fi

"$OBJ_DIR/apb_timer_verification" "${ARGS[@]}" \
  2>&1 | tee "$REGRESSION_LOG"

if [[ -f coverage.dat ]]; then
  cp coverage.dat "$COVERAGE_DAT"
  if command -v verilator_coverage >/dev/null 2>&1; then
    verilator_coverage --write-info "$COVERAGE_INFO" coverage.dat
  fi
  rm -f coverage.dat
fi

SOURCE_COMMIT="unknown"
if command -v git >/dev/null 2>&1 && git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
  SOURCE_COMMIT="$(git rev-parse HEAD)"
fi

SEED="$SEED" \
TEST="$TEST" \
RANDOM_OPERATIONS="$RANDOM_OPERATIONS" \
TRACE="$TRACE" \
EVIDENCE_MODE="$EVIDENCE_MODE" \
RUN_SCOPE="$RUN_SCOPE" \
VERILATOR_VERSION="$VERILATOR_VERSION" \
SOURCE_COMMIT="$SOURCE_COMMIT" \
METADATA="$METADATA" \
python3 - <<'PY'
import json
import os
from pathlib import Path

metadata = {
    "evidence_mode": os.environ["EVIDENCE_MODE"],
    "run_scope": os.environ["RUN_SCOPE"],
    "selected_test": os.environ["TEST"] or None,
    "seed": int(os.environ["SEED"]),
    "random_operations": int(os.environ["RANDOM_OPERATIONS"]),
    "trace_enabled": os.environ["TRACE"] == "1",
    "verilator_version": os.environ["VERILATOR_VERSION"],
    "source_commit_at_run": os.environ["SOURCE_COMMIT"],
}
Path(os.environ["METADATA"]).write_text(
    json.dumps(metadata, indent=2) + "\n",
    encoding="utf-8",
)
PY

if [[ "$EVIDENCE_MODE" == "canonical" ]]; then
  grep -Fq "Tests: 30 passed, 0 failed, 30 executed" "$REGRESSION_LOG" || {
    echo "ERROR: canonical evidence did not execute and pass all 30 tests." >&2
    exit 1
  }
  grep -Fq "Scoreboard: PASS checks=4099 failures=0" "$REGRESSION_LOG" || {
    echo "ERROR: canonical evidence did not produce the expected 4,099 passing checks." >&2
    exit 1
  }
  echo "CANONICAL_VERIFICATION_EVIDENCE=PASS"
else
  echo "NONCANONICAL_VERIFICATION_RUN=PASS"
fi
