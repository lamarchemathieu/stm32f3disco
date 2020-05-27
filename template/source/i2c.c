
#include "i2c.h"
#include "stm32f3xx_ll_i2c.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_bus.h"

#include "serial.h"

void i2c_init(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
	LL_GPIO_InitTypeDef gpios;
	LL_GPIO_StructInit(&gpios);
	gpios.Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
	gpios.Mode = LL_GPIO_MODE_ALTERNATE;
	gpios.Pull = LL_GPIO_PULL_NO;
	gpios.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
	gpios.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	gpios.Alternate = LL_GPIO_AF_4;
	LL_GPIO_Init(GPIOB, &gpios);

	LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_SYSCLK);
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);
	LL_I2C_InitTypeDef i2c;
	LL_I2C_StructInit(&i2c);
	i2c.PeripheralMode = LL_I2C_MODE_I2C;
	i2c.Timing = __LL_I2C_CONVERT_TIMINGS(5, 5, 5, 100, 100);
	i2c.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
	i2c.DigitalFilter = 0;
	i2c.OwnAddress1 = 0x00;
	i2c.TypeAcknowledge = LL_I2C_ACK;
	i2c.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
	LL_I2C_Disable(I2C1);
	LL_I2C_Init(I2C1, &i2c);
	LL_I2C_Enable(I2C1);
}

uint32_t i2c_detect(uint8_t addr)
{
	uint32_t ret = 0;

	LL_I2C_HandleTransfer(I2C1, addr << 1, LL_I2C_ADDRSLAVE_7BIT, 1, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(!LL_I2C_IsActiveFlag_STOP(I2C1))
	{
		if (LL_I2C_IsActiveFlag_RXNE(I2C1))
		{
			uint8_t c = LL_I2C_ReceiveData8(I2C1);
			(void)c;
			ret = 1;
		}
	}

	LL_I2C_ClearFlag_STOP(I2C1);

	return ret;
}

uint32_t i2c_write(uint8_t addr, const uint8_t *data, uint8_t length)
{
	uint32_t i = 0;

	LL_I2C_HandleTransfer(I2C1, addr << 1, LL_I2C_ADDRSLAVE_7BIT, length, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_WRITE);

	while(!LL_I2C_IsActiveFlag_STOP(I2C1))
	{
		if (LL_I2C_IsActiveFlag_TXIS(I2C1))
		{
			LL_I2C_TransmitData8(I2C1, data[i++]);
		}
	}

	LL_I2C_ClearFlag_STOP(I2C1);	

	if (i == length)
		return 1;
	return 0;
}

uint32_t i2c_read(uint8_t addr, uint8_t *data, uint8_t length)
{
	uint32_t i = 0;

	LL_I2C_HandleTransfer(I2C1, addr << 1, LL_I2C_ADDRSLAVE_7BIT, length, LL_I2C_MODE_AUTOEND, LL_I2C_GENERATE_START_READ);

	while(!LL_I2C_IsActiveFlag_STOP(I2C1))
	{
		if (LL_I2C_IsActiveFlag_RXNE(I2C1))
		{
			data[i++] = LL_I2C_ReceiveData8(I2C1);
		}
	}

	LL_I2C_ClearFlag_STOP(I2C1);	

	if (i == length)
		return 1;
	return 0;
}

uint32_t i2c_read_reg(uint8_t addr, uint8_t reg, uint8_t *v)
{
	uint8_t buf[1] = {reg};

	if (!i2c_write(addr, buf, 1))
		return 0;

	if (!i2c_read(addr, v, 1))
		return 0;

	return 1;
}

uint32_t i2c_write_reg(uint8_t addr, uint8_t reg, uint8_t v)
{
	uint8_t buf[2] = {reg, v};

	if (!i2c_write(addr, buf, 2))
		return 0;

	return 1;

}
