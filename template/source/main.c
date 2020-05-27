
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

void SystemClock_Config(void);

typedef struct 
{
	int16_t x;
	int16_t y;
	int16_t z;
} acc_t;

uint32_t acc_get(acc_t *acc);

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

	serial_init(115200);
	i2c_init();

	serial_print("Hello world !\r\n");


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
	serial_print("\r\n");

	
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
	serial_print("I2C detection end.\r\n");
	
	uint8_t ira, irb, irc;

	if (i2c_read_reg(0x1E, 0x0A, &ira))
	{
		serial_print("IRA = ");
		serial_print_hex8(ira);
		serial_print("\r\n");
	}

	if (i2c_read_reg(0x1E, 0x0B, &irb))
	{
		serial_print("IRB = ");
		serial_print_hex8(irb);
		serial_print("\r\n");
	}

	if (i2c_read_reg(0x1E, 0x0C, &irc))
	{
		serial_print("IRC = ");
		serial_print_hex8(irc);
		serial_print("\r\n");
	}

	i2c_write_reg(0x19, 0x20, 0x97);//Normal, ODR = 1.344kHz, X,Y,Z En
	i2c_write_reg(0x19, 0x23, 0x00);//Continuous, LSB first, FS = +/-2g

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
				serial_print_dec(a.x);
				serial_print(", ");
				serial_print_dec(a.y);
				serial_print(", ");
				serial_print_dec(a.z);
				serial_print("\r\n");


				/*serial_put(0xAA);
				serial_transmit(&a.x, 2);
				serial_transmit(&a.y, 2);
				serial_transmit(&a.z, 2);
				serial_put(0x55);*/
			}
		}
	}
}

uint32_t acc_get(acc_t *acc)
{
	uint8_t buf[2];

	if (!i2c_read_reg(0x19, 0x28, &buf[0]))
		return 0;
	if (!i2c_read_reg(0x19, 0x29, &buf[1]))
		return 0;

	acc->x = (buf[1] << 8)| (buf[0] << 0);

	if (!i2c_read_reg(0x19, 0x2A, &buf[0]))
		return 0;
	if (!i2c_read_reg(0x19, 0x2B, &buf[1]))
		return 0;

	acc->y = (buf[1] << 8)| (buf[0] << 0);

	if (!i2c_read_reg(0x19, 0x2C, &buf[0]))
		return 0;
	if (!i2c_read_reg(0x19, 0x2D, &buf[1]))
		return 0;

	acc->z = (buf[1] << 8)| (buf[0] << 0);

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
