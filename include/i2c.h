#ifndef _I2C_H_
#define _I2C_H_

#include <stdint.h>

void i2c_init(void);
uint32_t i2c_detect(uint8_t addr);
uint32_t i2c_write(uint8_t addr, const uint8_t *data, uint8_t length);
uint32_t i2c_read(uint8_t addr, uint8_t *data, uint8_t length);

uint32_t i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *v);
uint32_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t v);

#endif