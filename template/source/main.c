
#include <stdio.h>

#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_utils.h"
#include "stm32f3xx_ll_usart.h"
#include "serial.h"

void SystemClock_Config(void);


int main(void)
{
	LL_GPIO_InitTypeDef gpios;

	SystemClock_Config();

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE);
	LL_GPIO_StructInit(&gpios);
	gpios.Pin = LL_GPIO_PIN_8 | LL_GPIO_PIN_9 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11 | LL_GPIO_PIN_12 | LL_GPIO_PIN_13 | LL_GPIO_PIN_14 | LL_GPIO_PIN_15;
	gpios.Mode = LL_GPIO_MODE_OUTPUT;
	gpios.Pull = LL_GPIO_PULL_NO;
	gpios.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpios.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
	LL_GPIO_Init(GPIOE, &gpios);

	serial_init(115200);

	LL_RCC_ClocksTypeDef rcc_clocks;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);
	SysTick_Config(rcc_clocks.HCLK_Frequency / 10);

	serial_print("Hello world !\r\n");

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

	while (1)
	{
		
	}
}

void SysTick_Handler(void)
{
	LL_GPIO_TogglePin(GPIOE, LL_GPIO_PIN_8);
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
