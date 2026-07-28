#!/usr/bin/env bash
set -euo pipefail

if [[ "${EUID}" -eq 0 ]]; then
  SUDO=""
elif command -v sudo >/dev/null 2>&1; then
  SUDO="sudo"
else
  echo "ERROR: sudo is required to install Ubuntu packages." >&2
  exit 1
fi

$SUDO apt-get update
$SUDO apt-get install -y \
  build-essential git make python3 python3-venv \
  iverilog gtkwave verilator yosys \
  clang-format jq graphviz

cat <<'MESSAGE'
Base RTL and verification tools are installed.

OpenLane 2 is intentionally not installed by this script because its supported
installation method and release should be pinned separately. Use the project's
container flow after selecting and recording an OpenLane 2 release digest.
MESSAGE
