# APB Timing Notes

## Write

```text
Cycle N setup:  PSEL=1, PENABLE=0, PWRITE=1, address/data/strobes stable
Cycle N+1 access: PSEL=1, PENABLE=1; PREADY=1; write commits on rising edge
```

## Read

```text
Cycle N setup:  PSEL=1, PENABLE=0, PWRITE=0, address stable
Cycle N+1 access: PSEL=1, PENABLE=1; PRDATA and PSLVERR sampled before completion edge
```

A COUNTER read while enabled can observe a value that changes at the transfer-completion edge. The C++ master therefore samples `PRDATA` before clocking that edge, while the reference model updates state on the edge.

Back-to-back transfers move directly from one access phase to the next transfer's setup phase without an idle clock edge.
