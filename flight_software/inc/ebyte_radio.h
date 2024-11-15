#ifndef EBYTE_RADIO_H
#define EBYTE_RADIO_H

void radio_uart_init(void);
void radio_print_debug(char buff[]);
INTERRUPT_HANDLER(UART1_RXNE_IRQHandler, ITC_IRQ_UART1_RX);
void send_telemetry(void);



#endif // EBYTE_RADIO_H