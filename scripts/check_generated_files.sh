#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

cp "$ROOT/rtl/generated/apb_timer_regs.svh" "$TMP/apb_timer_regs.svh"
cp "$ROOT/verification/cpp/include/generated/timer_registers.hpp" "$TMP/timer_registers.hpp"
cp "$ROOT/docs/generated/register-map.md" "$TMP/register-map.md"
python3 "$ROOT/scripts/generate_registers.py"

diff -u "$TMP/apb_timer_regs.svh" "$ROOT/rtl/generated/apb_timer_regs.svh"
diff -u "$TMP/timer_registers.hpp" "$ROOT/verification/cpp/include/generated/timer_registers.hpp"
diff -u "$TMP/register-map.md" "$ROOT/docs/generated/register-map.md"
echo "Generated register files are current."
