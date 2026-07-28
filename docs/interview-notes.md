# Interview Notes

## Two-minute explanation

I built a 32-bit APB4 timer peripheral to demonstrate an end-to-end SoC hardware workflow. The RTL implements a writable counter, programmable compare value, sticky match status, maskable level interrupt, partial byte writes, and deterministic invalid-address errors. I froze ambiguous behaviours before coding, especially whether writes pause counting, whether the old or new compare value applies on a write edge, and what happens when software clears status on the same edge as a hardware match.

For verification, I created a C++17 environment around Verilator. An APB master drives legal setup and access phases, an independent cycle-accurate reference model predicts state, and a scoreboard checks read data, errors, ready, and IRQ. The regression contains 29 directed tests plus deterministic randomized sequences, assertions, semantic coverage, failure seeds, and non-zero exit codes. The same register JSON generates the RTL and C++ constants to prevent interface drift.

I then synthesize with Yosys and use a pinned OpenLane/OpenROAD flow targeting 100 MHz in SKY130. I only claim timing, area, DRC, and LVS values after preserving the real reports.

## Likely questions

### Why APB rather than AXI?

The timer is a low-bandwidth control peripheral. APB exercises realistic SoC register timing without adding unrelated AXI channel, burst, and ordering complexity.

### Why is COUNTER writable?

It supports firmware initialization and makes rollover and boundary verification deterministic instead of requiring billions of cycles.

### Why does compare use `counter + 1`?

The event is defined when the counter enters the programmed value. This removes ambiguity about whether status appears one cycle before or after the visible threshold.

### What happens when clear and match coincide?

Set dominates clear: `(pending && !clear) || match`. This avoids losing a real hardware event.

### Is this UVM?

No. It is a reusable object-oriented C++ verification environment using transaction, driver, reference-model, scoreboard, assertion, and coverage concepts. Claiming UVM would be inaccurate.

### What would you extend next?

Add a prescaler and periodic auto-reload, connect the block through an APB interconnect to my RISC-V core, and run a firmware-level interrupt demonstration on FPGA.
