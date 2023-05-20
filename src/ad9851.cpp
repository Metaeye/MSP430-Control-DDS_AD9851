#include "data_type.hpp"

#include <msp430g2553.h>

//----------------AD9851 IO口定义----------------//
#define ad9851_w_clk    BIT2 // P2.2口接AD9851的w_clk脚/PIN7
#define ad9851_fq_up    BIT1 // P2.1口接AD9851的fq_up脚/PIN8
#define ad9851_rest     BIT0 // P2.0口接AD9851的rest脚/PIN12
#define ad9851_bit_data BIT5 // P1.5口接AD9851的D7脚/PIN25

void ad9851_reset_serial() {
    P2OUT &= ~ad9851_w_clk;
    P2OUT &= ~ad9851_fq_up;

    P2OUT &= ~ad9851_rest;
    P2OUT |= ad9851_rest;
    P2OUT &= ~ad9851_rest;

    P2OUT &= ~ad9851_w_clk;
    P2OUT |= ad9851_w_clk;
    P2OUT &= ~ad9851_w_clk;

    P2OUT &= ~ad9851_fq_up;
    P2OUT |= ad9851_fq_up;
    P2OUT &= ~ad9851_fq_up;
}

void ad9851_wr_serial(u8 w0, f64 frequency) {

    u32 frequency_u32 = 4294.967296 * frequency / 125 * 4.1668;

    u8 w = frequency_u32;
    for (u8 i = 0; i < 8; i++) {
        P1OUT = ((w >> i) & 0x01) ? P1OUT | ad9851_bit_data : P1OUT & ~ad9851_bit_data;
        P2OUT |= ad9851_w_clk;
        P2OUT &= ~ad9851_w_clk;
    }

    w = frequency_u32 >> 8;
    for (u8 i = 0; i < 8; i++) {
        P1OUT = ((w >> i) & 0x01) ? P1OUT | ad9851_bit_data : P1OUT & ~ad9851_bit_data;
        P2OUT |= ad9851_w_clk;
        P2OUT &= ~ad9851_w_clk;
    }

    w = frequency_u32 >> 16;
    for (u8 i = 0; i < 8; i++) {
        P1OUT = ((w >> i) & 0x01) ? P1OUT | ad9851_bit_data : P1OUT & ~ad9851_bit_data;
        P2OUT |= ad9851_w_clk;
        P2OUT &= ~ad9851_w_clk;
    }

    w = frequency_u32 >> 24;
    for (u8 i = 0; i < 8; i++) {
        P1OUT = ((w >> i) & 0x01) ? P1OUT | ad9851_bit_data : P1OUT & ~ad9851_bit_data;
        P2OUT |= ad9851_w_clk;
        P2OUT &= ~ad9851_w_clk;
    }

    w = w0;
    for (u8 i = 0; i < 8; i++) {
        P1OUT = ((w0 >> i) & 0x01) ? P1OUT | ad9851_bit_data : P1OUT & ~ad9851_bit_data;
        P2OUT |= ad9851_w_clk;
        P2OUT &= ~ad9851_w_clk;
    }

    // 移入始能
    P2OUT |= ad9851_fq_up;
    P2OUT &= ~ad9851_fq_up;
}
