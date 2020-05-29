#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

void spi_init(void);
void spi_read_write(const uint8_t *tx, uint8_t *rx, uint32_t len);

void spi_write_reg(uint8_t reg, uint8_t value);
void spi_read_reg(uint8_t reg, uint8_t *value);


#endif