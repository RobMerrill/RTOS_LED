/* Header file for uclib functions */
/* Robert merrill */
#ifndef UCLIB_H
#define UCLIB_H

#include <stdint.h>

void delay(uint32_t);
uint16_t to_hex_ascii8(uint8_t);
char to_hex_ascii(char);
void print_hex(uint8_t);
void print_hex8(uint8_t);
void print_string(char [], uint8_t);

#endif
