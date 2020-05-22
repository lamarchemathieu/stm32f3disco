
#include <stdio.h>

#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_rcc.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_utils.h"
#include "stm32f3xx_ll_usart.h"

void SystemClock_Config(void);
void print(const char *str);


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

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	LL_GPIO_StructInit(&gpios);
	gpios.Pin = LL_GPIO_PIN_9 | LL_GPIO_PIN_10;
	gpios.Mode = LL_GPIO_MODE_ALTERNATE;
	gpios.Pull = LL_GPIO_PULL_NO;
	gpios.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	gpios.Speed      = LL_GPIO_SPEED_FREQ_HIGH;
	gpios.Alternate  = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &gpios);

	LL_RCC_ClocksTypeDef rcc_clocks;
	LL_RCC_GetSystemClocksFreq(&rcc_clocks);
	SysTick_Config(rcc_clocks.HCLK_Frequency / 10);


	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
	LL_USART_InitTypeDef usart;
	LL_USART_StructInit(&usart);
	usart.BaudRate            = 115200;
	usart.DataWidth           = LL_USART_DATAWIDTH_8B;
	usart.StopBits            = LL_USART_STOPBITS_1;
	usart.Parity              = LL_USART_PARITY_NONE;
	usart.TransferDirection   = LL_USART_DIRECTION_TX_RX;
	usart.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	usart.OverSampling        = LL_USART_OVERSAMPLING_16;
	LL_USART_Init(USART1, &usart);
	LL_USART_Enable(USART1);

	LL_USART_EnableIT_RXNE(USART1);

	NVIC_EnableIRQ(USART1_IRQn);

	char tmp[64];

	sprintf(tmp, "SYSCLK_Frequency = %ld\r\n", rcc_clocks.SYSCLK_Frequency);
	print(tmp);
	sprintf(tmp, "HCLK_Frequency = %ld\r\n", rcc_clocks.HCLK_Frequency);
	print(tmp);
	sprintf(tmp, "PCLK1_Frequency = %ld\r\n", rcc_clocks.PCLK1_Frequency);
	print(tmp);
	sprintf(tmp, "PCLK2_Frequency = %ld\r\n", rcc_clocks.PCLK2_Frequency);
	print(tmp);

	print("Hello world !\r\n");

	while (1)
	{
		
	}
}

void SysTick_Handler(void)
{
	LL_GPIO_TogglePin(GPIOE, LL_GPIO_PIN_8);
}

void USART1_IRQHandler(void)
{
	if(LL_USART_IsActiveFlag_RXNE(USART1) && LL_USART_IsEnabledIT_RXNE(USART1))
	{
		uint8_t c = LL_USART_ReceiveData8(USART1);

		while(LL_USART_IsActiveFlag_TC(USART1) == 0)
		{

		}
		LL_USART_TransmitData8(USART1, c);

	}

	LL_GPIO_TogglePin(GPIOE, LL_GPIO_PIN_9);
}


void SystemClock_Config(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

	if (LL_RCC_HSE_IsReady() == 0)
	{
		LL_RCC_HSE_Enable(); 
		while(LL_RCC_HSE_IsReady() != 1)
		{
		};
	}

	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_2, LL_RCC_PLL_MUL_16);

	LL_RCC_PLL_Enable();
	while(LL_RCC_PLL_IsReady() != 1) 
	{
	};

	/* Sysclk activation on the main PLL */
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{
	};

	/* Set APB1 & APB2 prescaler*/
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

	LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_SYSCLK);

	LL_SetSystemCoreClock(64000000);
}

void print(const char *str)
{
	uint32_t i=0;

	while(str[i])
	{
		while(LL_USART_IsActiveFlag_TC(USART1) == 0)
		{

		}
		LL_USART_TransmitData8(USART1, str[i]);
		i++;
	}
}