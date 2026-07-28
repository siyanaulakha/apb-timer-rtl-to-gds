`timescale 1ns/1ps
`default_nettype none

module apb_timer (
    input  logic                  PCLK,
    input  logic                  PRESETn,
    input  logic                  PSEL,
    input  logic                  PENABLE,
    input  logic                  PWRITE,
    input  logic [11:0] PADDR,
    input  logic [31:0] PWDATA,
    input  logic [3:0]  PSTRB,
    /* verilator lint_off UNUSEDSIGNAL */
    input  logic [2:0]  PPROT,
    /* verilator lint_on UNUSEDSIGNAL */
    output logic [31:0] PRDATA,
    output logic                  PREADY,
    output logic                  PSLVERR,
    output logic                  irq_o
);

    `include "generated/apb_timer_regs.svh"

    logic        timer_enable_q;
    logic        irq_enable_q;
    logic [31:0] counter_q;
    logic [31:0] compare_q;
    logic        match_pending_q;

    logic transfer;
    logic write_transfer;
    logic address_aligned;
    logic address_valid;
    logic control_write;
    logic counter_write;
    logic compare_write;
    logic status_clear;
    logic automatic_increment;
    logic match_event;
    logic [31:0] incremented_counter;
    logic [31:0] merged_counter;
    logic [31:0] merged_compare;

    function automatic logic [31:0] merge_bytes(
        input logic [31:0] old_value,
        input logic [31:0] new_value,
        input logic [3:0]  strobes
    );
        logic [31:0] result;
        int unsigned byte_index;
        begin
            result = old_value;
            for (byte_index = 0; byte_index < 4; byte_index++) begin
                if (strobes[byte_index]) begin
                    result[byte_index*8 +: 8] = new_value[byte_index*8 +: 8];
                end
            end
            merge_bytes = result;
        end
    endfunction

    assign PREADY = 1'b1;
    assign transfer = PSEL && PENABLE && PREADY;
    assign write_transfer = transfer && PWRITE;
    assign address_aligned = (PADDR[1:0] == 2'b00);

    always_comb begin
        case (PADDR)
            APB_TIMER_CONTROL_ADDR,
            APB_TIMER_COUNTER_ADDR,
            APB_TIMER_COMPARE_ADDR,
            APB_TIMER_STATUS_ADDR: address_valid = address_aligned;
            default:               address_valid = 1'b0;
        endcase
    end

    assign PSLVERR = transfer && !address_valid;

    assign control_write = write_transfer && address_valid &&
                           (PADDR == APB_TIMER_CONTROL_ADDR) && PSTRB[0];
    assign counter_write = write_transfer && address_valid &&
                           (PADDR == APB_TIMER_COUNTER_ADDR) && (|PSTRB);
    assign compare_write = write_transfer && address_valid &&
                           (PADDR == APB_TIMER_COMPARE_ADDR) && (|PSTRB);
    assign status_clear = write_transfer && address_valid &&
                          (PADDR == APB_TIMER_STATUS_ADDR) &&
                          PSTRB[0] && PWDATA[0];

    assign automatic_increment = timer_enable_q && !control_write && !counter_write;
    assign incremented_counter = counter_q + 32'd1;
    assign match_event = automatic_increment && (incremented_counter == compare_q);
    assign merged_counter = merge_bytes(counter_q, PWDATA, PSTRB);
    assign merged_compare = merge_bytes(compare_q, PWDATA, PSTRB);

    always_comb begin
        PRDATA = 32'h0000_0000;
        if (address_aligned) begin
            case (PADDR)
                APB_TIMER_CONTROL_ADDR: PRDATA = {30'd0, irq_enable_q, timer_enable_q};
                APB_TIMER_COUNTER_ADDR: PRDATA = counter_q;
                APB_TIMER_COMPARE_ADDR: PRDATA = compare_q;
                APB_TIMER_STATUS_ADDR:  PRDATA = {30'd0, irq_o, match_pending_q};
                default:                PRDATA = 32'h0000_0000;
            endcase
        end
    end

    assign irq_o = match_pending_q && irq_enable_q;

    always_ff @(posedge PCLK or negedge PRESETn) begin
        if (!PRESETn) begin
            timer_enable_q  <= 1'b0;
            irq_enable_q    <= 1'b0;
            counter_q       <= 32'h0000_0000;
            compare_q       <= 32'hFFFF_FFFF;
            match_pending_q <= 1'b0;
        end else begin
            if (control_write) begin
                timer_enable_q <= PWDATA[0];
                irq_enable_q   <= PWDATA[1];
            end

            if (counter_write) begin
                counter_q <= merged_counter;
            end else if (automatic_increment) begin
                counter_q <= incremented_counter;
            end

            if (compare_write) begin
                compare_q <= merged_compare;
            end

            match_pending_q <= (match_pending_q && !status_clear) || match_event;
        end
    end

endmodule

`default_nettype wire
