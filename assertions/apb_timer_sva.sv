`default_nettype none

module apb_timer_sva (
    input logic                  PCLK,
    input logic                  PRESETn,
    input logic                  PSEL,
    input logic                  PENABLE,
    input logic                  PWRITE,
    input logic [11:0] PADDR,
    input logic [31:0]           PWDATA,
    input logic [3:0]            PSTRB,
    input logic [2:0]            PPROT,
    input logic [31:0]           PRDATA,
    input logic                  PREADY,
    input logic                  PSLVERR,
    input logic                  irq_o
);

`ifdef APB_TIMER_ENABLE_SVA
    default clocking cb @(posedge PCLK); endclocking
    default disable iff (!PRESETn);

    apb_access_requires_select: assert property (PENABLE |-> PSEL);
    ready_is_constant:          assert property (PREADY == 1'b1);
    error_only_in_access:       assert property (PSLVERR |-> (PSEL && PENABLE));

    apb_control_stable_during_wait: assert property (
        PSEL && PENABLE && !PREADY |=>
        $stable({PADDR, PWRITE, PWDATA, PSTRB, PPROT})
    );

    irq_known: assert property (!$isunknown(irq_o));
    read_data_known_on_valid_read: assert property (
        PSEL && PENABLE && !PWRITE && PREADY && !PSLVERR |-> !$isunknown(PRDATA)
    );
`endif

endmodule

`default_nettype wire

`ifdef APB_TIMER_ENABLE_SVA
bind apb_timer apb_timer_sva apb_timer_sva_i (.*);
`endif
