/* Header file for irq based tx UART0 Driver*/
/* Robert Merrill */

#ifndef UART0_TXIRQ_H
#define UART0_TXIRQ_H

#include <stdbool.h>
#include <stdint.h>

void Configure_Uart0_Txirq();
_Bool Send_Data_To_Uart0_Irq(uint8_t data);
_Bool Recieve_Data_From_Uart0( uint8_t *data);

#endif
