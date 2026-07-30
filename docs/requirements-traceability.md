# Requirements Traceability

| Requirement | Description | Tests |
|---|---|---|
| RST-01 | Defined asynchronous reset state | T01, T28 |
| APB-01 | Legal two-phase APB transfers and setup-to-access stability | T02–T30, assertions |
| APB-02 | Constant ready response | All bus tests, assertion |
| APB-03 | Invalid and misaligned accesses error | T23, T24, T30 |
| APB-04 | Back-to-back transfers supported | T22 |
| REG-01 | CONTROL R/W and reserved-zero behaviour | T02, T03 |
| REG-02 | COUNTER full and partial writes | T06, T07 |
| REG-03 | COMPARE full and partial writes | T04, T05 |
| REG-04 | STATUS sticky W1C behaviour | T12, T17, T18, T29 |
| TMR-01 | Disabled counter holds | T08 |
| TMR-02 | Enabled counter increments modulo 32 bits | T09, T20 |
| TMR-03 | CONTROL and COUNTER writes suppress increment | T10, T11 |
| TMR-04 | Compare/status writes do not suppress increment | T18, T27 |
| TMR-05 | Match occurs on entering compare value | T12–T14 |
| TMR-06 | Compare zero matches rollover | T21 |
| TMR-07 | New compare active after write edge | T26, T27 |
| IRQ-01 | IRQ is pending AND enable | T13–T16 |
| IRQ-02 | Hardware set dominates clear | T18 |
| VER-01 | Random failures replay from seed | T30 |
