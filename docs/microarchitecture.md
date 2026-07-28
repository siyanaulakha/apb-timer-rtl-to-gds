# Microarchitecture

The implementation is deliberately one RTL module because the design contains only four software-visible registers and no independent clock domains.

## Datapath

- `counter_q`: 32-bit state register and incrementer.
- `compare_q`: 32-bit threshold register.
- Equality comparator between `counter_q + 1` and the old `compare_q`.
- Byte-merge function used by COUNTER and COMPARE writes.

## Control

- Address/alignment decoder derives valid access and `PSLVERR`.
- Completed APB writes generate per-register write enables.
- CONTROL and COUNTER write enables suppress automatic counting.
- Sticky status uses a set-dominant next-state equation.
- IRQ is combinationally derived from pending and mask state.

## Design rationale

A writable counter avoids impractically long rollover tests and mirrors timer blocks that permit software initialization. A sticky status bit prevents a one-cycle compare pulse from being missed. A level interrupt is simpler for firmware and is naturally cleared through the W1C status mechanism.
