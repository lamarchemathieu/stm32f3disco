
#include <stdio.h>

#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_utils.h"
#include "stm32f3xx_ll_usart.h"
#include "utils.h"
#include "serial.h"
#include "i2c.h"

#define ACC_ADDR (0x19)
#define MAG_ADDR (0x1E)

#define ABS(a) (((a) > 0) ? (a) : (-(a)))

void SystemClock_Config(void);

typedef struct 
{
	int16_t x;
	int16_t y;
	int16_t z;
} acc_t;

typedef struct 
{
	int16_t x;
	int16_t y;
	int16_t z;
	int16_t t;
} mag_t;

uint32_t acc_get(acc_t *acc);
uint32_t mag_get(mag_t *mag);

int main(void)
{
	LL_GPIO_InitTypeDef gpios;

	SystemClock_Config();

	tick_init();

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
	LL_GPIO_StructInit(&gpios);
	gpios.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	gpios.Mode = LL_GPIO_MODE_OUTPUT;
	gpios.Pull = LL_GPIO_PULL_NO;
	gpios.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpios.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOE, &gpios);

	serial_init();
	i2c_init();

	serial_print("\r\nHello world !\r\n\r\n");

	LL_RCC_ClocksTypeDef rcc_clocks;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);

	serial_print("SYSCLK_Frequency = ");
	serial_print_dec(rcc_clocks.SYSCLK_Frequency);
	serial_print("\r\n");
	serial_print("HCLK_Frequency  = ");
	serial_print_dec(rcc_clocks.HCLK_Frequency);
	serial_print("\r\n");
	serial_print("PCLK1_Frequency = ");
	serial_print_dec(rcc_clocks.PCLK1_Frequency);
	serial_print("\r\n");
	serial_print("PCLK2_Frequency = ");
	serial_print_dec(rcc_clocks.PCLK2_Frequency);
	serial_print("\r\n\r\n");

	
	serial_print("I2C detection start :\r\n");
	uint8_t addr = 0;
	for(addr = 8; addr < 120; addr++)
	{
		if (i2c_detect(addr))
		{
			serial_print(" - I2C at ");
			serial_print_hex8(addr);
			serial_print("\r\n");
		}
	}
	serial_print("I2C detection end.\r\n\r\n");
	
	uint8_t ira, irb, irc;

	if (i2c_read_reg(MAG_ADDR, 0x0A, &ira))
	{
		serial_print("IRA = ");
		serial_print_hex8(ira);
		serial_print("\r\n");
	}

	if (i2c_read_reg(MAG_ADDR, 0x0B, &irb))
	{
		serial_print("IRB = ");
		serial_print_hex8(irb);
		serial_print("\r\n");
	}

	if (i2c_read_reg(MAG_ADDR, 0x0C, &irc))
	{
		serial_print("IRC = ");
		serial_print_hex8(irc);
		serial_print("\r\n\r\n");
	}

	i2c_write_reg(ACC_ADDR, 0x20, 0x97);//Normal, ODR = 1.344kHz, X,Y,Z En
	i2c_write_reg(ACC_ADDR, 0x23, 0x00);//Continuous, LSB first, FS = +/-2g


	i2c_write_reg(MAG_ADDR, 0x00, 0x9C);// temp_en, DO = 220Hz
	i2c_write_reg(MAG_ADDR, 0x01, 0xE0);//+/-8.1 Gauss
	i2c_write_reg(MAG_ADDR, 0x02, 0x00);//Continuous mode


	uint64_t last = tick_get();

	while (1)
	{
		uint64_t now = tick_get();

		if (now - last >= 10)
		{
			last = now;

			acc_t a;
			if (acc_get(&a))
			{
				uint32_t pins = 0;

				if ((a.x > 5700) && (a.y > 5700))
					pins |= LL_GPIO_PIN_8;
				else if ((a.x > 5700) && (a.y < -5700))
					pins |= LL_GPIO_PIN_10;
				else if ((a.x < -5700) && (a.y > 5700))
					pins |= LL_GPIO_PIN_14;
				else if ((a.x < -5700) && (a.y < -5700))
					pins |= LL_GPIO_PIN_12;
				else if (a.x > 5700)
					pins |= LL_GPIO_PIN_9;
				else if (a.x < -5700)
					pins |= LL_GPIO_PIN_13;
				else if (a.y > 5700)
					pins |= LL_GPIO_PIN_15;
				else if (a.y < -5700)
					pins |= LL_GPIO_PIN_11;

				uint32_t tmp = GPIOE->ODR;
				tmp &= 0xFFFF0000;
				tmp |= pins;
				GPIOE->ODR = tmp;

				/*serial_put(0xAA);
				serial_transmit(&a.x, 2);
				serial_transmit(&a.y, 2);
				serial_transmit(&a.z, 2);
				serial_put(0x55);*/
			}

			mag_t m;
			if (mag_get(&m))
			{
				serial_print_dec(m.t);
				serial_print(", ");
				serial_print_dec(m.x);
				serial_print(", ");
				serial_print_dec(m.y);
				serial_print(", ");
				serial_print_dec(m.z);
				serial_print("\r\n");
			}

		}
	}
}

