# Verification Plan

## Strategy

1. Verilator lint for structural and width issues.
2. Icarus self-checking smoke test for basic RTL portability.
3. C++ directed regression for requirement-level behaviour.
4. Deterministic randomized APB sequences with seed replay.
5. Assertions for protocol assumptions and known outputs.
6. Semantic functional coverage for corner-case evidence.

## Directed suite

| ID | Scenario | Principal expected result |
|---:|---|---|
| T01 | Reset values | All state equals specified reset values |
| T02 | CONTROL write/read | Bits 1:0 retain valid write |
| T03 | CONTROL strobe | Only byte strobe zero modifies control |
| T04 | Full COMPARE write | All 32 bits update |
| T05 | Partial COMPARE write | Selected bytes merge correctly |
| T06 | Full COUNTER write | All 32 bits update |
| T07 | Partial COUNTER write | Selected bytes merge correctly |
| T08 | Disabled hold | Counter remains constant |
| T09 | Enabled increment | Counter advances each clock |
| T10 | CONTROL precedence | Completion edge does not increment |
| T11 | COUNTER precedence | Load replaces increment on completion edge |
| T12 | Match pending | Increment into COMPARE sets status |
| T13 | IRQ masked | Pending sets but IRQ remains low |
| T14 | IRQ enabled | Pending sets and IRQ rises |
| T15 | Enable IRQ after event | Existing pending state drives IRQ |
| T16 | Disable IRQ | IRQ falls but pending remains |
| T17 | W1C | Status and IRQ clear |
| T18 | Clear/match collision | Hardware set dominates clear |
| T19 | Disable/restart | Counter holds then resumes |
| T20 | Rollover | `0xFFFFFFFF` advances to zero |
| T21 | COMPARE zero | Rollover creates match |
| T22 | Back-to-back APB | No inserted idle edge required |
| T23 | Invalid aligned address | Error, zero read, no corruption |
| T24 | Misaligned address | Error, zero read, no corruption |
| T25 | Zero strobe | No write and no false suppression |
| T26 | Compare changed running | No write-induced event; new threshold later matches |
| T27 | Old compare on write edge | Completion edge uses old compare value |
| T28 | Reset while active | State and IRQ immediately return to reset |
| T29 | Repeated match pending | Pending remains sticky across later events |
| T30 | Random sequences | Model and DUT remain equivalent for replayable seed |

## Pass criteria

- Every selected test reports pass.
- The process exits zero.
- Scoreboard failures are zero.
- Assertions do not fire.
- Required semantic coverage bins have at least one hit in the full regression.
