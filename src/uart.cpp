#include "data_type.hpp"

#include <msp430g2553.h>
#include <stdio.h>

void parse(const char string[]);
void ad9851_reset_serial();
void ad9851_wr_serial(u8 w0, f64 frequency);

u32 frequency = 200;
u16 amplitude = 1000;
u16 upper_limit = 2000;
char rx_buffer[16] = {0};
char tx_buffer[32] = {0};
char const usage_helper[] =
    "+------------------------------------------------------------------------------------+\n\
     | -->> [S/G][F/A/L][G: Value]                                                        |\n\
     | Arg[S/G]:                                                                          |\n\
     |     S: set new value to var.                                                       |\n\
     |     G: get the var's value.                                                        |\n\
     | Arg[F/A/L]:                                                                        |\n\
     |     F: choose var -> frequency.                                                    |\n\
     |     A: choose var -> amplitude.                                                    |\n\
     |     L: choose var -> upper_limit.                                                  |\n\
     | Arg[G: Value]:                                                                     |\n\
     |     if you type 'G' in first arg, there need to input a integer as the new value   |\n\
     +------------------------------------------------------------------------------------+";

void uart_send_string(const char* buffer) {
    while (*buffer != '\0') {
        UCA0TXBUF = *buffer++;
        while (UCA0STAT & UCBUSY) {
            ;
        }
    }
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void uart_receive_isr() {
    static u8 i = 0;

    if (IFG2 & UCA0RXIFG) {
        IFG2 &= ~UCA0RXIFG;
        rx_buffer[i++] = UCA0RXBUF;
        if (rx_buffer[i - 1] == ';') {
            i = 0;
            parse(rx_buffer);
        }
    }
}

void hello() {
    uart_send_string("Start successfully! Here is the usage of our system:\n     ");
    uart_send_string(usage_helper);
}

u32 extract_value(const char* s) {
    u32 result = 0;

    while (*s != '\0') {
        if ('0' <= *s && *s <= '9') {
            result = result * 10 + *s - '0';
        }
        s++;
    }

    return result;
}

void update_frequency(const char string[]) {
    u32 new_frequency = extract_value(string);
    if (100 <= new_frequency && new_frequency <= 100000) {
        frequency = new_frequency;
    }

    ad9851_reset_serial();
    ad9851_wr_serial(0, frequency * 1.0);
}

void update_amplitude() {}

void update_upper_limit() {}

void parse(const char string[]) {
    switch (string[0]) {
        case 'S': {
            switch (string[1]) {
                case 'F': {
                    update_frequency(string);
                    break;
                }
                case 'A': {
                    update_amplitude();
                    break;
                }
                case 'L': {
                    update_upper_limit();
                    break;
                }
                default: {
                    uart_send_string("Error format! You should follow this format:\n     ");
                    uart_send_string(usage_helper);
                    break;
                }
            }
            break;
        }
        case 'G': {
            switch (string[1]) {
                case 'F': {
                    sprintf(tx_buffer, "Current frequency is %d Hz", frequency);
                    uart_send_string(tx_buffer);
                    break;
                }
                case 'A': {
                    sprintf(tx_buffer, "Current amplitude is %d mV", amplitude);
                    uart_send_string(tx_buffer);
                    break;
                }
                case 'L': {
                    sprintf(tx_buffer, "Current upper limit is %d Hz", upper_limit);
                    uart_send_string(tx_buffer);
                    break;
                }
                default: {
                    uart_send_string("Error format! You should follow this format:\n     ");
                    uart_send_string(usage_helper);
                    break;
                }
            }
            break;
        }
        default: {
            uart_send_string("Error format! You should follow this format:\n     ");
            uart_send_string(usage_helper);
            break;
        }
    }
}
