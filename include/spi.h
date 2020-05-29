#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

void spi_init(void);
void spi_read_write(const uint8_t *tx, uint8_t *rx, uint32_t len);

#endif