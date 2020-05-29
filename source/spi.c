
#include <string.h>

#include "spi.h"
#include "stm32f3xx_ll_spi.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_bus.h"

#include "serial.h"

void spi_init(void)
{
	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_GPIO_InitTypeDef gpios;
	LL_GPIO_StructInit(&gpios);
	gpios.Pin = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_7;
	gpios.Mode = LL_GPIO_MODE_ALTERNATE;
	gpios.Alternate = LL_GPIO_AF_5;
	gpios.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpios.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &gpios);

	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1);
	LL_SPI_InitTypeDef spi;
	LL_SPI_StructInit(&spi);
	spi.TransferDirection = LL_SPI_FULL_DUPLEX;
	spi.Mode = LL_SPI_MODE_MASTER;
	spi.DataWidth = LL_SPI_DATAWIDTH_8BIT;
	spi.ClockPolarity = LL_SPI_POLARITY_HIGH;
	spi.ClockPhase = LL_SPI_PHASE_2EDGE;
	spi.NSS = LL_SPI_NSS_SOFT;
	spi.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256;
	spi.BitOrder = LL_SPI_MSB_FIRST;
	spi.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
	LL_SPI_Init(SPI1, &spi);
	LL_SPI_SetRxFIFOThreshold(SPI1, LL_SPI_RX_FIFO_TH_QUARTER);
	LL_SPI_Enable(SPI1);
}

void spi_read_write(const uint8_t *tx, uint8_t *rx, uint32_t len)
{
	LL_SPI_ReceiveData8(SPI1);

	for(uint32_t i=0;i<len;i++)
	{
		LL_SPI_TransmitData8(SPI1, tx[i]);

		while(!LL_SPI_IsActiveFlag_RXNE(SPI1))
		{
		}
		rx[i] = LL_SPI_ReceiveData8(SPI1);
	}
}

void spi_write_reg(uint8_t reg, uint8_t value)
{
	uint8_t tx[2] = { reg, value};
	uint8_t rx[2];

	spi_read_write(tx, rx, 2);
}

void spi_read_reg(uint8_t reg, uint8_t *value)
{
	uint8_t tx[2] = { 0x80 | reg, 0xFF};
	uint8_t rx[2];

	spi_read_write(tx, rx, 2);

	*value = rx[1];
}

void spi_write_regs(uint8_t reg, const uint8_t *values, uint32_t len)
{
	uint8_t tx[1 + len];
	uint8_t rx[1 + len];

	tx[0] = 0x40 | reg;
	memcpy(&tx[1], values, len);

	spi_read_write(tx, rx, 1 + len);
}

void spi_read_regs(uint8_t reg, uint8_t *values, uint32_t len)
{
	uint8_t tx[1 + len];
	uint8_t rx[1 + len];

	tx[0] = 0xC0 | reg;

	spi_read_write(tx, rx, 1 + len);

	memcpy(values, &rx[1], len);
}
