`default_nettype none

module apb_timer_sva (
    input logic                  PCLK,
    input logic                  PRESETn,
    input logic                  PSEL,
    input logic                  PENABLE,
    input logic                  PWRITE,
    input logic [11:0]           PADDR,
    input logic [31:0]           PWDATA,
    input logic [3:0]            PSTRB,
    input logic [2:0]            PPROT,
    input logic [31:0]           PRDATA,
    input logic                  PREADY,
    input logic                  PSLVERR,
    input logic                  irq_o
);

`ifdef APB_TIMER_ENABLE_SVA
    logic                       previous_cycle_valid;
    logic                       previous_wait_state;
    logic [11:0]                previous_PADDR;
    logic                       previous_PWRITE;
    logic [31:0]                previous_PWDATA;
    logic [3:0]                 previous_PSTRB;
    logic [2:0]                 previous_PPROT;

    /*
     * Clocked immediate assertions are used instead of concurrent
     * disable-iff properties. This preserves the protocol checks while
     * avoiding a Verilator 5.050 bound-SVA code-generation failure.
     */
    always_ff @(posedge PCLK or negedge PRESETn) begin
        if (!PRESETn) begin
            previous_cycle_valid <= 1'b0;
            previous_wait_state  <= 1'b0;
            previous_PADDR       <= '0;
            previous_PWRITE      <= 1'b0;
            previous_PWDATA      <= '0;
            previous_PSTRB       <= '0;
            previous_PPROT       <= '0;
        end else begin
            apb_access_requires_select:
                assert (!PENABLE || PSEL)
                else $error(
                    "APB protocol violation: PENABLE asserted without PSEL"
                );

            ready_is_constant:
                assert (PREADY === 1'b1)
                else $error(
                    "APB timer must remain a zero-wait-state slave"
                );

            error_only_in_access:
                assert (!PSLVERR || (PSEL && PENABLE))
                else $error(
                    "PSLVERR asserted outside an APB access phase"
                );

            if (previous_cycle_valid && previous_wait_state) begin
                apb_control_stable_during_wait:
                    assert (
                        PADDR  == previous_PADDR  &&
                        PWRITE == previous_PWRITE &&
                        PWDATA == previous_PWDATA &&
                        PSTRB  == previous_PSTRB  &&
                        PPROT  == previous_PPROT
                    )
                    else $error(
                        "APB control changed while the slave was waiting"
                    );
            end

            irq_known:
                assert (!$isunknown(irq_o))
                else $error("irq_o contains an unknown value");

            if (
                PSEL &&
                PENABLE &&
                !PWRITE &&
                PREADY &&
                !PSLVERR
            ) begin
                read_data_known_on_valid_read:
                    assert (!$isunknown(PRDATA))
                    else $error(
                        "PRDATA contains an unknown value during a valid read"
                    );
            end

            previous_cycle_valid <= 1'b1;
            previous_wait_state  <= PSEL && PENABLE && !PREADY;
            previous_PADDR       <= PADDR;
            previous_PWRITE      <= PWRITE;
            previous_PWDATA      <= PWDATA;
            previous_PSTRB       <= PSTRB;
            previous_PPROT       <= PPROT;
        end
    end
`endif

endmodule

`default_nettype wire

`ifdef APB_TIMER_ENABLE_SVA
bind apb_timer apb_timer_sva apb_timer_sva_i (.*);
`endif
