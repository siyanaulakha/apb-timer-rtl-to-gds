#!/usr/bin/env bash
set -u
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
OUT="$ROOT/results/tool-versions.txt"
mkdir -p "$ROOT/results"
{
  echo "timestamp=$(date --iso-8601=seconds 2>/dev/null || date)"
  echo "kernel=$(uname -srmo)"
  command -v lsb_release >/dev/null 2>&1 && lsb_release -ds || true
  git --version 2>/dev/null || true
  make --version 2>/dev/null | head -n 1 || true
  gcc --version 2>/dev/null | head -n 1 || true
  g++ --version 2>/dev/null | head -n 1 || true
  python3 --version 2>/dev/null || true
  iverilog -V 2>/dev/null | head -n 2 || true
  verilator --version 2>/dev/null || true
  yosys -V 2>/dev/null || true
  openroad -version 2>/dev/null || true
  magic --version 2>/dev/null || true
  klayout -v 2>/dev/null || true
  docker --version 2>/dev/null || true
} | tee "$OUT"
