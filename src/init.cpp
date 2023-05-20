#include "data_type.hpp"

#include <msp430g2553.h>

void ad9851_reset_serial();
void ad9851_wr_serial(u8 w0, f64 frequency);

extern u16 frequency;

void init_system_clock() {
    /*配置DCO为1MHz*/
    DCOCTL = CALDCO_1MHZ;
    BCSCTL1 = CALBC1_1MHZ;
    /*配置SMCLK的时钟源为DCO*/
    BCSCTL2 &= ~SELS;
    /*SMCLK的分频系数置为1*/
    BCSCTL2 &= ~(DIVS0 | DIVS1);
}

void init_uart() {
    /*复位USCI_Ax*/
    UCA0CTL1 |= UCSWRST;

    /*选择USCI_Ax为UART模式*/
    UCA0CTL0 &= ~UCSYNC;

    /*配置UART时钟源为SMCLK*/
    UCA0CTL1 |= UCSSEL1;

    /*配置波特率为9600@1MHz*/
    UCA0BR0 = 0x68;
    UCA0BR1 = 0x00;
    UCA0MCTL = 0x01 << 1;
    /*使能端口复用*/
    P1SEL |= BIT1 + BIT2;
    P1SEL2 |= BIT1 + BIT2;

    /*清除复位位，使能UART*/
    UCA0CTL1 &= ~UCSWRST;

    // 接收中断启用
    IE2 |= UCA0RXIE;
    // 清空接收中断标志
    IFG2 &= ~UCA0RXIFG;
}

void init_system() {
    // 停用看门狗定时器
    WDTCTL = WDTPW + WDTHOLD;
    P1DIR |= BIT5;
    P2DIR |= (BIT0 + BIT1 + BIT2);

    init_system_clock();
    init_uart();

    __bis_SR_register(GIE);

    ad9851_reset_serial();
    ad9851_wr_serial(0, frequency);
}
