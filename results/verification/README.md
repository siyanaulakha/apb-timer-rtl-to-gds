# Verification Evidence

This directory contains compact evidence from real RTL verification runs.

## GTKWave capture

[`gtkwave-t18-clear-match-collision.png`](gtkwave-t18-clear-match-collision.png) is a focused waveform screenshot showing:

- APB setup and access phases (`PSEL`, `PENABLE`, `PWRITE`)
- Register addressing and data (`PADDR`, `PWDATA`, `PRDATA`, `PSTRB`)
- Timer control state (`timer_enable_q`, `irq_enable_q`)
- Counter/compare state
- Sticky match state and interrupt output

The screenshot complements, but does not replace, the automated regression and scoreboard logs.
