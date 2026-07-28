# APB Timer Specification

## 1. Purpose

`apb_timer` is a single-channel, 32-bit programmable timer exposed as a four-register APB4 slave. It targets low-bandwidth SoC control paths and demonstrates deterministic bus and interrupt behaviour.

## 2. Interface

| Signal | Direction | Width | Meaning |
|---|---|---:|---|
| `PCLK` | input | 1 | APB and timer clock |
| `PRESETn` | input | 1 | Active-low asynchronous reset assertion |
| `PSEL` | input | 1 | Peripheral select |
| `PENABLE` | input | 1 | APB access phase |
| `PWRITE` | input | 1 | Write when high, read when low |
| `PADDR` | input | 12 | Byte address within a 4 KiB aperture |
| `PWDATA` | input | 32 | Write data |
| `PSTRB` | input | 4 | One write strobe per byte |
| `PPROT` | input | 3 | Accepted but not policy-enforced |
| `PRDATA` | output | 32 | Read data |
| `PREADY` | output | 1 | Permanently high |
| `PSLVERR` | output | 1 | Invalid or misaligned access error |
| `irq_o` | output | 1 | Active-high level interrupt |

A transfer completes on a rising `PCLK` edge for which `PSEL && PENABLE && PREADY` is true. Because `PREADY=1`, every legal access has one setup cycle and one access cycle.

## 3. Addressing

Valid word-aligned offsets are `0x000`, `0x004`, `0x008`, and `0x00C`. `PADDR[1:0]` must be zero. Invalid or misaligned accesses assert `PSLVERR` only during the access phase. Invalid reads return zero; invalid writes have no register side effects. Timer counting is not paused by an invalid transfer.

## 4. Registers

### CONTROL (`0x000`)

- Bit 0 `TIMER_ENABLE`: enables automatic increment.
- Bit 1 `IRQ_ENABLE`: masks the interrupt output without clearing pending state.
- Bits 31:2 read as zero and ignore writes.
- Only `PSTRB[0]` can modify CONTROL.

### COUNTER (`0x004`)

A 32-bit readable and writable counter. Byte strobes merge write data with the old value. An effective write suppresses automatic increment on the same edge and cannot directly generate a compare event.

### COMPARE (`0x008`)

A 32-bit byte-writable compare threshold. Reset value is `0xFFFFFFFF`. A write does not pause counting and the write-completion edge compares against the old COMPARE value. The new value is active on the following edge.

### STATUS (`0x00C`)

- Bit 0 `MATCH_PENDING`: sticky, read/write-one-to-clear.
- Bit 1 `IRQ_ACTIVE`: read-only mirror of `irq_o`.
- Bits 31:2 read zero and ignore writes.

A clear request requires a valid STATUS write with `PSTRB[0]=1` and `PWDATA[0]=1`.

## 5. Counter and event behaviour

When enabled and not suppressed by an effective CONTROL or COUNTER write:

```text
next_counter = counter + 1 modulo 2^32
```

A compare event occurs when the incremented value equals the current compare value:

```text
match_event = TIMER_ENABLE && increment_occurs && (counter + 1 == COMPARE)
```

`COMPARE=0` is legal and matches when the counter rolls from `0xFFFFFFFF` to zero. Writing the counter or compare register does not itself generate a match.

## 6. Interrupt behaviour

```text
irq_o = MATCH_PENDING && IRQ_ENABLE
```

Pending status remains set until reset or W1C. Disabling IRQ only masks the output. Enabling IRQ while status is pending asserts the output immediately after the CONTROL write.

## 7. Event priority

Reset has highest priority. Per register:

1. Effective CONTROL write updates enables and suppresses increment.
2. Effective COUNTER write loads the merged value and suppresses increment.
3. COMPARE write updates compare but does not suppress increment; old compare is used on that edge.
4. STATUS W1C does not suppress increment.
5. Hardware match dominates software clear:

```text
next_pending = (pending && !clear_request) || match_event
```

## 8. Reset

`PRESETn` asynchronously asserts reset. System integration must deassert it synchronously to `PCLK`.

| State | Reset value |
|---|---:|
| Timer enable | 0 |
| IRQ enable | 0 |
| Counter | `0x00000000` |
| Compare | `0xFFFFFFFF` |
| Match pending | 0 |
| IRQ | 0 |
