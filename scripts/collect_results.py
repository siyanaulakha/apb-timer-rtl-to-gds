#!/usr/bin/env python3
"""Collect only metrics that exist in real tool output; never invent values."""
from __future__ import annotations

import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SUMMARY = ROOT / "results" / "results-summary.md"


def first_match(text: str, patterns: list[str]) -> str | None:
    for pattern in patterns:
        match = re.search(pattern, text, re.MULTILINE | re.IGNORECASE)
        if match:
            return match.group(1).strip()
    return None


def find_metrics_json() -> Path | None:
    candidates = sorted((ROOT / "build" / "openlane").rglob("metrics.json")) if (ROOT / "build" / "openlane").exists() else []
    return candidates[-1] if candidates else None


def status(value: object | None) -> str:
    return str(value) if value not in (None, "") else "Not measured"


def main() -> None:
    yosys_log = ROOT / "results" / "synthesis" / "yosys.log"
    yosys_text = yosys_log.read_text(errors="replace") if yosys_log.exists() else ""
    cell_count = first_match(yosys_text, [r"Number of cells:\s+(\d+)"])
    wire_bits = first_match(yosys_text, [r"Number of wire bits:\s+(\d+)"])

    metrics_path = find_metrics_json()
    metrics: dict[str, object] = {}
    if metrics_path:
        try:
            metrics = json.loads(metrics_path.read_text())
        except (json.JSONDecodeError, OSError):
            metrics = {}

    def metric(*keys: str) -> object | None:
        for key in keys:
            if key in metrics:
                return metrics[key]
        return None

    setup_wns = metric("timing__setup__wns", "design__instance__setup__wns")
    hold_wns = metric("timing__hold__wns", "design__instance__hold__wns")
    area = metric("design__instance__area", "design__core__area")
    drc = metric("magic__drc_error__count", "klayout__drc_error__count")
    lvs = metric("netgen__lvs_error__count")

    lines = [
        "# Results Summary",
        "",
        "> This file reports only values found in tool-generated output. Missing values remain `Not measured`.",
        "",
        "## Verification",
        "",
        f"- Icarus smoke log: {'Present' if (ROOT / 'results/verification/iverilog-smoke.log').exists() else 'Not run'}",
        f"- Verilator regression log: {'Present' if (ROOT / 'results/verification/verilator-regression.log').exists() else 'Not run'}",
        f"- Functional coverage report: {'Present' if (ROOT / 'results/verification/functional-coverage.md').exists() else 'Not run'}",
        "",
        "## Synthesis",
        "",
        f"- Yosys report: {'Present' if yosys_log.exists() else 'Not run'}",
        f"- Generic cell count: {status(cell_count)}",
        f"- Wire bits: {status(wire_bits)}",
        "",
        "## Physical design",
        "",
        f"- OpenLane metrics file: `{metrics_path.relative_to(ROOT)}`" if metrics_path else "- OpenLane metrics file: Not run",
        f"- Implemented area: {status(area)}",
        f"- Setup WNS: {status(setup_wns)}",
        f"- Hold WNS: {status(hold_wns)}",
        f"- DRC error count: {status(drc)}",
        f"- LVS error count: {status(lvs)}",
        "",
        "## Resume gate",
        "",
        "Do not replace bracketed résumé metrics until the corresponding value above is measured and its report is committed.",
        "",
    ]
    SUMMARY.write_text("\n".join(lines), encoding="utf-8")
    print(f"Wrote {SUMMARY}")


if __name__ == "__main__":
    main()
