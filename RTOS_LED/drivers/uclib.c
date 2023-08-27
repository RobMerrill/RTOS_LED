/* Implementation of extra functions for main program */
/* Robert Merrill */

#include <stdint.h>
#include "uclib.h"
#include "uart0_txirq.h"

void delay(uint32_t some_time)
    {
    volatile uint32_t counter;
    uint32_t delay_time = some_time * 1000000;
    for (counter = 0; counter < delay_time; counter++);
    }
    
uint16_t to_hex_ascii8(uint8_t data)
    {
    uint16_t retval;
    retval = to_hex_ascii(data);
    retval = retval | (to_hex_ascii(data >> 4) << 8);
    return retval;
    }

char to_hex_ascii(char data)
    {
        return ((data & 0x0f)) < 10 ? ((data & 0x0f) + '0') : ((data & 0x0f) + 'a' - 10);
    }

void print_hex(uint8_t data)
    {
    Send_Data_To_Uart0_Irq (to_hex_ascii8(data));
    }

void print_hex8(uint8_t data)
    {
    Send_Data_To_Uart0_Irq (to_hex_ascii8(data) >> 8);
    Send_Data_To_Uart0_Irq (to_hex_ascii8(data));
    }
    
void print_string(char str[], uint8_t length)
    {
    uint8_t i;
    for (i = 0; i < length+1; i++)
        {
        Send_Data_To_Uart0_Irq(str[i]);
        }
    }
