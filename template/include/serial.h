#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>

void serial_init(uint32_t baudrate);
void serial_put(uint8_t c);
void serial_transmit(const uint8_t *data, uint32_t length);
void serial_print(const char *str);

void serial_print_hex32(uint32_t v);
void serial_print_hex16(uint16_t v);
void serial_print_hex8(uint8_t v);
void serial_print_bin32(uint32_t v);
void serial_print_bin16(uint16_t v);
void serial_print_bin8(uint8_t v);
void serial_print_dec(int32_t v);

#endif