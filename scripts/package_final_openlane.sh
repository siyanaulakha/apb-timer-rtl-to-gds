#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SOURCE="$ROOT/results/final-openlane"
DIST="$ROOT/dist"
BUNDLE_NAME="apb_timer_sky130_signoff_bundle"
STAGING="$(mktemp -d)"
trap 'rm -rf "$STAGING"' EXIT

required=(
  FINAL_RUN_TAG.txt
  SIGNOFF_SUMMARY.md
  metrics.json
  metrics.csv
  FROZEN_CONFIG.json
  FROZEN_PNR.sdc
  FROZEN_SIGNOFF.sdc
  def
  gds
  lef
  lib
  nl
  pnl
  sdc
  sdf
  spef
)

for item in "${required[@]}"; do
  if [[ ! -e "$SOURCE/$item" ]]; then
    echo "ERROR: missing final artifact: $SOURCE/$item" >&2
    exit 1
  fi
done

mkdir -p "$DIST" "$STAGING/$BUNDLE_NAME"
for item in "${required[@]}"; do
  cp -a "$SOURCE/$item" "$STAGING/$BUNDLE_NAME/"
done

(
  cd "$STAGING/$BUNDLE_NAME"
  find . -type f ! -name SHA256SUMS -print0 |
    sort -z |
    xargs -0 sha256sum > SHA256SUMS
)

tar -C "$STAGING" -czf "$DIST/${BUNDLE_NAME}.tar.gz" "$BUNDLE_NAME"
sha256sum "$DIST/${BUNDLE_NAME}.tar.gz" > "$DIST/${BUNDLE_NAME}.tar.gz.sha256"

ls -lh "$DIST/${BUNDLE_NAME}.tar.gz" "$DIST/${BUNDLE_NAME}.tar.gz.sha256"