uint32_t acc_get(acc_t *acc)
{
	uint8_t buf[2];

	if (!i2c_read_reg(ACC_ADDR, 0x28, &buf[0]))
		return 0;
	if (!i2c_read_reg(ACC_ADDR, 0x29, &buf[1]))
		return 0;

	acc->x = (buf[1] << 8) | (buf[0] << 0);

	if (!i2c_read_reg(ACC_ADDR, 0x2A, &buf[0]))
		return 0;
	if (!i2c_read_reg(ACC_ADDR, 0x2B, &buf[1]))
		return 0;

	acc->y = (buf[1] << 8) | (buf[0] << 0);

	if (!i2c_read_reg(ACC_ADDR, 0x2C, &buf[0]))
		return 0;
	if (!i2c_read_reg(ACC_ADDR, 0x2D, &buf[1]))
		return 0;

	acc->z = (buf[1] << 8) | (buf[0] << 0);

	return 1;
}

uint32_t mag_get(mag_t *mag)
{
	uint8_t buf[2];

	if (!i2c_read_reg(MAG_ADDR, 0x04, &buf[0]))
		return 0;
	if (!i2c_read_reg(MAG_ADDR, 0x03, &buf[1]))
		return 0;

	mag->x = (buf[1] << 8) | (buf[0] << 0);

	if (!i2c_read_reg(MAG_ADDR, 0x06, &buf[0]))
		return 0;
	if (!i2c_read_reg(MAG_ADDR, 0x05, &buf[1]))
		return 0;

	mag->y = (buf[1] << 8) | (buf[0] << 0);

	if (!i2c_read_reg(MAG_ADDR, 0x08, &buf[0]))
		return 0;
	if (!i2c_read_reg(MAG_ADDR, 0x07, &buf[1]))
		return 0;

	mag->z = (buf[1] << 8) | (buf[0] << 0);

	if (!i2c_read_reg(MAG_ADDR, 0x32, &buf[0]))
		return 0;
	if (!i2c_read_reg(MAG_ADDR, 0x31, &buf[1]))
		return 0;

	mag->t = (buf[1] << 8) | (buf[0] << 0);

	return 1;
}


void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

	LL_UTILS_PLLInitTypeDef pll;
	pll.Prediv = LL_RCC_PREDIV_DIV_2;
	pll.PLLMul = LL_RCC_PLL_MUL_16;

	LL_UTILS_ClkInitTypeDef clk;
	clk.APB1CLKDivider = LL_RCC_APB1_DIV_2;
	clk.APB2CLKDivider = LL_RCC_APB2_DIV_1;
	clk.AHBCLKDivider = LL_RCC_SYSCLK_DIV_1;

	LL_PLL_ConfigSystemClock_HSE(8000000, LL_UTILS_HSEBYPASS_OFF, &pll, &clk);

	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);
}
